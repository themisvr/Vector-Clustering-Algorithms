#ifndef METRIC_H
#define METRIC_H

#include <iterator>
#include <vector>
#include <cassert>


inline uint32_t manhattan_distance_rd(std::vector<uint8_t> dataset_vector, std::vector<uint8_t> query_vector) {
    assert(dataset_vector.size() == query_vector.size());

    uint32_t dist{};

    std::vector<uint8_t>::const_iterator d_it, q_it;
    d_it = dataset_vector.begin();
    q_it = query_vector.begin();

    for (; d_it != dataset_vector.end(); ++d_it, ++q_it) {
        dist += std::abs((*d_it) - (*q_it));
    }

    return dist;
};


#endif // METRIC_H