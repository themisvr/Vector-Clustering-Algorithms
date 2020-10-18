#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <map>

#include "../../include/modules/lsh/lsh.h"
#include "../../include/metric/metric.h"

#define C 1.8


template<typename T>
std::map<std::vector<T>, std::vector<size_t>> lloyds_assignment(   const std::vector<std::vector<T>> &dataset, 
                                    const std::vector<std::pair<std::vector<T>, size_t>> &centroids) {

    /*  each cluster has a centroid and its data assigned to it
        our cluster representation is a map that keys are centroids (unique)
        and values are vectors assigned to their centroid
    */
    std::map<std::vector<T>, std::vector<size_t>> clusters;

    uint32_t min_dist{};
    uint32_t dist{};

    /* for each point compute l1 metric distance to every centroid */
    for (size_t i = 0; i != dataset.size(); ++i)  {
        min_dist = std::numeric_limits<uint32_t>::max();
        std::vector<T> best_centroid{};
        for (size_t j = 0; j != centroids.size(); ++j) {
            dist = manhattan_distance_rd<T> (dataset[i], centroids[j].first);
            if (dist < min_dist) {
                min_dist = dist;
                best_centroid = centroids[j].first;
            }
        }
        auto it = clusters.find(best_centroid);
        if (it == clusters.end()) {
            std::vector<size_t> vector_ids;
            vector_ids.emplace_back(i);
            clusters.insert(std::make_pair(best_centroid, vector_ids));
        }
        else {
            (it->second).emplace_back(i);
        }
    }

    return clusters;
}

template<typename T>
static inline void assign_vector_to_centroids( const std::map<std::vector<T>, std::vector<size_t>> &clusters, 
                                                const std::vector<std::pair<std::vector<T>, size_t>> &centroids, const int &best_centroid,
                                                const size_t &vector_index) {

    /* check if the current centroid already exists in the map of clusters */
    auto it = clusters.find(centroids[best_centroid].first);
    /* if it doesnt exist, create a new centroid with its assigned vector_id (indexes to the dataset)*/
    if (it == clusters.end()) {
        std::vector<size_t> vector_ids;
        vector_ids.emplace_back(vector_index);
        clusters.insert(std::make_pair(centroids[best_centroid].first, vector_ids));
    }
    /* if it does exist, then add the vector to its centroid in the map of clusters */
    else {
        (it->second).emplace_back(vector_index);
    }
}

template<typename T>
std::map<std::vector<T>, std::vector<size_t>> lsh_reverse_assignment(   const std::vector<std::vector<T>> &dataset, LSH<T> *lsh,
                                                                        const std::vector<std::pair<std::vector<T>, size_t>> &centroids) {

    size_t n_centroids, n_vectors;
    double radius;
    uint32_t dist{}, min_dist{};
    std::map<int, int> assigned_vectors;

    /*  each cluster has a centroid and its data assigned to it
        our cluster representation is a map that keys are centroids (unique)
        and values are vectors assigned to their centroid
    */
    std::map<std::vector<T>, std::vector<size_t>> clusters;

    n_vectors = dataset.size();
    n_centroids = centroids.size();

    /* at the beggining mark each vector as unassigned (-1) */
    for (size_t i = 0; i != n_vectors; ++i) {
        assigned_vectors[i] = -1;
    }

    /* calculate min distance between centers */
    min_dist = std::numeric_limits<uint32_t>::max();
    for (size_t i = 0; i != n_centroids; ++i) {
        for (size_t j = i + 1; j != n_centroids; ++j) {
            dist = manhattan_distance_rd<T> (centroids[i].first, centroids[j].first);
            if (dist < min_dist) {
                min_dist = dist;
            }
        }
    }

    /* start with min(dist between centers)/2 */
    radius = (double) (min_dist / 2);
    // TODO: stop condition ??
    while (...) {
        /* for each centroid c, range/ball queries centered at c */
        for (size_t i = 0; i != n_centroids; ++i) {
            auto res = lsh->approximate_range_search(C, radius, centroids[i].first);
            for (const auto &vector_index: res) {
                /* The case where the vector is not assigned to a cluster */ 
                if (assigned_vectors[vector_index] == -1) {
                    assign_vector_to_centroids(clusters, centroids, i, vector_index);
                    /* mark the vector as "assigned" to a cluster */
                    assigned_vectors[vector_index] = i;
                }
                /*  
                    In this case, the vector has been assigned to another centroid before,
                    so compare its distances to the respective centroids, assign to closest centroid.     
                */
                else {
                    int assigned_centroid = assigned_vectors[vector_index];
                    uint32_t assigned_dist = manhattan_distance_rd<T> (dataset[vector_index], centroids[assigned_centroid].first);
                    // TODO: calculate previous dist
                    if (vec_dist < assigned_dist) {
                        auto it = clusters.find(centroids[assigned_centroid].first);
                        (it->second).emplace_back(vector_index);
                        /* mark the vector as "assigned" to a cluster */
                        assigned_vectors[vector_index] = i;
                    }
                }
            }       
        }
        /* multiply radius by 2 */
        radius *= 2;
    }
    /* At end: for every unassigned point, compare its distances to all centroids */
    for (size_t i = 0; i != n_vectors; ++i) {
        if (assigned_vectors[i] == -1) {
            min_dist = std::numeric_limits<uint32_t>::max();
            int best_centroid{};
            for (size_t j = 0; j != n_centroids; ++j) {
                uint32_t dist = manhattan_distance_rd(dataset[i], centroids[j].first);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_centroid = j;
                }
            }
            assign_vector_to_centroid(clusters, centroids, best_centroid, i);
            assigned_vectors[i] = best_centroid;
        }
    }
    return clusters;
}




#endif // ASSIGNMENT_H