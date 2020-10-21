#include <iostream>
#include <vector>
#include <string>
#include <utility>

#include "../../include/io_utils/io_utils.h"
#include "../../include/cluster/cluster_utils.h"

#include "../../include/cluster/cluster.h"

int main(int argc, char *argv[])  {

    cluster_args args;  
    Cluster<uint8_t> *cluster = nullptr;

    /* 
     * ./cluster -i <input file> -c <config file> -o <output file> -complete <optional>
     *           -m <method: Classic or LSH or Hypercube>
     *
     * Configuration file includes: - number_of_clusters            <int>
     *                              - number_of_hash_tables L       <int>
     *                              - number_of_hash_functions k    <int>
     *                              - max_number_M_hypercube        <int>
     *                              - hypercube_dimensions d'       <int>
     *                              - number_of_probes              <int>
     *
     */
    if (argc != 9) cluster_usage(argv[0]);

    
    parse_cluster_args(argc, argv, &args);

    //parse_config() -> a) prosthese sto cluster_args pedia gia clusters, L, k, M, d', probes kai kalese parse_config(&args) ή 
    //                  b) epestrepse ta me references sta orismata: parse_config(&num_clusters, &num_hash_tables, &num_hfunctions, ...) 
    //
    //                  Kanto opws thes!

    /* doesn't work - empty string!!! */
    std::cout << args.input_file << std::endl;


    /* read training set file and store the training data */
    //std::vector<std::vector<uint8_t>> train_data = read_file<uint8_t> (args.input_file);
    std::vector<std::vector<uint8_t>> train_data = read_file<uint8_t> ("../../datasets/train-images-idx3-ubyte");


    /* based on the assignment method specified by the user, use the appropriate constructor for Cluster */
    if (args.method == "Classic") {
        //cluster = new Cluster<uint8_t> (clusters_num);
    }
    else if (args.method == "LSH") {
        /* calculate mean nearest neighbor distance of the training set */
        //double r = mean_nn_distance<uint8_t> (train_data);
        //cluster = new Cluster<uint8_t> (clusters_num, L, 0, K, r, train_data);
    }
    else {
        double r = mean_nn_distance<uint8_t> (train_data);
        //cluster = new Cluster<uint8_t> (clusters_num, cube_dims, M, probes, 0, 0.0, train_data.size(), train_data[0].size(), r, train_data);
        cluster = new Cluster<uint8_t> (10, 14, 10, 2, 0, 0.0, train_data.size(), train_data[0].size(), r, train_data);
    }

    auto start = std::chrono::high_resolution_clock::now();
    cluster->k_medians_plus_plus(train_data, args.method);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    cluster->silhouette(train_data);
    cluster->write_cluster_output("output", "Hypercube", true, duration);

    delete cluster;

    return EXIT_SUCCESS;
}
