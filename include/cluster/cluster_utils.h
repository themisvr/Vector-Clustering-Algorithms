#ifndef CLUSTER_UTILS_H
#define CLUSTER_UTILS_H

#include <string>


typedef struct cluster_args {
    std::string input_file;
    std::string config_file;
    std::string output_file;
    std::string method;
    bool complete;
} cluster_args;


void cluster_usage(const char *exec);

void parse_cluster_args(int argc, char * const argv[], cluster_args *args);


#endif // CLUSTER_UTILS_H