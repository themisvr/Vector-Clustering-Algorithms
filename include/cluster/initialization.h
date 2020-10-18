#include <vector>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <ctime>
#include <cassert>
#include <random>

#include "../modules/exact_nn/exact_nn.h"


size_t binary_search(const std::vector<std::pair<float, size_t>> &partial_sums, size_t sz, float val)
{

    size_t middle = 0, begin = 0, end = sz;
    const std::pair<float, size_t> *less_than = &partial_sums[0];
    const std::pair<float, size_t> *greater_than = &partial_sums[0];

    while (begin <= end) {

        middle = begin + (end - begin) / 2;
        if (val == partial_sums[middle].first) {
            return partial_sums[middle].second;
        }
        if(val < partial_sums[middle].first) {
            less_than = &partial_sums[middle];
            end = middle - 1;

        }
        else {
            greater_than = &partial_sums[middle];
            begin = middle + 1;

        }
    }

    std::cout << "P(r-1) = " << greater_than->first << " < " << val << " <= P(r) = " << less_than->first << std::endl;

    return less_than->second;
}


float find_max(const std::vector<float> &min_distances)
{
    float max_dist = std::numeric_limits<float>::min();

    for (float dist : min_distances) {
        if (dist > max_dist) max_dist = dist;
    }

    return max_dist;
}


void normalize_distances(std::vector<float> &min_distances)
{
    float dmax = find_max(min_distances);

    for (float &d : min_distances)
        d /= dmax;
}


static bool compare(const std::pair<float, size_t> &p1, const std::pair<float, size_t> &p2) 
{
    return p1.first < p2.first;
}


template<typename T>
bool in(const std::vector<std::pair<std::vector<T>, size_t>> &centroids, size_t index)
{

    for (size_t j = 0; j != centroids.size(); ++j) {

        if (centroids[j].second == index) return true;

    }
    
    return false;
}


template<typename T>
std::vector<std::pair<std::vector<T>, size_t>> init_plus_plus(const std::vector<std::vector<T>> &train_samples, uint8_t clusters)
{

    std::vector<std::pair<std::vector<T>, size_t>>  centroids;
    std::vector<std::pair<float, size_t>>           partial_sums;   
    std::vector<float>                              min_distances(train_samples.size());              


    /* randomly select the index of the 1st centroid from the training set */
    srand( time(NULL) );
    size_t index = rand() % train_samples.size();
    centroids.emplace_back(train_samples[index], index);
    std::default_random_engine generator;
    

    for (size_t t = 1; t != clusters; ++t) {  
        for (size_t i = 0; i != train_samples.size(); ++i) {

            /* if training sample with index i is one of the k centroids, 
             * don't calculate the distance with itself
             */
            if ( in(centroids, i) ) continue;

            min_distances[i] = exact_nn<T> (centroids, train_samples[i]);
        }

        /* normalize D(i)'s */
        normalize_distances(min_distances);

        /* calculate n - t partial sums */
        float prev_partial_sum = 0.0;
        float new_partial_sum  = 0.0;
        partial_sums.emplace_back(0.0, 0);      // P(0) = 0
        for (size_t j = 0; j != train_samples.size(); ++j) {

            if ( in(centroids, j) ) continue;

            new_partial_sum = prev_partial_sum + (min_distances[j] * min_distances[j]);
            partial_sums.emplace_back(new_partial_sum, j);
            prev_partial_sum = new_partial_sum;
        }

        /* generate uniformly distributed x in [0, P(n - t)]
         * do binary search on the sorted vector containing pairs of (partial sum, index)
         * function binary_search() returns index r of the training sample that is the next centroid
         */
        std::uniform_real_distribution<float> distribution(0.0, new_partial_sum);
        float x = distribution(generator);
        std::sort(partial_sums.begin(), partial_sums.end(), compare);
        size_t r = binary_search(partial_sums, partial_sums.size(), x);
        std::cout << r << std::endl;
        centroids.emplace_back(train_samples[r], r);
        partial_sums.clear();   // next iteration: partial_sum's size will be decreased by one
    }

    return centroids;
}
