#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include "../../include/io_utils/cmd_args.h"

using namespace std;


inline bool file_exists(const char *filepath)
{
    struct stat buf;

    return ( stat(filepath, &buf) == 0 );

}


void parse_args_lsh(int argc, char * const argv[], Prog_args **args)
{
    int opt, hfunc_num, htabl_num, nn_num;
    string dataset_file, query_file, output_file;
    float rad;

    while( (opt = getopt(argc, argv, "d:q:k:L:o:N:R:")) != -1 )
    {
        switch(opt) 
        {
            case 'd':
                        if( !file_exists(optarg) ) {
                            cerr << "\nInput file does not exist!\n" << endl;
                            exit(EXIT_FAILURE);
                        }
                        dataset_file = optarg;
                        break;
                        
            case 'q':
                        if( !file_exists(optarg) ) {
                            cerr << "\nQuery file does not exist!\n" << endl;
                            exit(EXIT_FAILURE);
                        }
                        query_file = optarg; 
                        break;

            case 'k':
                        hfunc_num = atoi(optarg);
                        break;

            case 'L':
                        htabl_num = atoi(optarg);
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
                        break;

            case 'R':
                        rad = atof(optarg);
                        break;

            default: 
                        // one or more of the "-x" options did not appear
                        fprintf(stderr, "\nUsage: %s -d <input file> -q <query file> -k <hash functions number> -L <hash tables number> "
                                                        "-o <output file> -N <nearest neighbors number> -R <radius>\n\n", argv[0]);
                        exit(EXIT_FAILURE);
                        break;

        }
    }

    // if -k and -L options did not appear, use default values
    if(hfunc_num == 0 && htabl_num == 0) { 
        hfunc_num = 4; // k = 4
        htabl_num = 5; // L = 5
        nn_num = 1;    // N = 1
        rad = 1.0;    // R = 1.0
    }
    *args = new Prog_args(dataset_file, query_file, output_file, hfunc_num, htabl_num, nn_num, rad); // allocate a pointer to a Prog_args object
}


//void parse_args_cube() {}


uint32_t bigend_to_littlend(uint32_t big_endian)
{
    uint32_t b0, b1, b2, b3;
    uint32_t little_endian;

    b0 = (big_endian & 0x000000ff) << 24U;
    b1 = (big_endian & 0x0000ff00) << 8U;
    b2 = (big_endian & 0x00ff0000) >> 8U;
    b3 = (big_endian & 0xff000000) >> 24U;

    little_endian = b0 | b1 | b2 | b3;

    return little_endian;
}


vector<vector<unsigned char>> read_dataset(const string &datapath)
{

    ifstream data;
    data.open(datapath, ios::in | ios::binary);

    if(data) {
        uint32_t magic_num, images_number, rows, cols;

        // read magic number
        data.read( (char *) &magic_num, sizeof(magic_num) );
        magic_num = bigend_to_littlend(magic_num);
        cout << magic_num << endl;
        if(magic_num != 2051) {
            cerr << "Wrong magic number!" << endl;
            exit(EXIT_FAILURE);
        }

        // read number of images
        data.read( (char *) &images_number, sizeof(images_number) );
        images_number = bigend_to_littlend(images_number);
        cout << images_number << endl;

        // read number of rows
        data.read( (char *) &rows, sizeof(rows) );
        rows = bigend_to_littlend(rows);
        cout << rows << endl;

        // read number of columns
        data.read( (char *) &cols, sizeof(cols) );
        cols = bigend_to_littlend(cols);
        cout << cols << endl;

        /* images_number images, and each image is of dimension rows * columns
         * each pixel takes values from [0, 255]
         */
        vector< vector<unsigned char> > pixels(images_number, vector<unsigned char>(rows * cols, 0));
        for(auto i = pixels.begin(); i != pixels.end(); ++i) 
            for(auto j = i->begin(); j != i->end(); ++j) {
                unsigned char temp;
                data.read( (char *) &temp, sizeof(temp) );
                *j = temp;
            }

        return pixels;
                
    }
    else {
        cerr << "Could not open the file containing the dataset!" << endl;
        exit(EXIT_FAILURE);
    }
}
