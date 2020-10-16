#ifndef LSH_H
#define LSH_H


#include <iterator>
#include <vector>
#include <unordered_map>
#include <utility>

#include "../../hash_function/hash_function.h"
#include "../../metric/metric.h"

#define HT_SIZE(N) ((N / 8))
#define MULTIPLE 10

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
        /* Radius for Range Search */
        const double R;
        /* Size of each hash table */
        size_t ht_size;
        /* Number of vectors given in the dataset */
        size_t n_vectors;
        /* Window size */
        double w;

        std::vector<std::vector<T>> dataset;

        std::vector<std::unordered_multimap<int, std::pair<std::vector<T>, size_t>>> lsh_tables;
        
        std::vector<HashFunction<T>> hash_functions;


        void initialize_k_best_vectors(std::vector<std::pair<uint32_t, size_t>> &k_best_vectors) {
            
            auto best_dist = std::numeric_limits<uint32_t>::max();

            for (size_t i = 0; i != N; ++i) {
                k_best_vectors.push_back(std::make_pair(best_dist, 0));
            }
        }



    public:

        LSH(    const uint16_t &L, const uint16_t &N, \
                const uint32_t &K, const double &R, const double &meandist, \
                std::vector<std::vector<T>> &input) : L(L), N(N), K(K), R(R), dataset(input) {
            
            D = dataset[0].size();
            n_vectors = dataset.size();
            ht_size = HT_SIZE(n_vectors);
            M = 1ULL << (32 / K);
            m = (1ULL << 32) - (5);
            w = meandist * MULTIPLE;


            uint64_t amplified_value{};

            for (size_t i = 0; i != L; ++i) {
                hash_functions.emplace_back(HashFunction<T>(K, D, m, M, w));
            }

            for (size_t i = 0; i != L; ++i) {                
                std::unordered_multimap<int, std::pair<std::vector<T>, size_t>> hash_table{};

                for (size_t index = 0; index != n_vectors; ++index) {           
                    amplified_value = hash_functions[i].amplified_function_construction(dataset[index]);
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
                af_value = hash_functions[i].amplified_function_construction(query);
                bucket = fast_mod(af_value, ht_size);
                auto it = lsh_tables[i].equal_range(bucket);


                /* Finds a range containing all elements whose key is the number of bucket in the multimap */
                for (auto item = it.first; item != it.second; ++item) {
                    bucket_item = item->second;
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


        std::vector<size_t> approximate_range_search(const double &c, const std::vector<T> &query) {

            std::vector<size_t> result;
            uint64_t af_value{};
            uint32_t bucket{};
            uint32_t items_checked = 0;

            std::pair<std::vector<T>, size_t> bucket_item;

            for (size_t i = 0; i != L; ++i) {
                af_value = hash_functions[i].amplified_function_construction(query);
                bucket = fast_mod(af_value , ht_size);
                auto it = lsh_tables[i].equal_range(bucket);

                for (auto item = it.first; item != it.second; ++item) {
                    items_checked++;
                    bucket_item = item->second;
                    if (manhattan_distance_rd<T>(bucket_item.first, query) < (c * R)) {
                        result.emplace_back(bucket_item.second);
                    }
                    // if (items_checked > 20 * L) { return result; }
                }    
            }
            return result;
        };

};


#endif // LSH_H