#include <vector>
#include <iostream>
#include <fstream>

#include "cmd_args.h"

string user_prompt_file(const string&);

uint16_t user_prompt_search_arg(const string&);

float user_prompt_rad(const string&);

uint32_t bigend_to_littlend(uint32_t);

inline bool file_exists(const char *);

void lsh_parse_args(int, char * const [], Lsh_args **);

void cube_parse_args(int, char * const [], Cube_args **);

void lsh_usage(const char *);

void cube_usage(const char *);

void user_interface(Lsh_args **);


template <typename T>
vector<vector<T>> read_file(const string& datapath) {
    ifstream data;
    data.open(datapath, ios::in | ios::binary);

    if(data) {
        uint32_t magic_num, images_number, rows, cols;

        // read magic number
        data.read( (char *) &magic_num, sizeof(magic_num) );
        magic_num = bigend_to_littlend(magic_num);

        // read number of images
        data.read( (char *) &images_number, sizeof(images_number) );
        images_number = bigend_to_littlend(images_number);

        // read number of rows
        data.read( (char *) &rows, sizeof(rows) );
        rows = bigend_to_littlend(rows);

        // read number of columns
        data.read( (char *) &cols, sizeof(cols) );
        cols = bigend_to_littlend(cols);

        /* images_number images, and each image is of dimension rows * columns
         * each pixel takes values from [0, 255]
         * "flatten" and store i-th image to the i-th element of the vector
         */
        std::vector<std::vector<T> > pixels(images_number, std::vector<T>(rows * cols, 0));
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
