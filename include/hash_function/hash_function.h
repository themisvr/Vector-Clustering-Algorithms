#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>

#include "../math_utils/math_utils.h"


namespace hash {

    template <typename T>
    class HashFunction {

        /* 
            params:
            
        */
        private:
            const uint16_t k;
            const uint16_t d;
            const uint32_t m;
            const uint32_t M;
            const double w;
            std::vector<double> s_transformations;
            std::vector<double> a;

        public:

            HashFunction(   const uint16_t &k, const uint16_t &d, const uint32_t &m, const uint32_t &M, 
                            const uint32_t &w) : k(k), d(d), m(m), M(M), w(w), s_transformations(d), a(d) {

                std::default_random_engine generator;
                std::uniform_real_distribution<double> distribution(0.0, w);
                
                for (size_t i = 0; i != d; ++i) {
                    s_transformations[i] = distribution(generator);
                }
            } 
            
            ~HashFunction() = default;

            uint32_t hash_function_construction(std::vector<double> &pixels) {
                uint32_t hash_value = 0;

                for (size_t i = 0; i != d; ++i) {
                    a[i] = floor((pixels[i] - s[i]) / w)
                }

                std::reverse(a.begin(), a.end());

                for (size_t i = 0; i != d; ++i) {
                    hash_value += (math_utils::modulo(a[i], i) * math_utils::exp_modulo(m, i, M)) % M;
                }

                return hash_value % M;
            }

            uint64_t amplified_function_construction(std::vector<uint32_t> hash_values) {
                std::string res = "";
                char *p_end = nullptr;
                
                for (size_t i; i != k; ++i) {
                    res += std::to_string(hash_values[i]);
                }

                return std::strtoull(res.c_str(), &p_end, 10);
            };

    };


}