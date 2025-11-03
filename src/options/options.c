#include "options.h"
#include <string.h>

void init_options(Options *options) {
    if (options == NULL) {
        return;
    }
    options->show_all = 0;
    options->long_format = 0;
    options->human_readable = 0;
    options->sort_time = 0;
}

int parse_options(int argc, char *argv[], Options *options) {
    if (options == NULL || argv == NULL) {
        return 0;
    }

    // Initialize with default values
    init_options(options);

    int non_option_count = 0;

    // Parse each argument
    for (int i = 1; i < argc; i++) {
        // Check if argument starts with '-' (option flag)
        if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
            // Process each character in the option string
            for (size_t j = 1; j < strlen(argv[i]); j++) {
                switch (argv[i][j]) {
                    case 'a':
                        options->show_all = 1;
                        break;
                    case 'l':
                        options->long_format = 1;
                        break;
                    case 'h':
                        options->human_readable = 1;
                        break;
                    case 't':
                        options->sort_time = 1;
                        break;
                    default:
                        // Ignore unknown options for now
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
