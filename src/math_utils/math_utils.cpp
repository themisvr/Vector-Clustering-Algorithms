#include <iostream>

#include "../../include/math_utils/math_utils.h"
#include "../../include/metric/metric.h"


double vectors_mean_distance(const std::vector<std::vector<uint8_t>> &dataset) {
    double mean{};

    for (size_t i = 0; i != dataset.size() - 1; ++i) {
        for (size_t j = i+1; j != dataset[i].size(); ++j) {
            mean += (double) manhattan_distance_rd(dataset[i], dataset[j]);
        }
    }

    return (mean / dataset.size());
}