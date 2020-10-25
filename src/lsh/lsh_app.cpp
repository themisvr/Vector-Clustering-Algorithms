#include <iostream>
#include <string>
#include <cstdint>
#include <chrono>
#include <sys/time.h>

#include "../../include/io_utils/cmd_args.h"
#include "../../include/io_utils/io_utils.h"
#include "../../include/modules/lsh/lsh.h"
#include "../../include/modules/exact_nn/exact_nn.h"


#define C 1.2


static void start_lsh_simulation(Lsh_args *args) {

    /* Create LSH structure */
    uint16_t L = args->get_hash_tables_num();
    uint16_t N = args->get_nearest_neighbors_num();
    uint32_t K = args->get_k();
    double R = args->get_radius();

    /* read training set and store each data sample */
    std::cout << "\nReading training set..." << std::endl;
    std::vector<std::vector<uint8_t>> dataset;
    read_file<uint8_t> (args->get_input_file_path(), dataset);
    std::cout << "Done!" << std::endl;

    std::cout << "\nComputing mean nearest neighbor distance..." << std::endl;
    double r = mean_nn_distance<uint8_t> (dataset); 
    std::cout << "Done!" << std::endl;

    std::cout << "\nCreating LSH structure..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    LSH<uint8_t> lsh = LSH<uint8_t> (L, N, K, r, dataset);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start); 
    std::cout << "LSH structure creation lasted " << duration.count() << " seconds" << std::endl;


    /* read test set and store each query point */
    std::vector<std::vector<uint8_t>> queries;
    read_file<uint8_t> (args->get_query_file_path(), queries);

    /********** Start ANN / ENN / Range search **********/
    std::vector<std::vector<std::pair<uint32_t, size_t>>> ann_results(queries.size(), \
                                                            std::vector<std::pair<uint32_t, size_t>> (args->get_nearest_neighbors_num()));

    std::vector<std::vector<uint32_t>>          enn_distances(queries.size(), \
                                                    std::vector<uint32_t> (args->get_nearest_neighbors_num()));

    std::vector<std::vector<size_t>>            range_results(queries.size());
    std::vector<uint64_t>           ann_query_times(queries.size());
    std::vector<uint64_t>           enn_query_times(queries.size());

    for (size_t i = 0; i != queries.size(); ++i) {

        struct timeval end_t, start_t;
        uint64_t millis;

        /* Approximate K-NN calculation */
        gettimeofday(&start_t, NULL);
        ann_results[i] = lsh.approximate_k_nn(queries[i]);
        gettimeofday(&end_t, NULL);
        millis = ((end_t.tv_sec * (uint64_t)1000 + (end_t.tv_usec / 1000)) - (start_t.tv_sec * (uint64_t)1000 + (start_t.tv_usec / 1000)));
        ann_query_times[i] = millis;

        /* Exact NN calculation */
        gettimeofday(&start_t, NULL);
        enn_distances[i] = exact_nn<uint8_t> (dataset, queries[i], args->get_nearest_neighbors_num());
        gettimeofday(&end_t, NULL);
        millis = ((end_t.tv_sec * (uint64_t)1000 + (end_t.tv_usec / 1000)) - (start_t.tv_sec * (uint64_t)1000 + (start_t.tv_usec / 1000)));
        enn_query_times[i] = millis;

        /* Range Search */
       // range_results[i] = lsh.approximate_range_search(C, R, queries[i]);
    }

    std::cout << "\nCreating the output format file..." << std::endl;
    write_output(args->get_output_file_path(), args->get_nearest_neighbors_num(), queries.size(), \
                            ann_results, ann_query_times, enn_distances, enn_query_times, range_results, "LSH");
    std::cout << "Done!" << std::endl;

    delete args;
}


static void user_interface(Lsh_args **args) {

    std::string input_file, query_file, output_file;
    std::string exit = "";

    if (*args == nullptr) {
        while (exit != "N") {
            input_file = user_prompt_file("Enter path to input file: ");
            query_file = user_prompt_file("Enter path to query file: ");
            output_file = user_prompt_file("Enter path to output file: ");
            *args = new Lsh_args(input_file, query_file, output_file);
            
            start_lsh_simulation(*args);
            
            exit = user_prompt_exit("\nDo you want to continue the simulation with another dataset/trainset: [Y/N]: ");
        }
    }
}


int main (int argc, char *argv[]) {
    
    Lsh_args *args = nullptr;

    /* user executed the program using command line arguments */
    if (argc == 15) {
        lsh_parse_args(argc, argv, &args);
        start_lsh_simulation(args);
    }
    /* user executed the program with no command line arguments i.e ./lsh */
    else if (argc == 1) {
        user_interface(&args);

    }
    else { lsh_usage(argv[0]); }

    return EXIT_SUCCESS;
}
