#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include "../../include/io_utils/cmd_args.h"


void lsh_usage(const char *exec) {
    fprintf(stderr, "\nUsage: %s \n"
                        "[+] -d [input_file]\n"
                        "[+] -q [query_file]\n"
                        "[+] -k [hash_functions_number]\n"
                        "[+] -L hash_tables_number\n"
                        "[+] -o [output_file]\n"
                        "[+] -N [nearest_neighbors_number]\n"
                        "[+] -R [radius]\n"
                        "\nProvide all the above arguments\n",exec); 

    exit(EXIT_FAILURE);
}


string user_prompt_file(const string &message) {
    std::string file_path;

    std::cout << message ;
    std::cin >> file_path; 

    return file_path;
}


inline bool file_exists(const char *filepath) {
    struct stat buf;

    return ( stat(filepath, &buf) == 0 );

}


void lsh_parse_args(int argc, char * const argv[], Lsh_args **args) {
    
    int opt{};
    uint32_t hfunc_num{};
    uint16_t htabl_num{}, nn_num{};
    std::string dataset_file, query_file, output_file;
    float rad{};

    while ( (opt = getopt(argc, argv, "d:q:k:L:o:N:R:")) != -1 ) {
        switch (opt) {
            case 'd':
                if ( !file_exists(optarg) ) {
                    std::cerr << "\n[+]Error: Input file does not exist!\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                dataset_file = optarg;
                break;
                        
            case 'q':
                if ( !file_exists(optarg) ) {
                    std::cerr << "\n[+]Error: Query file does not exist!\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                query_file = optarg; 
                break;

            case 'k':
                hfunc_num = atoi(optarg);
                if (hfunc_num < 1) {
                    std::cerr << "\n[+]Error: -k must be >= 1\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;

            case 'L':
                htabl_num = atoi(optarg);
                if (htabl_num < 1) {
                    std::cerr << "\n[+]Error: -L muste be >= 1\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;

            case 'o':
                // convention: if the output file does not exist, create one on the working directory
                if( file_exists(optarg) ) 
                    output_file = optarg;
                else {
                    ofstream out("./output");
                    output_file = "output";
                }
                break;

            case 'N':
                nn_num = atoi(optarg);
                if (nn_num < 1) {
                    std::cerr << "\n[+]Error: -N must be >= 1\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;

            case 'R':
                rad = atof(optarg);
                break;

            default: 
                // one or more of the "-x" options did not appear
                lsh_usage(argv[0]);
                break;
        }
    }
    *args = new Lsh_args(dataset_file, query_file, output_file, nn_num, rad, hfunc_num, htabl_num); 
}


void user_interface(Lsh_args **args) {

    std::string input_file, query_file, output_file; 

    if (*args == nullptr) {
        input_file = user_prompt_file("Enter path to input file: ");
        query_file = user_prompt_file("Enter path to query file: ");
        output_file = user_prompt_file("Enter path to output file: ");

        *args = new Lsh_args(input_file, query_file, output_file);
    }
}


uint32_t bigend_to_littlend(uint32_t big_endian) {
    uint32_t b0, b1, b2, b3;
    uint32_t little_endian;

    b0 = (big_endian & 0x000000ff) << 24U;
    b1 = (big_endian & 0x0000ff00) << 8U;
    b2 = (big_endian & 0x00ff0000) >> 8U;
    b3 = (big_endian & 0xff000000) >> 24U;

    little_endian = b0 | b1 | b2 | b3;

    return little_endian;
}

