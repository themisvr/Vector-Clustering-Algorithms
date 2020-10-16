#include <iostream>
#include <vector>

#include "../../include/io_utils/io_utils.h"


int main(int argc, char *argv[]) 
{

    Cluster_args *args = nullptr;

    /* if -complete is specified argc is 10; otherwise argc is 9 */
    if ( argc == 9 || argc == 10 ) {

        cluster_parse_args(&args);
    }
    else {

        cluster_usage(argv[0]);
    }


    /* read training set file and store the training data */
    std::vector<std::vector<uint8_t>> train_data = read_file<uint8_t> (args->get_input_file());


    delete args;

    exit(EXIT_SUCCESS);

}
