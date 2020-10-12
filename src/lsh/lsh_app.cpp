#include <iostream>
#include <string>
#include <cstdint>
#include <chrono>

#include "../../include/io_utils/cmd_args.h"
#include "../../include/io_utils/io_utils.h"
#include "../../include/modules/lsh/lsh.h"


#define C 0.2


int main (int argc, char *argv[]) {
    
    Lsh_args *args = nullptr;

    /* user executed the program using command line arguments */
    if (argc == 15) {
        lsh_parse_args(argc, argv, &args);
    }
    /* user executed the program with no command line arguments i.e ./lsh */
    else if (argc == 1) {
        user_interface(&args);

    }
    else { lsh_usage(argv[0]); }

    /* Store the images */
    std::vector<std::vector<uint8_t>> dataset = read_file<uint8_t> (args->get_input_file_path());
    std::vector<std::vector<uint8_t>> queries = read_file<uint8_t> (args->get_query_file_path());


    /* Create lsh (search) structure */
    uint16_t L = args->get_hash_tables_num();
    uint16_t N = args->get_nearest_neighbors_num();
    uint32_t K = args->get_k();
    double R = args->get_radius();

    auto start = std::chrono::high_resolution_clock::now();
    LSH<uint8_t> lsh = LSH<uint8_t> (L, N, K, R, dataset);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start); 
    std::cout << "Time taken by inserting dataset into the LSH structure: " << duration.count() << " seconds" << std::endl; 

    /* Start executing Approximate Nearest Neighbor */
    std::vector<uint8_t> ann_result;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i != queries.size(); ++i) {
        ann_result = lsh.Approximate_NN(queries[i]);
    }
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "Time taken by Approximate-NN: " << duration.count() << " seconds" << std::endl;

    /* Start executing Approximate Range Search */
    std::vector<std::vector<uint8_t>> ars_result;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i != queries.size(); ++i) {
        ars_result = lsh.Approximate_Range_Search(C, queries[i]);
    }
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "Time taken by Approximate-Range-Search: " << duration.count() << " seconds" << std::endl;

    /* Start executing Approximate K Nearest Neighbor */
    std::vector<std::pair<std::vector<uint8_t>, uint32_t>> k_ann_result;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i != queries.size(); ++i) {
        k_ann_result = lsh.Approximate_K_NN(queries[i]);
    }
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "Time taken by Approximate-K-Nearest-Neighbor: " << duration.count() << " seconds" << std::endl;


    delete args;

    return EXIT_SUCCESS;
}
