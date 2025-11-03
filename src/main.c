#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "options.h"
#include "directory_reader.h"
#include "file_info.h"
#include "display.h"

// Global variable to store directory path for comparison functions
static const char *g_dir_path = NULL;

// Forward declaration
char *construct_full_path(const char *base_path, const char *filename);

/**
 * @brief Comparison function for qsort to sort strings alphabetically
 */
int compare_strings(const void *a, const void *b) {
    const char *str_a = *(const char**)a;
    const char *str_b = *(const char**)b;
    return strcmp(str_a, str_b);
}

/**
 * @brief Comparison function for qsort to sort by modification time (newest first)
 */
int compare_by_time(const void *a, const void *b) {
    const char *str_a = *(const char**)a;
    const char *str_b = *(const char**)b;
    
    if (g_dir_path == NULL) {
        return strcmp(str_a, str_b);
    }

    // Construct full paths
    char *path_a = construct_full_path(g_dir_path, str_a);
    char *path_b = construct_full_path(g_dir_path, str_b);
    
    if (path_a == NULL || path_b == NULL) {
        if (path_a != NULL) free(path_a);
        if (path_b != NULL) free(path_b);
        return strcmp(str_a, str_b);
    }

    // Get file stats
    struct stat stat_a, stat_b;
    int result = 0;
    
    if (stat(path_a, &stat_a) == 0 && stat(path_b, &stat_b) == 0) {
        // Compare modification times (newer files first)
        if (stat_a.st_mtime > stat_b.st_mtime) {
            result = -1;  // a is newer, so it comes first
        } else if (stat_a.st_mtime < stat_b.st_mtime) {
            result = 1;   // b is newer, so it comes first
        } else {
            // Same time, sort alphabetically
            result = strcmp(str_a, str_b);
        }
    } else {
        // If stat fails, fall back to alphabetical
        result = strcmp(str_a, str_b);
    }

    free(path_a);
    free(path_b);
    return result;
}

/**
 * @brief Construct full path from base directory and filename
 * 
 * @param base_path Base directory path
 * @param filename Filename
 * @return char* Allocated string containing full path (must be freed by caller)
 */
char *construct_full_path(const char *base_path, const char *filename) {
    if (base_path == NULL || filename == NULL) {
        return NULL;
    }

    size_t base_len = strlen(base_path);
    size_t file_len = strlen(filename);
    size_t total_len = base_len + file_len + 2; // +2 for '/' and '\0'

    char *full_path = (char *)malloc(total_len * sizeof(char));
    if (full_path == NULL) {
        return NULL;
    }

    // Copy base path
    strcpy(full_path, base_path);
    
    // Add '/' if base path doesn't end with it
    if (base_path[base_len - 1] != '/') {
        full_path[base_len] = '/';
        full_path[base_len + 1] = '\0';
        strcat(full_path, filename);
    } else {
        strcat(full_path, filename);
    }

    return full_path;
}

int main(int argc, char *argv[]) {
    // Parse command-line options
    Options options;
    int non_option_count = parse_options(argc, argv, &options);

    // Determine directory path (default to current directory)
    const char *dir_path = ".";
    
    // If directory path is provided, use it (for now, just use current directory)
    // Future: support multiple directories
    if (non_option_count > 0) {
        // Find first non-option argument
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] != '-') {
                dir_path = argv[i];
                break;
            }
        }
    }

    // Read directory contents
    DirectoryContent content = read_directory(dir_path, options.show_all);

    if (content.entries == NULL) {
        fprintf(stderr, "Error: Cannot read directory '%s'\n", dir_path);
        return 1;
    }

    // Sort entries based on option
    if (options.sort_by_time) {
        // Set global directory path for comparison function
        g_dir_path = dir_path;
        qsort(content.entries,
              content.count,
              sizeof(char*),
              compare_by_time);
        g_dir_path = NULL;
    } else {
        // Sort alphabetically by default
        qsort(content.entries,
              content.count,
              sizeof(char*),
              compare_strings);
    }

    // Display based on format option
    if (options.long_format) {
        // Long format: collect file information for each entry
        FileInfo *file_infos = (FileInfo *)malloc(content.count * sizeof(FileInfo));
        if (file_infos == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            free_directory_content(content);
            return 1;
        }

        // Get file info for each entry
        for (int i = 0; i < content.count; i++) {
            char *full_path = construct_full_path(dir_path, content.entries[i]);
            if (full_path != NULL) {
                file_infos[i] = get_file_info(full_path, content.entries[i]);
                free(full_path);
            } else {
                // Initialize empty FileInfo on error
                memset(&file_infos[i], 0, sizeof(FileInfo));
            }
        }

        // Display in long format
        display_long_format(file_infos, content.count, options.human_readable);

        // Free file info structures
        for (int i = 0; i < content.count; i++) {
            free_file_info(file_infos[i]);
        }
        free(file_infos);
    } else {
        // Normal format: just display names
        display_normal((const char **)content.entries, content.count);
    }

    // Clean up directory content
    free_directory_content(content);

    return 0;
}
