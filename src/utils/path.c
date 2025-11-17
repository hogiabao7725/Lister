#include "path.h"
#include <stdlib.h>
#include <string.h>

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

    strcpy(full_path, base_path);

    if (base_len > 0 && base_path[base_len - 1] != '/') {
        full_path[base_len] = '/';
        full_path[base_len + 1] = '\0';
        strcat(full_path, filename);
    } else {
        strcat(full_path, filename);
    }

    return full_path;
}

