#include <iostream>
#include <string>
#include <cstdint>
#include "../../include/io_utils/cmd_args.h"
#include "../../include/io_utils/io_utils.h"

using namespace std;

int main(int argc, char *argv[])
{
    Lsh_args *args = nullptr;


    if(argc == 1) { // user executed the program with no command line arguments i.e ./lsh
        string input_file = user_prompt_file("Enter path to input file: ");
        args = new Lsh_args(input_file);
        cout << args->get_input_file_path() << endl;
    }
    else {          // user executed the program using command line arguments
        parse_args_lsh(argc, argv, &args);

        /* for testing */
        cout << args->get_input_file_path() << endl;
        cout << args->get_query_file_path() << endl;
        cout << args->get_output_file_path() << endl;
        cout << args->get_k() << endl;
        cout << args->get_hash_tables_num() << endl;
        cout << args->get_nearest_neighbors_num() << endl;
        cout << args->get_radius() << endl;

    }

    // store the images
    vector<vector<uint8_t>> pixels = read_dataset(args->get_input_file_path());
    cout << pixels.size() << endl;

    // create lsh (search) structure...
    

    // if user executed the program with no command line options, do some io
    if(args->get_query_file_path().empty()) {
        // tha ta valw se mia sinartisi ola ta parakatw sto io.utils.cpp
        string query_file = user_prompt_file("Enter path to query file: ");
        args->set_query_file_path(query_file);

        string output_file = user_prompt_file("Enter path to output file: ");
        args->set_output_file_path(output_file);

        // prompt user for k, L, N, R
        uint16_t nearest_neighbors = user_prompt_search_arg("Enter number of nearest neighbors: ");
        args->set_nearest_neighbors_num(nearest_neighbors);
        float rad = user_prompt_rad("Enter search radius: ");
        args->set_radius(rad);
        uint16_t hashfun_number = user_prompt_search_arg("Enter number of hash functions: ");
        args->set_hash_functions_num(hashfun_number);
        uint16_t hashtabl_number = user_prompt_search_arg("Enter number of hash tables / amplified hash functions: ");
        args->set_hash_tables_num(hashtabl_number);
    }

    // start executing query search

    

    delete args;

    exit(EXIT_SUCCESS);
}
