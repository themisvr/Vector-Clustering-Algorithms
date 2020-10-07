#include <iostream>
#include <string>
#include <cstdint>
#include "../../include/io_utils/cmd_args.h"
#include "../../include/io_utils/io_utils.h"

using namespace std;

int main(int argc, char *argv[])
{
    Prog_args *args = nullptr;

    parse_args_lsh(argc, argv, &args);

    /* for testing */
    cout << args->get_input_file_path() << endl;
    cout << args->get_query_file_path() << endl;
    cout << args->get_output_file_path() << endl;
    cout << args->get_hash_functions_num() << endl;
    cout << args->get_hash_tables_num() << endl;
    cout << args->get_nearest_neighbors_num() << endl;
    cout << args->get_radius() << endl;

    read_dataset(args->get_input_file_path());


    delete args;


    return 0;
}
