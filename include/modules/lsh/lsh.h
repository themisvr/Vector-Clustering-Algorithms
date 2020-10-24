#ifndef LSH_H
#define LSH_H


#include <iterator>
#include <vector>
#include <unordered_map>
#include <utility>
#include <map>
#include <set>

#include "../../hash_function/hash_function.h"
#include "../../metric/metric.h"

#define HT_SIZE(N) ((N / 8))
#define MULTIPLE 4

template <typename T>
class LSH {

    private:
        /* Useful Constants for the LSH structure */
        uint32_t m;
        uint32_t M;
        /* Number of hash tables */
        const uint16_t L;
        /* Number of nearest neighbours */
        const uint16_t N;
        /* Number of h hash functions */
        const uint32_t K;
        /* Vector dimension */
        uint32_t D;
        /* Size of each hash table */
        size_t ht_size;
        /* Number of vectors given in the dataset */
        size_t n_vectors;
        /* Window size */
        double w;

        std::vector<std::vector<T>> dataset;

        std::vector<std::unordered_multimap<int, std::pair<std::vector<T>, size_t>>> lsh_tables;
        
        std::vector<AmplifiedHashFunction<T>> g_hash_functions;

        std::map<uint64_t, std::set<size_t>> g_values;


        void initialize_k_best_vectors(std::vector<std::pair<uint32_t, size_t>> &k_best_vectors) {
            
            auto best_dist = std::numeric_limits<uint32_t>::max();

            for (size_t i = 0; i != N; ++i) {
                k_best_vectors.push_back(std::make_pair(best_dist, 0));
            }
        }


        void map_vec_to_g_value(uint64_t amplified_value, size_t vec_index) {
            
            auto it = g_values.find(amplified_value);
            if (it != g_values.end()) {
                it->second.insert(vec_index);
            }
            else {
                std::set<size_t> g_vec_ids;
                g_vec_ids.insert(vec_index);
                g_values.insert(std::make_pair(amplified_value, g_vec_ids));
            }
        }


    public:

        LSH(    const uint16_t &L, const uint16_t &N, \
                const uint32_t &K, const double &meandist, \
                const std::vector<std::vector<T>> &input) : L(L), N(N), K(K), dataset(input) {
            
            D = dataset[0].size();
            n_vectors = dataset.size();
            ht_size = HT_SIZE(n_vectors);
            M = 1ULL << (32 / K);
            m = (1ULL << 32) - (5);
            // up to 4000 results are not so go good
            // w = meandist * MULTIPLE;
            // best results till now with w = 4000
            w = 4000;


            uint64_t amplified_value{};

            for (size_t i = 0; i != L; ++i) {
                g_hash_functions.emplace_back(AmplifiedHashFunction<T>(K, D, m, M, w));
            }

            for (size_t i = 0; i != L; ++i) {                
                std::unordered_multimap<int, std::pair<std::vector<T>, size_t>> hash_table{};

                for (size_t index = 0; index != n_vectors; ++index) {           
                    amplified_value = g_hash_functions[i].amplified_function_construction(dataset[index]);
                    map_vec_to_g_value(amplified_value, index);
                    hash_table.insert(std::make_pair(amplified_value % ht_size, std::make_pair(dataset[index], index)));
                }
                lsh_tables.emplace_back(hash_table);
            }
        };


        ~LSH() = default;


        std::vector<std::pair<uint32_t, size_t>> approximate_k_nn(const std::vector<T> &query) {
            
            std::vector<std::pair<uint32_t, size_t>> k_best_vectors;
            uint64_t af_value{};
            uint32_t dist{};
            uint32_t bucket{};
            uint32_t items_checked = 0;

            initialize_k_best_vectors(k_best_vectors);

            std::pair<std::vector<T>, size_t> bucket_item;

            for (size_t i = 0; i != L; ++i) {
                af_value = g_hash_functions[i].amplified_function_construction(query);
                auto map_it = g_values.find(af_value);
                if (map_it == g_values.end()) {
                    continue;
                }
                bucket = fast_mod(af_value, ht_size);
                auto it = lsh_tables[i].equal_range(bucket);


                /* Finds a range containing all elements whose key is the number of bucket in the multimap */
                for (auto item = it.first; item != it.second; ++item) {
                    bucket_item = item->second;
                    if (map_it->second.find(bucket_item.second) == map_it->second.end()) continue;
                    dist = manhattan_distance_rd<T>(bucket_item.first, query);
                    items_checked++;
                    if (dist < k_best_vectors[0].first) {
                        k_best_vectors[0] = std::make_pair(dist, bucket_item.second);
                        std::sort(k_best_vectors.begin(), k_best_vectors.end(), [](const std::pair<uint32_t, size_t> &left, \
                                                                                    const std::pair<uint32_t, size_t> &right) { \
                            return left.first > right.first;
                        });
                    }
                    // if (items_checked > 10 * L) return k_best_vectors;
                }
            }
            std::sort(k_best_vectors.begin(), k_best_vectors.end(), [](const std::pair<uint32_t, size_t> &left, \
                                                                                    const std::pair<uint32_t, size_t> &right) { \
                            return left.first < right.first;
                        });
            return k_best_vectors;
        }


        std::vector<size_t> approximate_range_search(const double &c, const double &r, const std::vector<T> &query) {

            std::vector<size_t> result;
            uint64_t af_value{};
            uint32_t bucket{};
            uint32_t items_checked = 0;

            std::pair<std::vector<T>, size_t> bucket_item;

            for (size_t i = 0; i != L; ++i) {
                af_value = g_hash_functions[i].amplified_function_construction(query);
                bucket = fast_mod(af_value , ht_size);
                auto map_it = g_values.find(af_value);
                if (map_it == g_values.end()) {
                    continue;
                }
                auto it = lsh_tables[i].equal_range(bucket);

                for (auto item = it.first; item != it.second; ++item) {
                    items_checked++;
                    bucket_item = item->second;
                    if (map_it->second.find(bucket_item.second) == map_it->second.end()) continue;
                    if (manhattan_distance_rd<T>(bucket_item.first, query) < (c * r)) {
                        result.emplace_back(bucket_item.second);
                    }
                    // if (items_checked > 20 * L) { return result; }
                }    
            }
            return result;
        };

};


#endif // LSH_H