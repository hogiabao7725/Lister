#ifndef OPTIONS_H
#define OPTIONS_H

/**
 * @brief Structure to hold parsed command-line options
 */
typedef struct {
    int show_all;          // -a flag: show all files including hidden ones
    int long_format;       // -l flag: display in long format
    int sort_by_time;     // -t flag: sort by modification time
    int show_size;         // -s flag: display file size in blocks
    int human_readable;   // -h flag: display file sizes in human-readable format
    int reverse_sort;      // -r flag: reverse the sort order
} Options;

/**
 * @brief Parse command-line arguments and extract options
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @param options Pointer to Options structure to fill
 * @return int Number of non-option arguments (directory paths)
 */
int parse_options(int argc, char *argv[], Options *options);

/**
 * @brief Initialize options structure with default values
 * 
 * @param options Pointer to Options structure to initialize
 */
void init_options(Options *options);

#endif
