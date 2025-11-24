#include "options.h"
#include <string.h>

void init_options(Options *options) {
    if (options == NULL) {
        return;
    }
    options->show_all = 0;
    options->long_format = 0;
    options->sort_by_time = 0;
    options->show_size = 0;
    options->list_directories = 0;
    options->human_readable = 0;
    options->reverse_sort = 0;
}

int parse_options(int argc, char *argv[], Options *options) {
    if (options == NULL || argv == NULL) {
        return 0;
    }

    // Initialize with default values
    init_options(options);

    int non_option_count = 0;

    // Parse each command-line argument
    for (int i = 1; i < argc; i++) {
        // Check for --help (long option)
        if (strcmp(argv[i], "--help") == 0) {
            // Return special value to indicate help was requested
            // We'll handle this in main() by checking if return value is -1
            return -1;
        }

        // Check if argument is an option flag (starts with '-')
        if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
            // Process each character in the option string (e.g., "-al" processes 'a' and 'l')
            for (size_t j = 1; j < strlen(argv[i]); j++) {
                switch (argv[i][j]) {
                    case 'a':
                        options->show_all = 1;
                        break;
                    case 'l':
                        options->long_format = 1;
                        break;
                    case 't':
                        options->sort_by_time = 1;
                        break;
                    case 's':
                        options->show_size = 1;
                        break;
                    case 'd':
                        options->list_directories = 1;
                        break;
                    case 'h':
                        options->human_readable = 1;
                        break;
                    case 'r':
                        options->reverse_sort = 1;
                        break;
                    default:
                        // Ignore unknown options
                        break;
                }
            }
        } else {
            // Non-option argument (directory path)
            non_option_count++;
        }
    }

    return non_option_count;
}
