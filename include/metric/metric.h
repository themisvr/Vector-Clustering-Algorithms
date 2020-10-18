#ifndef METRIC_H
#define METRIC_H

#include <iterator>
#include <vector>
#include <cassert>
#include <iostream>
#include <limits>
#include <map>


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
}


template <typename T>
inline uint32_t mean_nn_distance(const std::vector<std::vector<T>> &dataset) {
    
    uint32_t dist{};
    uint32_t sum{};
    uint32_t best_dist = std::numeric_limits<uint32_t>::max();

    for (size_t i = 0; i != dataset.size() / 1000; ++i) {

        for (size_t j = 0; j != dataset.size() / 1000; ++j) {

            if (i == j) continue;

            dist = manhattan_distance_rd<T>(dataset[i], dataset[j]);
            if (dist < best_dist) best_dist = dist;
        }
        sum += best_dist;
        best_dist = std::numeric_limits<uint32_t>::max();

    }

    return ( sum / ( dataset.size() / 1000) );
}


template<typename T>
std::pair<std::vector<double>, double> silhouette(  const std::vector<std::vector<T>> &dataset, 
                                                    const std::vector<std::pair<std::vector<T>, size_t>> &centroids,
                                                    const std::map<std::vector<T>, std::vector<size_t>> &clusters) {

    size_t n_vectors = dataset.size();

    std::vector<double> s(n_vectors);
    std::vector<double> a(n_vectors);
    std::vector<double> b(n_vectors);

    /* compute a[i] values */
    for (auto it = clusters.cbegin(); it != clusters.cend(); ++it) {
        std::vector<size_t> each_cluster_vector_indexes = it->second;
        for (size_t i = 0; i != each_cluster_vector_indexes.size(); ++i) {
            size_t total_a_dist{};
            for (size_t j = i; j != each_cluster_vector_indexes.size(); ++j) {
                if (dataset[each_cluster_vector_indexes[i]] == dataset[each_cluster_vector_indexes[j]]) continue;
                total_a_dist += manhattan_distance_rd<T> (dataset[each_cluster_vector_indexes[i]], dataset[each_cluster_vector_indexes[j]]);
            }
            if (each_cluster_vector_indexes.size() > 1) {
                a[each_cluster_vector_indexes[i]] = (double) total_a_dist / each_cluster_vector_indexes.size() - 1; 
            }
            else {
                a[each_cluster_vector_indexes[i]] = (double) total_a_dist;
            }
        }
    }
    /* compute closest centroid to each centroid */
    std::map<std::vector<T>, std::vector<T>> closest_centroids;
    for (size_t i = 0; i != centroids.size(); ++i) {
        uint32_t min_dist = std::numeric_limits<uint32_t>::max();
        std::vector<T> closest;
        for (size_t j = 0; j != centroids.size(); ++j) {
            if (centroids[i].first == centroids[j].first) continue;
            uint32_t dist = manhattan_distance_rd<T> (centroids[i].first, centroids[j].first);
            if (dist < min_dist) {
                min_dist = dist;
                closest = centroids[j].first;
            }
        }
        closest_centroids[centroids[i]] = closest;
    }
    /* compute b[i] values */
    for (auto it = clusters.cbegin(); it != clusters.cend(); ++it) {
        std::vector<size_t> each_cluster_vector_indexes = it->second;
        std::vector<size_t> closest_cluster_vector_indexes = clusters[closest_centroids[it->first]];
        for (size_t i = 0; i != each_cluster_vector_indexes.size(); ++i) {
            size_t total_b_dist{};
            for (size_t j = 0; j != closest_cluster_vector_indexes.size(); ++j) {
                total_b_dist += manhattan_distance_rd<T> (dataset[each_cluster_vector_indexes[i]], dataset[closest_cluster_vector_indexes[j]]);
            }
            if (closest_cluster_vector_indexes.size() > 0) {
                b[each_cluster_vector_indexes[i]] = (double) total_b_dist / each_cluster_vector_indexes.size(); 
            }
            else {
                b[each_cluster_vector_indexes[i]] = (double) total_b_dist;
            }
        }
    }
    /* compute s[i] values */
    for (size_t i = 0; i != s.size(); ++i) {
        s[i] = (b[i] - a[i]) / std::max(a[i], b[i]);
    }
    /* compute average s(p) of points in cluster i */
    std::vector<double> avg_sp(centroids.size(), 0);
    for (size_t i = 0; i != centroids.size(); ++i) {
        std::vector<size_t> each_cluster_vector_index = clusters[centroids[i].first];
        size_t n_vectors = each_cluster_vector_index.size();
        for (size_t i = 0; i != n_vectors; ++i) {
            avg_sp[i] += s[each_cluster_vector_index[i]];
        }
        if (n_vectors != 0) {
            avg_sp[i] /= n_vectors;
        }
    }
    /* compute stotal = average s(p) of points in dataset */
    double total_sp{};
    uint32_t n_centroids = centroids.size();

    for (size_t i = 0; i != n_centroids; ++i) {
        total_sp += avg_sp[i];
    }
    total_sp /= n_centroids;

    return std::make_pair(avg_sp, total_sp);
}





#endif // METRIC_H
