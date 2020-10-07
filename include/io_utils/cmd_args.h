#ifndef CMD_ARGS_H
#define CMD_ARGS_H

#include <string>

using namespace std;

class Prog_args
{
    private:
        const string input_file_path;
        const string query_file_path;
        const string output_file_path;
        const uint16_t hash_functions_num;
        const uint16_t hash_tables_num;
        const uint16_t nearest_neighbors_num;
        const float radius;

    public:
        Prog_args(const string &, const string &, const string &, uint16_t, uint16_t, uint16_t, float);
        string get_input_file_path() const;
        string get_query_file_path() const;
        string get_output_file_path() const;
        uint16_t get_hash_functions_num() const;
        uint16_t get_hash_tables_num() const;
        uint16_t get_nearest_neighbors_num() const;
        float get_radius() const;
};

#endif
