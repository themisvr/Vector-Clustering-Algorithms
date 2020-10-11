#include <iostream>
#include <string>
#include <cstdint>

#include "../../include/io_utils/cmd_args.h"
#include "../../include/io_utils/io_utils.h"
#include "../../include/modules/lsh/lsh.h"


int main (int argc, char *argv[]) {
    
    Lsh_args *args = nullptr;

    /* user executed the program using command line arguments */
    if (argc == 15) {
        lsh_parse_args(argc, argv, &args);
    }
    /* user executed the program with no command line arguments i.e ./lsh */
    else if (argc == 1) {
        user_interface(&args);

    }
    else { usage(argv[0]); }

    /* Store the images */
    std::vector<std::vector<uint8_t>> pixels = read_dataset(args->get_input_file_path());

    /* Compute the mean distance of all vectors */
    double r = vectors_mean_distance<uint8_t> (pixels);

    /* Create lsh (search) structure */
    uint16_t L = args->get_hash_tables_num();
    uint16_t N = args->get_nearest_neighbors_num();
    uint32_t K = args->get_k();
    uint32_t D = pixels[0].size();
    double R = args->get_radius();
    size_t n_vectors = pixels.size();
    size_t ht_size = HT_SIZE(n_vectors);

    LSH<uint8_t> lsh = LSH<uint8_t> (L, N, K, D, R, ht_size, r, n_vectors, pixels);

    /* Start executing query search */

    

    delete args;

    return EXIT_SUCCESS;
}
