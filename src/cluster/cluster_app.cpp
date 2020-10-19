#include <iostream>
#include <vector>
#include <string>
#include <utility>

#include "../../include/io_utils/io_utils.h"
#include "../../include/modules/lsh/lsh.h"
#include "../../include/modules/exact_nn/exact_nn.h"
#include "../../include/modules/hypercube/hypercube.h"
#include "../../include/cluster/cluster_utils.h"
#include "../../include/cluster/initialization.h"
#include "../../include/cluster/assignment.h"


int main(int argc, char *argv[])  {

    cluster_args args;

    if (argc != 9) cluster_usage(argv[0]);

    parse_cluster_args(argc, argv, &args);

    std::cout << args.input_file << std::endl;

    /* read training set file and store the training data */
    std::vector<std::vector<uint8_t>> train_data = read_file<uint8_t> ("../../datasets/train-images-idx3-ubyte");

    /* calculate mean nearest neighbor distance of the training set */
    double r = mean_nn_distance<uint8_t> (train_data);

    /* initialization++ */
    std::vector<std::pair<std::vector<uint8_t>, size_t>> centroids = init_plus_plus(train_data, 10);

    /* based on the assignment method specified by the user, create the appropriate object */
    if (args.input_file == "Classic") {
    }
    else if (args.input_file == "LSH") {
    }
    else {
        // testing
        Hypercube<uint8_t> cube(14, 10, 2, 0, 0.0, train_data.size(), train_data[0].size(), r, train_data);
        std::vector<std::vector<size_t>> clusters = hypercube_reverse_assignment(train_data, cube, centroids);

        /* get the actual centroid vector of cluster[i] by writing centroids[i].first */
        for (size_t i = 0; i != clusters.size(); ++i) {
            std::cout << "Cluster " << i + 1 << " contains " << clusters[i].size() << " points" << std::endl;
        }

    }


    return EXIT_SUCCESS;
}
