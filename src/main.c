#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "directory_reader.h"
#include "display.h"
#include "file_info.h"
#include "options.h"
#include "sort/sort.h"
#include "utils/path.h"

static const char *resolve_directory_path(int argc, char *argv[], int non_option_count);
static FileInfo *collect_file_infos(const char *dir_path, DirectoryContent content);
static void free_file_info_list(FileInfo *file_infos, int count);
static int render_listing(const DirectoryContent *content, const Options *options,
                          const char *dir_path);

int main(int argc, char *argv[]) {
    Options options;
    int non_option_count = parse_options(argc, argv, &options);

    const char *dir_path = resolve_directory_path(argc, argv, non_option_count);

    // Handle -d option: list directories themselves, not their contents
    if (options.list_directories) {
        struct stat path_stat;
        if (stat(dir_path, &path_stat) != 0) {
            fprintf(stderr, "Error: Cannot access '%s'\n", dir_path);
            return 1;
        }

        // Create a DirectoryContent with just the directory name
        DirectoryContent content;
        content.count = 1;
        content.entries = (char **)malloc(sizeof(char *));
        if (content.entries == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            return 1;
        }

        // Extract directory name from path
        const char *name = dir_path;
        const char *last_slash = strrchr(dir_path, '/');
        if (last_slash != NULL && last_slash[1] != '\0') {
            name = last_slash + 1;
        } else if (last_slash != NULL && last_slash == dir_path) {
            name = "/";
        } else if (strcmp(dir_path, ".") == 0) {
            name = ".";
        }

        size_t name_len = strlen(name);
        content.entries[0] = (char *)malloc((name_len + 1) * sizeof(char));
        if (content.entries[0] == NULL) {
            free(content.entries);
            fprintf(stderr, "Error: Memory allocation failed\n");
            return 1;
        }
        strcpy(content.entries[0], name);

        // Display the directory itself
        if (render_listing(&content, &options, dir_path) != 0) {
            free_directory_content(content);
            return 1;
        }

        free_directory_content(content);
        return 0;
    }

    DirectoryContent content = read_directory(dir_path, options.show_all);
    if (content.entries == NULL) {
        fprintf(stderr, "Error: Cannot read directory '%s'\n", dir_path);
        return 1;
    }

    // Determine sort mode and sort entries
    SortMode sort_mode = options.sort_by_time ? SORT_MODE_MTIME : SORT_MODE_ALPHA;
    sort_entries(content.entries, content.count, sort_mode, dir_path, options.reverse_sort);

    // Display the listing
    if (render_listing(&content, &options, dir_path) != 0) {
        free_directory_content(content);
        return 1;
    }

    free_directory_content(content);
    return 0;
}

/**
 * @brief Resolve the directory path from command-line arguments
 * 
 * @param argc Number of arguments
 * @param argv Argument array
 * @param non_option_count Number of non-option arguments
 * @return const char* Directory path (defaults to current directory ".")
 */
static const char *resolve_directory_path(int argc, char *argv[], int non_option_count) {
    const char *dir_path = ".";

    if (non_option_count <= 0) {
        return dir_path;
    }

    // Find the first non-option argument (directory path)
    for (int i = 1; i < argc; i++) {
        if (argv[i] != NULL && argv[i][0] != '-') {
            dir_path = argv[i];
            break;
        }
    }

    return dir_path;
}

/**
 * @brief Collect file information for all entries in a directory
 * 
 * @param dir_path Directory path
 * @param content Directory content structure
 * @return FileInfo* Array of file information (caller must free)
 */
static FileInfo *collect_file_infos(const char *dir_path, DirectoryContent content) {
    if (content.count == 0) {
        return NULL;
    }

    FileInfo *file_infos = (FileInfo *)malloc(content.count * sizeof(FileInfo));
    if (file_infos == NULL) {
        return NULL;
    }

    // Collect information for each file
    for (int i = 0; i < content.count; i++) {
        memset(&file_infos[i], 0, sizeof(FileInfo));

        char *full_path = construct_full_path(dir_path, content.entries[i]);
        if (full_path != NULL) {
            file_infos[i] = get_file_info(full_path, content.entries[i]);
            free(full_path);
        }
    }

    return file_infos;
}

/**
 * @brief Free an array of FileInfo structures
 * 
 * @param file_infos Array of FileInfo structures
 * @param count Number of entries
 */
static void free_file_info_list(FileInfo *file_infos, int count) {
    if (file_infos == NULL) {
        return;
    }

    for (int i = 0; i < count; i++) {
        free_file_info(file_infos[i]);
    }
    free(file_infos);
}

/**
 * @brief Render the directory listing based on options
 * 
 * @param content Directory content
 * @param options Display options
 * @param dir_path Directory path
 * @return int 0 on success, 1 on error
 */
static int render_listing(const DirectoryContent *content, const Options *options,
                          const char *dir_path) {
    if (content == NULL || options == NULL) {
        return 1;
    }

    // Display in normal format (just filenames)
    if (!options->long_format) {
        display_normal((const char **)content->entries, content->count, 
                       options->show_size, dir_path);
        return 0;
    }

    // Display in long format (detailed information)
    FileInfo *file_infos = collect_file_infos(dir_path, *content);
    if (content->count > 0 && file_infos == NULL) {
        fprintf(stderr, "Error: Unable to gather file information\n");
        return 1;
    }

    display_long_format(file_infos, content->count, options->human_readable);
    free_file_info_list(file_infos, content->count);
    return 0;
}
