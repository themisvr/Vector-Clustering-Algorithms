#include <iostream>
#include <string>
#include <vector>

#include "../../include/modules/hypercube/hypercube.h"
#include "../../include/io_utils/cmd_args.h"
#include "../../include/io_utils/io_utils.h"

using namespace std;


int main(int argc, char *argv[])
{
    Cube_args *args = nullptr;

    if (argc == 1) {    // user executed the program without command line options i.e ./cube
        string input_file_path = user_prompt_file("Enter input file path: ");
        args = new Cube_args(input_file_path);
    }
    else {              // user executed the program with command line options
        cube_parse_args(argc, argv, &args);

        /* testing */
        cout << args->get_input_file_path() << endl;
        cout << args->get_query_file_path() << endl;
        cout << args->get_output_file_path() << endl;
        cout << args->get_k() << endl;
        cout << args->get_max_candidates() << endl;
        cout << args->get_max_probes() << endl;
        cout << args->get_nearest_neighbors_num() << endl;
        cout << args->get_radius() << endl;

    }

    /* read training set and store each data sample */
    cout << "\nReading training set..." << endl;
    vector<vector<uint8_t>> training_samples = read_file<uint8_t> (args->get_input_file_path());
    cout << "Done!" << endl;


    cout << "\nComputing mean dataset distance..." << endl;
    double r = mean_nearest_distance<uint8_t> (training_samples);
    cout << "Done!" << endl;


    /* If search args were not specified up to this point, prompt user (d', M, probes)
     * Then, create Hypercube structure
     */
    cout << "\nCreating Hypercube structure..." << endl;
    auto start = std::chrono::high_resolution_clock::now();
    Hypercube<uint8_t> cube(args->get_k(), args->get_max_candidates(), args->get_max_probes(), \
                                                    args->get_nearest_neighbors_num(), args->get_radius(), training_samples.size(), \
                                                    training_samples[0].size(), r, training_samples);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    cout << "Done!" << endl;
    cout << duration.count() << " secs to create hypercube structure and to store all points" << endl;



    // at this point, if ./cube was run with no cmd args, prompt user for query file, output file
    


    /* read test set and store each query point */
    vector<vector<uint8_t>> test_samples = read_file<uint8_t> (args->get_query_file_path());


    /* ANN */
    for (size_t i = 0; i != test_samples.size() / 1000; ++i) {
       vector<std::pair<uint32_t, size_t>> nearest = cube.approximate_nn(test_samples[i]); 
       args->write_ann_output(nearest, i);
    }

    /* Range Search */


    delete args;

    exit(EXIT_SUCCESS);
}
