#ifndef CLUSTER_UTILS_H
#define CLUSTER_UTILS_H

#include <string>
#include <vector>
#include <utility>


typedef struct cluster_args {
    std::string input_file;
    std::string config_file;
    std::string output_file;
    std::string method;
    bool complete;
} cluster_args;


void cluster_usage(const char *exec);

void parse_cluster_args(int argc, char * const argv[], cluster_args *args);

void normalize_distances(std::vector<float> &);

size_t binary_search(const std::vector<std::pair<float, size_t>> &, float);

float find_max(const std::vector<float> &);

bool compare(const std::pair<float, size_t> &, const std::pair<float, size_t> &);

bool in(const std::vector<size_t> &, size_t);

#endif // CLUSTER_UTILS_H
