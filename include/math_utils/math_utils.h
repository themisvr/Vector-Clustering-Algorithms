#ifndef MATH_UTILS_H
#define MATH_UTILS_H


#include <iostream>
#include <cstdint>
#include <vector>
#include <limits>

#include "../../include/metric/metric.h"

inline uint64_t exp_modulo(uint32_t base, uint16_t exp, uint32_t mod) {
    uint64_t res = 1;

    while (exp > 0) {
        if (exp & 1) { res = (res * base) % mod; }
        exp >>= 1;
        base = (base * base) % mod;
    }
    return res;
}

inline int modulo(int i, int n) {
    return (i % n + n) % n;
}


template <typename T>
double mean_nearest_distance(const std::vector<std::vector<T>> &dataset) {
    double mean{};
    uint32_t dist{};
    
    uint32_t min_dist = std::numeric_limits<uint32_t>::max();

    for (size_t i = 0; i != dataset.size() - 1; ++i) {
        for (size_t j = i+1; j != dataset.size(); ++j) {
            if ((dist = manhattan_distance_rd<T>(dataset[i], dataset[j])) < min_dist) {
                min_dist = dist;
            }
        }
        mean += min_dist;
    }

    return (mean / dataset.size());
}


#endif // MATH_UTILS_H