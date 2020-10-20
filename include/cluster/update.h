#include <vector>
#include <cmath>
#include <utility>
#include <cassert>
#include <algorithm>


template<typename T>
void median_update(const std::vector<std::vector<T>> &train_data, std::vector<std::pair<std::vector<T>, size_t>> &centers, \
                    const std::vector<std::vector<size_t>> &clusters)
{
    assert(centers.size() == clusters.size());

    const size_t        num_clusters = clusters.size();
    const size_t        dim          = centers[0].first.size();
    std::vector<T>      components;

    for (size_t k = 0; k != num_clusters; ++k) {

        std::vector<T> &k_centroid = centers[k].first;
        size_t cluster_size = clusters[k].size(); 
        components.resize(cluster_size);

        for (size_t d = 0; d != dim; ++d) {

            for (size_t t = 0; t != cluster_size; ++t) {

                const std::vector<size_t> &cluster_indexes = clusters[k]; 
                const std::vector<T> &t_vector = train_data[cluster_indexes[t]];
                components[t] = t_vector[d];
            }
            std::sort(components.begin(), components.end());
            size_t median_index = std::ceil(cluster_size / 2);
            k_centroid[d] = components[median_index];
        }
        centers[k].second = 0;
    }
}
