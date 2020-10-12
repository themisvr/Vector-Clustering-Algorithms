#ifndef EXACT_NN
#define EXACT_NN

#include <iostream>
#include <utility>
#include <vector>

#include "../../../include/metric/metric.h"


template<typename T>
std::pair<std::vector<T>, uint32_t>  Exact_nn(const std::vector<std::vector<T>> &dataset, const std::vector<T> &query) {

    std::pair<std::vector<T>, uint32_t> best_vector; 
    uint32_t min_dist = std::numeric_limits<uint32_t>::max();

    for (size_t i = 0; i != dataset.size(); ++i) {
        auto dist = manhattan_distance_rd<T> (dataset[i], query);
        if (dist < min_dist) {
            best_vector = std::make_pair(dataset[i], dist);
        }
    }
    return best_vector;
}


#endif // EXACT_NN