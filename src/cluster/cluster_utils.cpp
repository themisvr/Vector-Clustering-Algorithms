#include <iostream>
#include <string>
#include <unistd.h>

#include "../../include/io_utils/io_utils.h"
#include "../../include/cluster/cluster_utils.h"
#include "../../include/cluster/assignment.h"


void cluster_usage(const char *exec) {
    fprintf(stderr, "\nUsage: %s \n\n"
                        "[+] -i [input_file]\n"
                        "[+] -c [configuration_file]\n"
                        "[+] -o [output_file]\n"
                        "[+] -m [assignment method]\n"
                        "\nProvide all the above arguments\n", exec); 

    exit(EXIT_FAILURE);
}


void parse_cluster_args(int argc, char * const argv[], cluster_args *args) {
    
    int opt;
    std::string input, output, config;

    while ((opt = getopt(argc, argv, "i:c:o:m:")) != -1) {
        switch(opt) {
            case 'i':
                if ( !file_exists(optarg) ) {
                    std::cerr << "\n[+]Error: Input file does not exist!\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                args->input_file = optarg;
                break;
                        
            case 'c':
                if ( !file_exists(optarg) ) {
                    std::cerr << "\n[+]Error: Configuration file does not exist!\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                args->config_file = optarg; 
                break;

            case 'o':
                // convention: if the output file does not exist, create one on the working directory
                if( file_exists(optarg) ) 
                    args->output_file = optarg;
                else {
                    ofstream out("./output");
                    args->output_file = "output";
                }
                break;

            case 'm':
                args->method = optarg;
                break;

            default: 
                // one or more of the "-x" options did not appear
                cluster_usage(argv[0]);
                break;
        }
    }
}