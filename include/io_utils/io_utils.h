#include <vector>
#include "cmd_args.h"

vector<vector<unsigned char>> read_dataset(const string &);
void parse_args_lsh(int, char * const [], Prog_args **);
uint32_t bigend_to_littlend(uint32_t);
inline bool file_exists(const char *);
