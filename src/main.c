#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "directory_reader.h"
#include "file_info.h"
#include "display.h"
#include <sys/stat.h>

/**
 * @brief Comparison function for qsort to sort strings alphabetically
 */
int compare_strings(const void *a, const void *b) {
    const char *str_a = *(const char**)a;
    const char *str_b = *(const char**)b;
    return strcmp(str_a, str_b);
}

// Helper structure and comparator for sorting by modification time
typedef struct {
    const char *name;
    time_t mtime;
} EntryTime;

static int entrytime_cmp(const void *a, const void *b) {
    const EntryTime *ea = (const EntryTime *)a;
    const EntryTime *eb = (const EntryTime *)b;
    if (ea->mtime < eb->mtime) return 1;  // newer first
    if (ea->mtime > eb->mtime) return -1;
    return strcmp(ea->name, eb->name);
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

    // If sorting by time is requested, build an array of mtimes and sort by that.
    char **display_order = NULL; // array of char* pointing into content.entries in the desired order
    if (options.sort_time) {
        EntryTime *et = (EntryTime *)malloc(content.count * sizeof(EntryTime));
        if (et == NULL) {
            // Fallback to alphabetical
            qsort(content.entries, content.count, sizeof(char *), compare_strings);
        } else {
            for (int i = 0; i < content.count; i++) {
                et[i].name = content.entries[i];
                char *full_path = construct_full_path(dir_path, content.entries[i]);
                struct stat st;
                if (full_path != NULL && stat(full_path, &st) == 0) {
                    et[i].mtime = st.st_mtime;
                } else {
                    et[i].mtime = 0;
                }
                if (full_path != NULL) free(full_path);
            }

            qsort(et, content.count, sizeof(EntryTime), entrytime_cmp);

            // Build display_order referencing original name pointers in sorted order
            display_order = (char **)malloc(content.count * sizeof(char *));
            if (display_order != NULL) {
                for (int i = 0; i < content.count; i++) {
                    display_order[i] = (char *)et[i].name;
                }
            } else {
                // Allocation failed; fall back to alphabetical
                qsort(content.entries, content.count, sizeof(char *), compare_strings);
            }

            free(et);
        }
    } else {
        // Sort entries alphabetically
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
            if (display_order != NULL) free(display_order);
            return 1;
        }

        // Get file info for each entry in the chosen display order
        for (int i = 0; i < content.count; i++) {
            const char *name = (display_order != NULL) ? (const char *)display_order[i] : (const char *)content.entries[i];
            char *full_path = construct_full_path(dir_path, name);
            if (full_path != NULL) {
                file_infos[i] = get_file_info(full_path, name);
                free(full_path);
            } else {
                // Initialize empty FileInfo on error
                memset(&file_infos[i], 0, sizeof(FileInfo));
            }
        }

        // Display in long format (pass human_readable option)
        display_long_format(file_infos, content.count, options.human_readable);

        // Free file info structures
        for (int i = 0; i < content.count; i++) {
            free_file_info(file_infos[i]);
        }
        free(file_infos);
        if (display_order != NULL) free(display_order);
    } else {
        // Normal format: just display names
        if (display_order != NULL) {
            display_normal((const char **)display_order, content.count);
            free(display_order);
        } else {
            display_normal((const char **)content.entries, content.count);
        }
    }

    // Clean up directory content
    free_directory_content(content);

    return 0;
}
