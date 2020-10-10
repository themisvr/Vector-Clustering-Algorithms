#ifndef MATH_UTILS_H
#define MATH_UTILS_H


#include <iostream>
#include <cstdint>
#include <vector>

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
double vectors_mean_distance(const std::vector<std::vector<T>> &dataset) {
    double mean{};

    for (size_t i = 0; i != 5; ++i) {
        std::cout << i << std::endl;
        for (size_t j = i+1; j != dataset.size(); ++j) {
            mean += (double) manhattan_distance_rd<T>(dataset[i], dataset[j]);
        }
    }

    return (mean / dataset.size());
}


#endif // MATH_UTILS_H