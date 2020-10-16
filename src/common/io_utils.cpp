#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <chrono>
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


void cube_usage(const char *exec) {
    fprintf(stderr, "\nUsage: %s \n"
                        "[+] -d [input_file]\n"
                        "[+] -q [query_file]\n"
                        "[+] -k [projection_dimension]\n"
                        "[+] -M [max_candidates]\n"
                        "[+] -probes [max_probes]\n"
                        "[+] -o [output_file]\n"
                        "[+] -N [nearest_neighbors_number]\n"
                        "[+] -R [radius]\n"
                        "\nProvide all the above arguments\n",exec); 

    exit(EXIT_FAILURE);
}


string user_prompt_exit(const string &message) {

    string exit;

    cout << message ;
    cin >> exit;

    return exit;
}


string user_prompt_file(const string &message) {
    std::string file_path;

    std::cout << message ;
    std::cin >> file_path; 

    return file_path;
}


uint16_t user_prompt_search_arg(const string &message)
{
    uint16_t search_arg = 0;

    cout << message;
    cin >> search_arg;

    return search_arg;
}


float user_prompt_rad(const string &message)
{
    float radius = 0.0;

    cout << message;
    cin >> radius;

    return radius;
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


void cube_parse_args(int argc, char * const argv[], Cube_args **args) {
    int opt = 0, projection_dimension = 0, max_candidates = 0, max_probes = 0, nn_num = 0;
    float rad = 0.0;
    std::string dataset_file, query_file, output_file;

    while ( (opt = getopt(argc, argv, "d:q:k:M:p:o:N:R:")) != -1 ) {
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
                projection_dimension = atoi(optarg);
                if (projection_dimension < 1) {
                    std::cerr << "\n[+]Error: -k must be >= 1\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;

            case 'M':
                max_candidates = atoi(optarg);
                if (max_candidates < 1) {
                    std::cerr << "\n[+]Error: -M must be >= 1\n" << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;

            case 'p':
                max_probes = atoi(argv[optind]);
                if (max_probes < 1) {
                    std::cerr << "\n[+]Error: -probes must be >= 1\n" << std::endl;
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
                cube_usage(argv[0]);
                break;
        }
    }

    /* if -k -M -probes were not specified, use default values */
    if (projection_dimension == 0 && max_candidates == 0 && max_probes == 0) {
        projection_dimension = 3;
        max_candidates = 10;
        max_probes = 2;
        nn_num = 1;
        rad = 1.0;
    }
    *args = new Cube_args(dataset_file, query_file, output_file, nn_num, rad, projection_dimension, max_candidates, max_probes); 
}


void user_interface(Cube_args **args) {

    std::string input_file, query_file, output_file; 

    if (*args == nullptr) {
        input_file = user_prompt_file("Enter path to input file: ");
        query_file = user_prompt_file("Enter path to query file: ");
        output_file = user_prompt_file("Enter path to output file: ");

        *args = new Cube_args(input_file, query_file, output_file);
    }
}


void user_interface(Cube_args *args) {
    uint32_t projection_dimension;
    uint16_t nearest_neighbors, max_candidates, max_probes;
    float radius;

    nearest_neighbors    = user_prompt_search_arg("Enter number of nearest neighbors: ");
    radius               = user_prompt_rad("Enter search radius: ");
    projection_dimension = user_prompt_search_arg("Enter hypercube dimension: ");
    max_candidates       = user_prompt_search_arg("Enter max candidate points to be checked: ");
    max_probes           = user_prompt_search_arg("Enter max hypercube vertices to be ckecked: ");

    args->set_nearest_neighbors_num(nearest_neighbors);
    args->set_radius(radius);
    args->set_projection_dim(projection_dimension);
    args->set_max_candidates(max_candidates);
    args->set_max_probes(max_probes);
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


void write_output(const string &out, const uint16_t nns, const size_t size, \
                            const vector<vector<pair<uint32_t, size_t>>> &ann_res, \
                            const vector<chrono::seconds> &ann_query_times, \
                            const vector<vector<uint32_t>> &enn_dists, const vector<chrono::seconds> &enn_query_times, \
                            const vector<vector<size_t>> &range_res, const string &structure) {
    
    vector<pair<uint32_t, size_t>> approx_nearest;
    vector<uint32_t> exact_nearest;
    ofstream ofile;
    ofile.open(out, ios::out | ios::app);

    for (size_t i = 0; i != size; ++i) {
        approx_nearest = ann_res[i];
        exact_nearest  = enn_dists[i];
        ofile << "Query: " << i << endl;
        for (size_t j = 0; j != nns; ++j) {
            ofile << "Nearest neighbor-" << j + 1 << ": " << approx_nearest[j].second << endl;
            ofile << "distance" << structure << ": " << approx_nearest[j].first << endl;
            ofile << "distanceTrue: " << exact_nearest[j] << endl;
        }
        ofile << "t" << structure << ": " << ann_query_times[i].count() << endl;
        ofile << "tTrue: " << enn_query_times[i].count() << endl;

        ofile << "R-near neighbors:" << endl;
        for (auto &c : range_res[i]) {
            ofile << c << endl;
        }
    }

    ofile.close();
}