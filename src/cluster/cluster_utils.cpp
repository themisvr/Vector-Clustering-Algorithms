#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>

#include "../../include/io_utils/io_utils.h"
#include "../../include/cluster/cluster_utils.h"
#include "../../include/cluster/assignment.h"


void cluster_usage(const char *exec) {
    fprintf(stderr, "\nUsage: %s \n\n"
                        "[+] -i [input_file]\n"
                        "[+] -c [configuration_file]\n"
                        "[+] -o [output_file]\n"
                        "[+] -m [assignment method]\n"
                        "\nProvide all the above arguments\n", exec); 

    exit(EXIT_FAILURE);
}


void parse_cluster_args(int argc, char * const argv[], cluster_args *args) {
    
    int opt;
    std::string input, output, config;

    while ((opt = getopt(argc, argv, "i:c:o:m:")) != -1) {
        switch(opt) {
            case 'i':
                if ( !file_exists(optarg) ) {
                    std::cerr << "\n[+]Error: Input file does not exist!\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                args->input_file = optarg;
                break;
                        
            case 'c':
                if ( !file_exists(optarg) ) {
                    std::cerr << "\n[+]Error: Configuration file does not exist!\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                args->config_file = optarg; 
                break;

            case 'o':
                // convention: if the output file does not exist, create one on the working directory
                if( file_exists(optarg) ) 
                    args->output_file = optarg;
                else {
                    ofstream out("./output");
                    args->output_file = "output";
                }
                break;

            case 'm':
                args->method = optarg;
                break;

            default: 
                // one or more of the "-x" options did not appear
                cluster_usage(argv[0]);
                break;
        }
    }
}


size_t binary_search(const std::vector<std::pair<float, size_t>> &partial_sums, float val)
{

    size_t middle = 0, begin = 0, end = partial_sums.size();
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


bool in(const std::vector<size_t> &centroid_indexes, size_t index)
{
          for (size_t j = 0; j != centroid_indexes.size(); ++j) {
                 if (centroid_indexes[j] == index)
                     return true;
             }
 
             return false;
}


bool compare(const std::pair<float, size_t> &p1, const std::pair<float, size_t> &p2) 
{
    return p1.first < p2.first;
}
