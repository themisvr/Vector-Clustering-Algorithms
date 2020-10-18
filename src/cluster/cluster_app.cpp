#include <iostream>
#include <vector>
#include <string>
#include <utility>


#include "../../include/io_utils/io_utils.h"
#include "../../include/cluster/cluster_utils.h"
#include "../../include/cluster/initialization.h"


int main(int argc, char *argv[])  {

    cluster_args args;

    if (argc != 9) cluster_usage(argv[0]);

    parse_cluster_args(argc, argv, &args);

    std::cout << args.input_file << std::endl;

    /* read training set file and store the training data */
    std::vector<std::vector<uint8_t>> train_data = read_file<uint8_t> ("../../datasets/train-images-idx3-ubyte");

    std::vector<std::pair<std::vector<uint8_t>, size_t>> centroids = init_plus_plus(train_data, 10);


    return EXIT_SUCCESS;
}
