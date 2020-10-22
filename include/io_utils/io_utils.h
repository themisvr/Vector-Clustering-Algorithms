#include <vector>
#include <iostream>
#include <fstream>

#include "cmd_args.h"

string user_prompt_exit(const string &);

string user_prompt_file(const string&);

uint16_t user_prompt_search_arg(const string&);

float user_prompt_rad(const string&);

uint32_t bigend_to_littlend(uint32_t);

bool file_exists(const char *);

void lsh_parse_args(int, char * const [], Lsh_args **);

void cube_parse_args(int, char * const [], Cube_args **);

void lsh_usage(const char *);

void cube_usage(const char *);

void user_interface(Cube_args **);

void user_interface(Cube_args *);

void write_output(const string &, const uint16_t, const size_t, const vector<vector<pair<uint32_t, size_t>>> &, \
                            const vector<chrono::milliseconds> &, const vector<vector<uint32_t>> &, const vector<chrono::milliseconds> &, \
                            const vector<vector<size_t>> &, const string &);


template <typename T>
void read_file(const string& filepath, vector<vector<T>> &data) {
    ifstream data_file;
    data_file.open(filepath, ios::in | ios::binary);

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
        data.resize(images_number, vector<T>(rows * cols, 0));
        for(size_t i = 0; i != images_number; ++i) {
            vector<T> &vec = data[i];
            for(size_t j = 0; j != rows * cols; ++j) {
                //unsigned char temp;
                //data.read( (char *) &temp, sizeof(temp) );
                data.read( (char *) &vec[j], sizeof(vec[j]) );
                //*j = temp;
            }
        }

        data_file.close();
    }
    else {
        std::cerr << "Could not open the file!" << std::endl;
        exit(EXIT_FAILURE);
    }
}
