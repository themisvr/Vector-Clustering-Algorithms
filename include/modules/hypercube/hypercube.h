#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <unordered_map>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>
#include <cstdlib>

#include "../../hash_function/hash_function.h"
#include "../../metric/metric.h"

#define MULTIPLE1 4
#define MULTIPLE2 10
#define C         1.1


template <typename T>
class Hypercube {

    private:

        uint32_t m;
        uint32_t M;

        /* hypercube method parameters */
        const uint32_t projection_dimension;
        const uint16_t max_candidates;
        const uint16_t max_probes;

        /* search methods parameters */
        const uint16_t N;
        const float    R;

        /* data parameters */
        const size_t   train_samples;
        const uint32_t D;
        const double   win;

        /* maintain a hash table for each f_i, where 1 <= i <= d'; each hash table stores pairs {h_i(x) : 0 or 1} */
        std::vector<std::unordered_multimap<uint32_t, bool>> uniform_binary_mapppings;

        /* hash table representing the hypercube; each vertex is basically a bucket */
        std::unordered_multimap<std::string, std::pair<std::vector<T> &, size_t>> hash_table;

        /* for the hypercube method we use d' x LSH hash functions */
        std::vector<HashFunction<T>> hash_functions;


    public:

        Hypercube (uint32_t projdim, uint16_t cands, uint16_t probes, uint16_t nns, float r, \
                    size_t trn, uint32_t d, double meandist, const std::vector<std::vector<T>> &samples) \
                    : projection_dimension(projdim), max_candidates(cands), max_probes(probes), \
                      N(nns), R(r), train_samples(trn), D(d), win(MULTIPLE1 * meandist)
        {
            std:: cout << "Window is: ";
            std::cout << win << std::endl;

            M = 1ULL << (32 / projection_dimension);
            m = (1ULL << 32) - (5);

            /* we do not use amplified hashes on this method, so we do not care about k (k = 0) */
            for (size_t i = 0; i != projection_dimension; ++i) {
                hash_functions.emplace_back( HashFunction<T> (0, D, m, M, win) );
                uniform_binary_mapppings.emplace_back(std::unordered_multimap<uint32_t, bool>());
            }

            for (size_t i = 0; i != train_samples; ++i) {
                cube_projection_train(samples[i], i);
            }
        }


        void cube_projection_train(const std::vector<T> &point, const size_t index)
        {
            uint32_t    hval;
            short       bit;
            std::string bitstring;

            for (size_t j = 0; j != projection_dimension; ++j) {
                
                hval = hash_functions[j].hash_function_construction(point);

                /* check if f_j(h_j(p)) has already been computed.
                 * positive: retrieve from data store and use that value 
                 * negative: compute f_j(h_j(x)) i.e toss a coin
                 */
                bit = retrieve_val(uniform_binary_mapppings[j], hval);

                /* this is the first time we encounter h_j, so we calculate and store f_j(h_j) */
                if (bit == -1) {
                    bit = uniform_binmap(uniform_binary_mapppings[j], hval);
                }

                bitstring += std::to_string(bit);
            }

            assert(bitstring.size() == projection_dimension);

            /* create object {point, index}; insert the object into the hash table as a pair: { key, {point, index} } */
            hash_table.insert( std::make_pair(bitstring, std::make_pair(point, index)) );   // we want to avoid creating a copy of point!
        }


        std::string cube_projection_test(const std::vector<T> &query)
        {
            uint32_t    hval;
            short       bit;
            std::string bitstring;

            for (size_t j = 0; j != projection_dimension; ++j) {
                
                hval = hash_functions[j].hash_function_construction(query);

                /* check if f_j(h_j(p)) has already been computed.
                 * positive: retrieve value from data store and use it 
                 * negative: compute f_j(h_j(x)) i.e toss a coin
                 */
                bit = retrieve_val(uniform_binary_mapppings[j], hval);

                /* this is the first time we encounter h_j, so we calculate and store f_j(h_j) */
                if (bit == -1) {
                    bit = uniform_binmap(uniform_binary_mapppings[j], hval);
                }

                bitstring += std::to_string(bit);
            }

            assert(bitstring.size() == projection_dimension);

            return bitstring;
        }


        short retrieve_val(std::unordered_multimap<uint32_t, bool> &map, uint32_t hval)
        {
            if ( !map.empty() ) {
                auto retrieved = map.find(hval);
                    if ( retrieved != map.end() ) 
                        return retrieved->second;
            }
            
            return -1;
        }


        short uniform_binmap(std::unordered_multimap<uint32_t, bool> &map, uint32_t hval)
        {
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(0,1); // uniform distribution in range [0, 1]
            short bit = dist(rng);  // generate 0 or 1
            map.insert( std::make_pair(hval, bit) ); // insert the pair {hval : f(hval)} into the hash table

            return bit;
        }


        static bool compare(const std::pair<uint32_t, size_t> &p1, const std::pair<uint32_t, size_t> &p2)
        {
            return p1.first < p2.first;
        }


        std::vector<std::pair<uint32_t, size_t>> approximate_nn(const std::vector<T> &query)
        {
            /* vector to store query's nearest neighbors - M candidates */
            std::vector<std::pair<uint32_t, size_t>> candidates;
            uint32_t dist;
            uint32_t cnt = projection_dimension;
            uint16_t M = max_candidates;
            uint16_t probes = max_probes;
            uint8_t  flag = 1;
            bool     same_keys = false;

            /* project query to a cube vertex / hash table bucket */
            const std::string key = cube_projection_test(query);
            std::string key1 = key;
            std::pair<std::vector<T>, size_t> value;

            while (M > 0) {
                if (probes > 0) {
                    auto range = hash_table.equal_range(key1); 
                    for (auto i = range.first; (i != range.second) && (M > 0); ++i, --M) {
                        same_keys = true;
                        value = i->second; // value = (vector<uint8_t>, size_t)
                        dist = manhattan_distance_rd<T>(query, value.first);
                        candidates.emplace_back(dist, value.second);
                    }

                    /* if no points with the same keys were found 
                     * don't decrement number of probes; otherwise might
                     * not find nearest neighbors at all
                     */
                    if (same_keys) --probes;
                    key1 = gen_similar_vertex(key, cnt, flag);
                    same_keys = false;
                }
                else
                    break;
            }

            std::sort(candidates.begin(), candidates.end(), compare);
            if(candidates.size() > N) candidates.resize(N); // keep only the N best candidates

            return candidates;
        }


        std::vector<size_t> range_search(const std::vector<T> &query)
        {
            /* vector to store query's nearest neighbors; only store the training index this time */
            std::vector<size_t> candidates;
            uint32_t dist;
            uint32_t cnt = projection_dimension;
            uint16_t M = max_candidates;
            uint16_t probes = max_probes;
            uint8_t  flag = 1;
            bool     same_keys = false;

            /* project query to a cube vertex / hash table bucket */
            const std::string key = cube_projection_test(query);
            std::string key1 = key;
            std::pair<std::vector<T>, size_t> value;

            while (M > 0) {
                if (probes > 0) {
                    auto range = hash_table.equal_range(key1); 
                    for (auto i = range.first; (i != range.second) && (M > 0); ++i, --M) {
                        same_keys = true;
                        value = i->second;
                        dist = manhattan_distance_rd<T>(query, value.first);
                        if (dist < C * R) {     // average distance is 20 000 - 35 000
                            candidates.emplace_back(value.second);
                        }
                    }
                    if (same_keys) --probes;
                    key1 = gen_similar_vertex(key, cnt, flag);
                    same_keys = false;
                }
                else
                    break;
            }

            return candidates;
        }


        std::vector<size_t> range_search(const std::vector<T> &query, double r)
        {
            /* vector to store query's nearest neighbors; only store the training index this time */
            std::vector<size_t> candidates;
            uint32_t dist;
            uint32_t cnt = projection_dimension;
            uint16_t M = max_candidates;
            uint16_t probes = max_probes;
            uint8_t  bits = 1;
            bool     same_keys = false;

            /* project query to a cube vertex / hash table bucket */
            const std::string key = cube_projection_test(query);
            std::string key1 = key;
            std::pair<std::vector<T>, size_t> value;

            while (M > 0) {
                if (probes > 0) {
                    auto range = hash_table.equal_range(key1); 
                    for (auto i = range.first; (i != range.second) && (M > 0); ++i, --M) {
                        same_keys = true;
                        value = i->second;
                        dist = manhattan_distance_rd<T>(query, value.first);
                        if (dist < C * r) {     // average distance is 20 000 - 35 000
                            candidates.emplace_back(value.second);
                        }
                    }
                    if (same_keys) --probes;
                    key1 = gen_similar_vertex(key, cnt, bits);
                    if (bits > projection_dimension) break;
                    same_keys = false;
                }
                else
                    break;
            }

            return candidates;
        }


        std::string gen_similar_vertex(const std::string &key, uint32_t &counter, uint8_t &bits)
        {
            std::string bitstring = key;
            for (size_t i = 0; i < bits && counter != 0; ++i, --counter) {
                bitstring[counter - 1] == '0' ? bitstring[counter - 1] = '1' : bitstring[counter - 1] = '0';
            }

            if(counter == 0) {
                counter = projection_dimension; // reset bit counter
                ++bits;
            }

            return bitstring;
        }
};

#endif
