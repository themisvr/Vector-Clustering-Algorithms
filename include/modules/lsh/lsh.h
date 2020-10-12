#ifndef LSH_H
#define LSH_H


#include <iterator>
#include <vector>
#include <unordered_map>
#include <utility>

#include "../../hash_function/hash_function.h"
#include "../../metric/metric.h"

#define HT_SIZE(N) ((N / 8))
#define MULTIPLE 20

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
        std::vector<std::unordered_multimap<int, std::vector<T>>> lsh_tables;
        std::vector<HashFunction<T>> hash_functions;

        void initialize_k_best_vectors(std::vector<std::pair<std::vector<T>, uint32_t>> &k_best_vectors) {

            auto best_dist = std::numeric_limits<uint32_t>::max();
            std::vector<T> best_vector;

            for (size_t i = 0; i != N; ++i) {
                k_best_vectors.push_back(std::make_pair(best_vector, best_dist));
            }
        }



    public:

        LSH(    const uint16_t &L, const uint16_t &N, \
                const uint32_t &K, const double &R, \
                std::vector<std::vector<T>> &input) : L(L), N(N), K(K), R(R), dataset(input) {
            
            D = dataset[0].size();
            n_vectors = dataset.size();
            ht_size = HT_SIZE(n_vectors);
            M = 1ULL << (32 / K);
            m = (1ULL << 32) - (5);
            w = R * MULTIPLE;


            uint64_t amplified_value{};

            for (size_t i = 0; i != L; ++i) {
                hash_functions.emplace_back(HashFunction<T>(K, D, m, M, w));
            }

            for (size_t i = 0; i != L; ++i) {                
                std::unordered_multimap<int, std::vector<T>> hash_table{};

                for (size_t j = 0; j != n_vectors; ++j) {           
                    amplified_value = hash_functions[i].amplified_function_construction(dataset[j]);
                    hash_table.insert(std::make_pair(amplified_value % ht_size, dataset[j]));
                }
                lsh_tables.emplace_back(hash_table);
            }
        };


        ~LSH() = default;


        std::vector<T> Approximate_NN(const std::vector<T> &query) {
            
            std::vector<T> closest_vector;
            uint64_t best_dist = std::numeric_limits<uint64_t>::max();

            uint32_t bucket{};
            uint32_t items_checked = 0;
            uint64_t af_value{};
            for (size_t i = 0; i != L; ++i) {
                af_value = hash_functions[i].amplified_function_construction(query);
                bucket = fast_mod(af_value, ht_size);
                auto it = lsh_tables[i].equal_range(bucket);

                /* Finds a range containing all elements whose key is the number of bucket in the multimap */
                for (auto item = it.first; item != it.second; ++item) {
                    uint32_t dist = manhattan_distance_rd<T>(item->second, query);
                    items_checked++;
                    if (dist < best_dist) {
                        best_dist = dist;
                        closest_vector = item->second;
                    }
                    if (items_checked > 10 * L) return closest_vector;
                }
            }
            return closest_vector;
        };


        std::vector<std::pair<std::vector<T>, uint32_t>> Approximate_K_NN(const std::vector<T> &query) {
            
            std::vector<std::pair<std::vector<T>, uint32_t>> k_best_vectors;
            uint32_t bucket{};
            uint32_t items_checked = 0;
            uint64_t af_value{};
            uint8_t k_th = 0;

            initialize_k_best_vectors(k_best_vectors);

            for (size_t i = 0; i != L; ++i) {
                af_value = hash_functions[i].amplified_function_construction(query);
                bucket = fast_mod(af_value, ht_size);
                auto it = lsh_tables[i].equal_range(bucket);

                /* Finds a range containing all elements whose key is the number of bucket in the multimap */
                for (auto item = it.first; item != it.second; ++item) {
                    uint32_t dist = manhattan_distance_rd<T>(item->second, query);
                    items_checked++;
                    if (dist < k_best_vectors[0].second) {
                        if (k_th != (N - 1)) {
                            k_best_vectors[k_th] = std::make_pair(item->second, dist);
                            k_th++;
                        }
                        else {
                            k_best_vectors[0] = std::make_pair(item->second, dist);
                            std::sort(k_best_vectors.begin(), k_best_vectors.end(), [](const std::pair<std::vector<T>,uint32_t> &left, \
                                                                                        const std::pair<std::vector<T>, uint32_t> &right) { \
                                return left.second > right.second;
                            });
                        }

                    }
                    if (items_checked > 10 * L) return k_best_vectors;
                }
            }
            return k_best_vectors;
        }


        std::vector<std::vector<T>> Approximate_Range_Search(const double &c, const std::vector<T> &query) {

            std::vector<std::vector<T>> result;
            uint32_t bucket{};
            uint64_t af_value{};
            uint32_t items_checked = 0;

            for (size_t i = 0; i != L; ++i) {
                af_value = hash_functions[i].amplified_function_construction(query);
                bucket = fast_mod(af_value , ht_size);
                auto it = lsh_tables[i].equal_range(bucket);

                for (auto item = it.first; item != it.second; ++item) {
                    items_checked++;
                    if (manhattan_distance_rd<T>(item->second, query) < (c * R)) {
                        result.emplace_back(item->second);
                    }
                    if (items_checked > 20 * L) { return result; }
                }    
            }
            return result;
        };

};


#endif // LSH_H