#ifndef EXACT_NN
#define EXACT_NN

#include <iostream>
#include <utility>
#include <vector>
#include <limits>
#include <algorithm>

#include "../../../include/metric/metric.h"


template<typename T>
std::vector<uint32_t> Exact_nn(const std::vector<std::vector<T>> &dataset, \
                                                  const std::vector<T> &query, const uint16_t nns) {

    std::vector<uint32_t> closest_distances; 

    for (size_t i = 0; i != dataset.size(); ++i) {
        auto dist = manhattan_distance_rd<T> (dataset[i], query);
        closest_distances.emplace_back(dist);
    }
    std::sort(closest_distances.begin(), closest_distances.end());
    closest_distances.resize(nns);

    return closest_distances;
}


#endif // EXACT_NN
