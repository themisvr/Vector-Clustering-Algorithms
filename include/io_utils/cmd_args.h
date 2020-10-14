#ifndef CMD_ARGS_H
#define CMD_ARGS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

class Prog_args // abstract class
{
    private:
        const string input_file_path;
        string query_file_path;
        string output_file_path;
        uint16_t nearest_neighbors_num = 0;
        float radius = 0.0;

    public:
        Prog_args(const string &, string &, string &, uint16_t, float);
        Prog_args(const string &);
        virtual ~Prog_args() = default;
        void set_query_file_path(const string &);
        void set_output_file_path(const string &);
        void set_nearest_neighbors_num(uint16_t);
        void set_radius(float);
        string get_input_file_path() const;
        string get_query_file_path() const;
        string get_output_file_path() const;
        uint16_t get_nearest_neighbors_num() const;
        float get_radius() const;
        
        virtual uint32_t get_k() const = 0; // pure virtual function
};


class Lsh_args: public Prog_args
{
    private:
        uint32_t hash_functions_num = 0; // k
        uint16_t hash_tables_num = 0;   // L

    public:
        Lsh_args(const string &, string &, string &, uint16_t, float, uint32_t, uint16_t);
        Lsh_args(const string &);
        /* Constructor with default values */
        Lsh_args(const string &, string &, string &);
        void set_hash_functions_num(uint16_t);
        void set_hash_tables_num(uint16_t);
        uint32_t get_k() const;
        uint16_t get_hash_tables_num() const;
};


class Cube_args: public Prog_args
{
    private:
        uint32_t projection_dim = 0; // d' is specified as the -k option in the command line
        uint16_t max_candidates = 0; // M
        uint16_t max_probes = 0;    // probes

    public:
        Cube_args(const string &, string &, string &, uint16_t, float, uint32_t, uint16_t, uint16_t);
        Cube_args(const string &);
        void set_projection_dim(uint32_t);
        void set_max_candidates(uint16_t);
        void set_max_probes(uint16_t);
        void write_ann_output(const size_t, const vector<pair<uint32_t, size_t>> &, const chrono::seconds &);
        uint32_t get_k() const;
        uint16_t get_max_candidates() const;
        uint16_t get_max_probes() const;
};


inline Prog_args::Prog_args::Prog_args(const string &ipath, string &qpath, string &opath, uint16_t nn_num, float rad) : 
                            input_file_path(ipath), query_file_path(qpath), output_file_path(opath), nearest_neighbors_num(nn_num), radius(rad)
{ }

inline Prog_args::Prog_args(const string &ipath) : input_file_path(ipath)
{ }
                                    
inline void Prog_args::set_query_file_path(const string &qpath)
{
    query_file_path = qpath;
}

inline void Prog_args::set_output_file_path(const string &opath)
{
    output_file_path = opath;
}

inline void Prog_args::set_nearest_neighbors_num(uint16_t nns)
{
    nearest_neighbors_num = nns;
}

inline void Prog_args::set_radius(float rad)
{
    radius = rad;
}

inline string Prog_args::get_input_file_path() const
{
    return input_file_path;
}

inline string Prog_args::get_query_file_path() const
{
    return query_file_path;
}

inline string Prog_args::get_output_file_path() const
{
    return output_file_path;
}

inline uint16_t Prog_args::get_nearest_neighbors_num() const
{
    return nearest_neighbors_num;
}

inline float Prog_args::get_radius() const
{
    return radius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Lsh_args::Lsh_args(const string &ipath, string &qpath, string &opath, uint16_t nn_num, float rad, uint32_t hfunc_num, uint16_t htabl_num)
    : Prog_args(ipath, qpath, opath, nn_num, rad), hash_functions_num(hfunc_num), hash_tables_num(htabl_num)
{}

inline Lsh_args::Lsh_args(const string &ipath, string &qpath, string &opath)
    : Prog_args(ipath, qpath, opath, 1, 1.0), hash_functions_num(4), hash_tables_num(5)

{}

inline Lsh_args::Lsh_args(const string &ipath) 
    : Prog_args(ipath)
{}


inline void Lsh_args::set_hash_functions_num(uint16_t hfunc_num)
{
    hash_functions_num = hfunc_num;
}


inline void Lsh_args::set_hash_tables_num(uint16_t htabl_num)
{
    hash_tables_num = htabl_num;
}


inline uint32_t Lsh_args::get_k() const
{
    return hash_functions_num;
}


inline uint16_t Lsh_args::get_hash_tables_num() const
{
    return hash_tables_num;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Cube_args::Cube_args(const string &ipath, string &qpath, string &opath, uint16_t nn_num, float rad, uint32_t proj, uint16_t max_cands, \
                            uint16_t max_probes) 
    : Prog_args(ipath, qpath, opath, nn_num, rad), projection_dim(proj), max_candidates(max_cands), max_probes(max_probes)  
{}


inline Cube_args::Cube_args(const string &ipath)
    : Prog_args(ipath)
{}


inline void Cube_args::set_projection_dim(uint32_t proj)
{
    projection_dim = proj;
}


inline void Cube_args::set_max_candidates(uint16_t max_cands)
{
    max_candidates = max_cands;
}


inline void Cube_args::set_max_probes(uint16_t max_prob)
{
    max_probes = max_prob;
}


inline void Cube_args::write_ann_output(const size_t index, const vector<pair<uint32_t, size_t>> &candidates, const chrono::seconds &dur)
{
    ofstream ofile;
    ofile.open(get_output_file_path(), ios::out | ios::app);
    ofile << "Query: " << index << endl;

    for (size_t i = 0; i != get_nearest_neighbors_num(); ++i) {
        ofile << "Nearest neighbor-" << i + 1 << ": " << candidates[i].second << endl;
        ofile << "distanceHypercube: " << candidates[i].first << endl;
        ofile << "distanceTrue: " << endl;
    }
    ofile << "tHypercube: " << dur.count() << endl;
    ofile << "tTrue: " << endl;

    ofile.close();
}


inline uint32_t Cube_args::get_k() const
{
    return projection_dim;
}


inline uint16_t Cube_args::get_max_candidates() const
{
    return max_candidates;
}


inline uint16_t Cube_args::get_max_probes() const
{
    return max_probes;
}

#endif
