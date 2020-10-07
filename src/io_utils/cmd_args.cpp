#include "../../include/io_utils/cmd_args.h"

using namespace std;

Prog_args::Prog_args(const string &ipath, const string &qpath, const string &opath, uint16_t hfun_num, uint16_t htabl_num, uint16_t nn_num, \
                    float rad) : input_file_path(ipath), query_file_path(qpath), output_file_path(opath), hash_functions_num(hfun_num), \
                                    hash_tables_num(htabl_num), nearest_neighbors_num(nn_num), radius(rad)
{

}

string Prog_args::get_input_file_path() const
{
    return input_file_path;
}

string Prog_args::get_query_file_path() const
{
    return query_file_path;
}

string Prog_args::get_output_file_path() const
{
    return output_file_path;
}

uint16_t Prog_args::get_hash_functions_num() const
{
    return hash_functions_num;
}

uint16_t Prog_args::get_hash_tables_num() const
{
    return hash_tables_num;
}

uint16_t Prog_args::get_nearest_neighbors_num() const
{
    return nearest_neighbors_num;
}

float Prog_args::get_radius() const
{
    return radius;
}
