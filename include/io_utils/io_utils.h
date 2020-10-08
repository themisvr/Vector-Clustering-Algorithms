#include <vector>
#include "cmd_args.h"

vector<vector<uint8_t>> read_dataset(const string&);
string   user_prompt_file(const string&);
uint16_t user_prompt_search_arg(const string&);
float    user_prompt_rad(const string&);
void parse_args_lsh(int, char * const [], Lsh_args **);
uint32_t bigend_to_littlend(uint32_t);
inline bool file_exists(const char *);
