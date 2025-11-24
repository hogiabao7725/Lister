#include "sort.h"
#include "utils/path.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Global variables for qsort callback functions
static const char *g_time_sort_dir = NULL;
static int g_reverse_sort = 0;

/**
 * @brief Compare two strings alphabetically
 * 
 * @param a Pointer to first string pointer
 * @param b Pointer to second string pointer
 * @param reverse If non-zero, reverse the comparison result
 * @return int Negative if a < b, positive if a > b, zero if equal
 */
static int compare_strings(const void *a, const void *b, int reverse) {
    const char *str_a = *(const char *const *)a;
    const char *str_b = *(const char *const *)b;
    int result = strcmp(str_a, str_b);
    return reverse ? -result : result;
}

/**
 * @brief Compare two files by modification time
 * 
 * @param str_a First filename
 * @param str_b Second filename
 * @param dir_path Directory path containing the files
 * @param reverse If non-zero, reverse the comparison result
 * @return int Negative if a is newer, positive if b is newer, zero if equal
 */
static int compare_by_time_internal(const char *str_a, const char *str_b, 
                                     const char *dir_path, int reverse) {
    if (dir_path == NULL) {
        int result = strcmp(str_a, str_b);
        return reverse ? -result : result;
    }

    char *path_a = construct_full_path(dir_path, str_a);
    char *path_b = construct_full_path(dir_path, str_b);

    if (path_a == NULL || path_b == NULL) {
        free(path_a);
        free(path_b);
        int result = strcmp(str_a, str_b);
        return reverse ? -result : result;
    }

    struct stat stat_a;
    struct stat stat_b;
    int result = 0;

    if (stat(path_a, &stat_a) == 0 && stat(path_b, &stat_b) == 0) {
        // Compare by modification time (newer files first)
        if (stat_a.st_mtime > stat_b.st_mtime) {
            result = -1;
        } else if (stat_a.st_mtime < stat_b.st_mtime) {
            result = 1;
        } else {
            // If times are equal, compare alphabetically
            result = strcmp(str_a, str_b);
        }
    } else {
        // If stat fails, fall back to alphabetical comparison
        result = strcmp(str_a, str_b);
    }

    free(path_a);
    free(path_b);
    return reverse ? -result : result;
}

/**
 * @brief Wrapper for qsort to compare by time
 */
static int compare_by_time(const void *a, const void *b) {
    const char *str_a = *(const char *const *)a;
    const char *str_b = *(const char *const *)b;
    return compare_by_time_internal(str_a, str_b, g_time_sort_dir, g_reverse_sort);
}

/**
 * @brief Wrapper for qsort to compare alphabetically
 */
static int compare_strings_wrapper(const void *a, const void *b) {
    return compare_strings(a, b, g_reverse_sort);
}

void sort_entries(char **entries, int count, SortMode mode, const char *dir_path, int reverse) {
    if (entries == NULL || count <= 1) {
        return;
    }

    g_reverse_sort = reverse;

    if (mode == SORT_MODE_ALPHA) {
        qsort(entries, count, sizeof(char *), compare_strings_wrapper);
        return;
    }

    // Sort by modification time
    g_time_sort_dir = dir_path;
    qsort(entries, count, sizeof(char *), compare_by_time);
    g_time_sort_dir = NULL;
}
