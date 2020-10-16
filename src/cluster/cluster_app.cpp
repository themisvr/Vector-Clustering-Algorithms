#include <iostream>
#include <vector>
#include <string>


#include "../../include/io_utils/io_utils.h"
#include "../../include/cluster/cluster_utils.h"


int main(int argc, char *argv[])  {

    cluster_args args;

    if (argc != 9) cluster_usage(argv[0]);

    parse_cluster_args(argc, argv, &args);


    /* read training set file and store the training data */
    std::vector<std::vector<uint8_t>> train_data = read_file<uint8_t> (args.input_file);



    return EXIT_SUCCESS;

}
