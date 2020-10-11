#include <vector>
#include "cmd_args.h"

vector<vector<uint8_t>> read_dataset(const string&);

string user_prompt_file(const string&);

uint16_t user_prompt_search_arg(const string&);

float user_prompt_rad(const string&);

uint32_t bigend_to_littlend(uint32_t);

inline bool file_exists(const char *);

void lsh_parse_args(int, char * const [], Lsh_args **);

void user_interface(Lsh_args **);

void usage(const char *);
