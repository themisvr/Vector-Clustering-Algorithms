#ifndef METRIC_H
#define METRIC_H

#include <iterator>
#include <vector>
#include <cassert>
#include <iostream>

template <typename T>
inline uint32_t manhattan_distance_rd(const std::vector<T> &dataset_vector, const std::vector<T> &query_vector) {
    assert(dataset_vector.size() == query_vector.size());
    
    uint32_t dist{};
    auto d_it = dataset_vector.cbegin();
    auto q_it = query_vector.cbegin();

    for (; d_it != dataset_vector.cend(); ++d_it, ++q_it) {
        dist += std::abs((*d_it) - (*q_it));
    }

    return dist;
};


#endif // METRIC_H