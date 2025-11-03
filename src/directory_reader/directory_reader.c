#include "directory_reader.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

DirectoryContent read_directory(const char *path, int show_all) {
    DirectoryContent content;
    content.entries = NULL;
    content.count = 0;

    if (path == NULL) {
        return content;
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        return content;
    }

    // First pass: count entries
    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        // Skip '.' and '..' entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // If show_all is 0, skip hidden files (starting with '.')
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        count++;
    }

    // Allocate memory for entries
    if (count == 0) {
        closedir(dir);
        return content;
    }

    content.entries = (char **)malloc(count * sizeof(char *));
    if (content.entries == NULL) {
        closedir(dir);
        return content;
    }

    // Second pass: collect entry names
    rewinddir(dir);
    int index = 0;
    while ((entry = readdir(dir)) != NULL) {
        // Skip '.' and '..' entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // If show_all is 0, skip hidden files
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        // Allocate memory for entry name and copy it
        size_t name_len = strlen(entry->d_name);
        content.entries[index] = (char *)malloc((name_len + 1) * sizeof(char));
        if (content.entries[index] == NULL) {
            // Clean up on error
            for (int i = 0; i < index; i++) {
                free(content.entries[i]);
            }
            free(content.entries);
            closedir(dir);
            content.entries = NULL;
            content.count = 0;
            return content;
        }
        strcpy(content.entries[index], entry->d_name);
        index++;
    }

    closedir(dir);
    content.count = count;
    return content;
}

void free_directory_content(DirectoryContent content) {
    if (content.entries != NULL) {
        for (int i = 0; i < content.count; i++) {
            if (content.entries[i] != NULL) {
                free(content.entries[i]);
            }
        }
        free(content.entries);
    }
}
