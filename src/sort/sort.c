#include "sort.h"
#include "utils/path.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static const char *g_time_sort_dir = NULL;

static int compare_strings(const void *a, const void *b) {
    const char *str_a = *(const char *const *)a;
    const char *str_b = *(const char *const *)b;
    return strcmp(str_a, str_b);
}

static int compare_by_time_internal(const char *str_a, const char *str_b, const char *dir_path) {
    if (dir_path == NULL) {
        return strcmp(str_a, str_b);
    }

    char *path_a = construct_full_path(dir_path, str_a);
    char *path_b = construct_full_path(dir_path, str_b);

    if (path_a == NULL || path_b == NULL) {
        free(path_a);
        free(path_b);
        return strcmp(str_a, str_b);
    }

    struct stat stat_a;
    struct stat stat_b;
    int result = 0;

    if (stat(path_a, &stat_a) == 0 && stat(path_b, &stat_b) == 0) {
        if (stat_a.st_mtime > stat_b.st_mtime) {
            result = -1;
        } else if (stat_a.st_mtime < stat_b.st_mtime) {
            result = 1;
        } else {
            result = strcmp(str_a, str_b);
        }
    } else {
        result = strcmp(str_a, str_b);
    }

    free(path_a);
    free(path_b);
    return result;
}

static int compare_by_time(const void *a, const void *b) {
    const char *str_a = *(const char *const *)a;
    const char *str_b = *(const char *const *)b;
    return compare_by_time_internal(str_a, str_b, g_time_sort_dir);
}

void sort_entries(char **entries, int count, SortMode mode, const char *dir_path) {
    if (entries == NULL || count <= 1) {
        return;
    }

    if (mode == SORT_MODE_ALPHA) {
        qsort(entries, count, sizeof(char *), compare_strings);
        return;
    }

    g_time_sort_dir = dir_path;
    qsort(entries, count, sizeof(char *), compare_by_time);
    g_time_sort_dir = NULL;
}

