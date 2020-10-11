#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include "../../include/io_utils/cmd_args.h"


void usage(const char *exec) {
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
    
    int opt;
    uint32_t hfunc_num;
    uint16_t htabl_num, nn_num;
    std::string dataset_file, query_file, output_file;
    float rad;

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
                usage(argv[0]);
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


std::vector<std::vector<uint8_t>> read_dataset(const std::string &datapath) {

    ifstream data;
    data.open(datapath, ios::in | ios::binary);

    if(data) {
        uint32_t magic_num, images_number, rows, cols;

        // read magic number
        data.read( (char *) &magic_num, sizeof(magic_num) );
        magic_num = bigend_to_littlend(magic_num);
        std::cout << magic_num << std::endl;
        if(magic_num != 2051) {
            std::cerr << "Wrong magic number!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // read number of images
        data.read( (char *) &images_number, sizeof(images_number) );
        images_number = bigend_to_littlend(images_number);
        std::cout << images_number << std::endl;

        // read number of rows
        data.read( (char *) &rows, sizeof(rows) );
        rows = bigend_to_littlend(rows);
        std::cout << rows << std::endl;

        // read number of columns
        data.read( (char *) &cols, sizeof(cols) );
        cols = bigend_to_littlend(cols);
        std::cout << cols << std::endl;

        /* images_number images, and each image is of dimension rows * columns
         * each pixel takes values from [0, 255]
         * "flatten" and store i-th image to the i-th element of the vector
         */
        std::vector<std::vector<uint8_t> > pixels(images_number, std::vector<uint8_t>(rows * cols, 0));
        for(auto i = pixels.begin(); i != pixels.end(); ++i) 
            for(auto j = i->begin(); j != i->end(); ++j) {
                unsigned char temp;
                data.read( (char *) &temp, sizeof(temp) );
                *j = temp;
            }

        return pixels;
                
    }
    else {
        std::cerr << "Could not open the file containing the dataset!" << std::endl;
        exit(EXIT_FAILURE);
    }
}
