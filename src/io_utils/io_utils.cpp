#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "cmd_args.h"

using namespace std;

void parse_args(int argc, char * const argv[], Prog_args **args)
{
    int opt, hfunc_num, htabl_num, nn_num;
    string dataset_file, query_file, output_file;
    float rad;

    while( (opt = getopt(argc, argv, "d:q:k:L:o:N:R")) != -1 )
    {
        switch(opt) 
        {
            case 'd':
                        dataset_file = optarg;
                        break;
                        
            case 'q':
                        query_file = optarg; 
                        break;

            case 'k':
                        hfunc_num = atoi(optarg);
                        break;

            case 'L':
                        htabl_num = atoi(optarg);
                        break;

            case 'o':
                        output_file = optarg;
                        break;

            case 'N':
                        nn_num = atoi(optarg);
                        break;

            case 'R':
                        optarg == NULL ? rad = 0.0 : rad = atof(optarg);
                        break;

            default: 
                        // one or more of the "-x" options did not appear
                        cout << "not k not L" << endl;
                        break;

        }
    }
    *args = new Prog_args(dataset_file, query_file, output_file, hfunc_num, htabl_num, nn_num, rad); // allocate a pointer to a Prog_args object
}
