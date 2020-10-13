#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <unordered_map>
#include <vector>

#include "../../hash_function/hash_function.h"
#include "../../metric/metric.h"

#define MULTIPLE1 4
#define MULTIPLE2 10

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
        std::unordered_multimap<uint64_t, std::pair<std::vector<T>, size_t>> hash_table;

        /* for the hypercube method we use d' x LSH hash functions */
        std::vector<HashFunction<T>> hash_functions;


    public:

        Hypercube (uint32_t projdim, uint16_t cands, uint16_t probes, uint16_t nns, float r, \
                    size_t trn, uint32_t d, double meandist, std::vector<std::vector<T>> &samples) \
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
            uint64_t    key;
            uint32_t    hval;
            short       bit;// = -1;
            std::string bitstring;

            for (size_t j = 0; j != projection_dimension; ++j) {
                
                hval = hash_functions[j].hash_function_construction(point);

                //std::cout << hval << std::endl;

                /* check if f_j(h_j(p)) has already been computed.
                 * positive: retrieve from data store and use that value 
                 * negative: compute f_j(h_j(x)) i.e toss a coin
                 */
                bit = retrieve_val(uniform_binary_mapppings[j], hval);

                /* this is the first time we encounter h_j, so we calculate and store f_j(h_j) */
                if (bit == -1) {
                    bit = uniform_binmap(uniform_binary_mapppings[j], hval);
                }

                //std::cout << bit << std::endl;
                bitstring += std::to_string(bit);
                //bit = -1;
            }

            //std::cout << bitstring << std::endl;
            key = strtoull(bitstring.c_str(), nullptr, 10);

            /* create object {point, index}; insert the object into the hash table as a pair: { key, {point, index} } */
            hash_table.insert( std::make_pair(key, std::make_pair(point, index)) );   // we want to avoid creating a copy of point!
        }


        uint64_t cube_projection_test(const std::vector<T> &query)
        {
            uint64_t    key;
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

            key = strtoull(bitstring.c_str(), nullptr, 10);

            return key;
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
            std::vector<std::pair<uint32_t, size_t>> candidates(max_candidates);
            uint32_t dist;
            uint16_t M = max_candidates;

            /* project query to a cube vertex / hash table bucket */
            uint64_t key   = cube_projection_test(query);
            auto     range = hash_table.equal_range(key); 

            for (auto i = range.first; i != range.second, M > 0; ++i, --M) {
                auto pair = i->second;
                //std::cout << i->first << std::endl;
                std::cout << pair.second << std::endl;
                dist = manhattan_distance_rd(query, pair.first);
                std::cout << dist << std::endl;
                candidates.emplace_back( std::make_pair(dist, pair.second) );
            }

            /* if probes > 1, then explore nearby vertices / buckets in terms of closest (hamming) distance */
            //if ( (M > 0) && (max_probes > 1) ) {
            //    uint16_t probes = probes - 1;
            //    while (probes >= 1) {
            //        uint64_t similar_key = gen_similar_vertex(key);
            //        range = hash_table.equal_range(similar_key);

            //        for (auto i = range.first; i != range.second, M > 0; ++i, --M) {
            //            auto pair = i->second;
            //            dist = manhattan_distance_rd(query, pair.first);
            //            candidates.emplace_back( std::make_pair(dist, pair.second) );
            //        }

            //        if (M == 0) break;

            //        --probes;
            //    }
            //}

            std::sort(candidates.begin(), candidates.end(), compare);

            return candidates;
        }


        uint64_t gen_similar_vertex(uint64_t key)
        {
            static short bitpos = max_probes; 
            uint64_t new_key(key);

            key ^= (-1 ^ key) & (1UL << bitpos);
            if(new_key != key) {
                --bitpos;
                return key;
            }
            else {
                key ^= (-0 ^ key) & (1UL << bitpos--);

                return key;
            }
        }
};

#endif
