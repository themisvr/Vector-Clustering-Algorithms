#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <sys/time.h>

#include "../../include/modules/hypercube/hypercube.h"
#include "../../include/io_utils/cmd_args.h"
#include "../../include/io_utils/io_utils.h"
#include "../../include/modules/exact_nn/exact_nn.h"

using namespace std;

using high_resolution_clock = std::chrono::steady_clock;

int main(int argc, char *argv[])
{

    Cube_args *args = nullptr;


    if (argc == 1) {            // user executed the program without command line options i.e ./cube
        user_interface(&args);
    }
    else if (argc == 17) {      // user executed the program with command line options
        cube_parse_args(argc, argv, &args);
    }
    else {                      // invalid execution
        cube_usage(argv[0]);
    }


    /* read training set and store each data sample */
    cout << "\nReading training set..." << endl;
    vector<vector<uint8_t>> training_samples = read_file<uint8_t> (args->get_input_file_path());
    cout << "Done!" << endl;


    cout << "\nComputing mean nearest neighbor distance..." << endl;
    double r = mean_nn_distance<uint8_t> (training_samples); //1077.6875; 
    cout << "Done!" << endl;


    /* If search args were not specified up to this point, prompt user (d', M, probes, N, R)
     * Then, create Hypercube structure
     */
    if (args->get_nearest_neighbors_num() == 0) {
        user_interface(args);
    }

    cout << "\nCreating Hypercube structure..." << endl;
    auto start = std::chrono::high_resolution_clock::now();
    Hypercube<uint8_t> cube(args->get_k(), args->get_max_candidates(), args->get_max_probes(), \
                                                    args->get_nearest_neighbors_num(), args->get_radius(), training_samples.size(), \
                                                    training_samples[0].size(), r, training_samples);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    cout << "Done!" << endl;
    cout << "Hypercube structure creation lasted " << duration.count() << " seconds" << endl;


    /* read test set and store each query point */
    vector<vector<uint8_t>> test_samples = read_file<uint8_t> (args->get_query_file_path());


    /********** Start ANN / ENN / Range search **********/

    vector<vector<pair<uint32_t, size_t>>> ann_results(test_samples.size(), \
                                                        vector<pair<uint32_t, size_t>> (args->get_nearest_neighbors_num()));

    vector<vector<uint32_t>>               enn_distances(test_samples.size(), \
                                                        vector<uint32_t> (args->get_nearest_neighbors_num()));

    vector<vector<size_t>>                 range_results(test_samples.size());
    vector<uint64_t>                ann_query_times(test_samples.size());
    vector<uint64_t>                enn_query_times(test_samples.size());

    for (size_t i = 0; i != test_samples.size(); ++i) {

        struct timeval end_t, start_t;
        uint64_t millis;

        /* Approximate K-NN calculation */
        gettimeofday(&start_t, NULL);
        ann_results[i] = cube.approximate_nn(test_samples[i]);
        gettimeofday(&end_t, NULL);
        millis = ((end_t.tv_sec * (uint64_t)1000 + (end_t.tv_usec / 1000)) - (start_t.tv_sec * (uint64_t)1000 + (start_t.tv_usec / 1000)));
        ann_query_times[i] = millis;

        /* Exact NN calculation */
        gettimeofday(&start_t, NULL);
        enn_distances[i] = exact_nn<uint8_t> (training_samples, test_samples[i], args->get_nearest_neighbors_num());
        gettimeofday(&end_t, NULL);
        millis = ((end_t.tv_sec * (uint64_t)1000 + (end_t.tv_usec / 1000)) - (start_t.tv_sec * (uint64_t)1000 + (start_t.tv_usec / 1000)));
        enn_query_times[i] = millis;

        /* Range Search */
        start = std::chrono::high_resolution_clock::now();
        range_results[i] = cube.range_search(test_samples[i]);
        stop = std::chrono::high_resolution_clock::now();
    }

    write_output(args->get_output_file_path(), args->get_nearest_neighbors_num(), test_samples.size(), \
                            ann_results, ann_query_times, enn_distances, enn_query_times, range_results, "Hypercube");


    delete args;

    return EXIT_SUCCESS;
}
