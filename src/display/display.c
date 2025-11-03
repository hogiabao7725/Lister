#include "display.h"
#include "file_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>


/**
 * @brief Get terminal width in columns
 * 
 * @return int Terminal width, or 80 if unable to determine
 */
static int get_terminal_width(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
    
    // Fallback: check environment variable
    const char *cols = getenv("COLUMNS");
    if (cols != NULL) {
        int width = atoi(cols);
        if (width > 0) {
            return width;
        }
    }
    
    // Default width
    return 80;
}

void display_normal(const char **entries, int count) {
    if (entries == NULL || count == 0) {
        return;
    }

    // Find maximum filename length
    int max_name_len = 0;
    for (int i = 0; i < count; i++) {
        if (entries[i] != NULL) {
            int len = (int)strlen(entries[i]);
            if (len > max_name_len) {
                max_name_len = len;
            }
        }
    }

    // Get terminal width
    int terminal_width = get_terminal_width();
    
    // Calculate column width: max filename length + 2 spaces for padding
    int column_width = max_name_len + 2;
    
    // Calculate number of columns
    int num_columns = terminal_width / column_width;
    if (num_columns < 1) {
        num_columns = 1;
    }
    
    // Calculate number of rows needed
    int num_rows = (count + num_columns - 1) / num_columns;  // Ceiling division
    
    // Display in column-major order (like ls command)
    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_columns; col++) {
            int index = col * num_rows + row;
            
            if (index < count && entries[index] != NULL) {
                // Left-align the filename in its column
                printf("%-*s", column_width, entries[index]);
            }
        }
        printf("\n");
    }
}

// Helper: format size into human-readable string (e.g., 1K, 2.5M)
static void format_human_readable(long long size, char *buf, size_t bufsz) {
    const char *units[] = {"B", "K", "M", "G", "T", "P"};
    double s = (double)size;
    int unit = 0;
    while (s >= 1024.0 && unit < 5) {
        s /= 1024.0;
        unit++;
    }

    if (unit == 0) {
        // Bytes: show as integer with B suffix
        snprintf(buf, bufsz, "%lldB", size);
    } else {
        // For larger units: show one decimal if <10, otherwise no decimals
        if (s < 10.0) {
            snprintf(buf, bufsz, "%.1f%s", s, units[unit]);
        } else {
            snprintf(buf, bufsz, "%.0f%s", s, units[unit]);
        }
    }
}

void display_long_format(FileInfo *file_infos, int count, int human_readable) {
    if (file_infos == NULL) {
        return;
    }

    // First, collect all file info and calculate maximum field widths
    int max_links = 0;
    int max_owner_len = 0;
    int max_group_len = 0;
    long long max_size = 0;

    for (int i = 0; i < count; i++) {
        int link_digits = 1;
        int temp_links = file_infos[i].link_count;
        while (temp_links >= 10) {
            link_digits++;
            temp_links /= 10;
        }
        if (link_digits > max_links) max_links = link_digits;

        if (file_infos[i].owner != NULL) {
            int owner_len = (int)strlen(file_infos[i].owner);
            if (owner_len > max_owner_len) max_owner_len = owner_len;
        }

        if (file_infos[i].group != NULL) {
            int group_len = (int)strlen(file_infos[i].group);
            if (group_len > max_group_len) max_group_len = group_len;
        }

        if (file_infos[i].size > max_size) max_size = file_infos[i].size;
    }

    // If human_readable, precompute size strings
    char **size_strs = NULL;
    int size_width = 1;
    if (human_readable) {
        size_strs = (char **)malloc(sizeof(char *) * count);
        if (size_strs == NULL) {
            human_readable = 0;
        } else {
            int max_w = 0;
            for (int i = 0; i < count; i++) {
                char buf[64];
                format_human_readable(file_infos[i].size, buf, sizeof(buf));

                size_strs[i] = malloc(strlen(buf) + 1);
                if (size_strs[i] != NULL) {
                    strcpy(size_strs[i], buf);
                } else {
                    size_strs[i] = malloc(2);
                    if (size_strs[i] != NULL) strcpy(size_strs[i], "?");
                }

                int l = (size_strs[i] != NULL) ? (int)strlen(size_strs[i]) : 1;
                if (l > max_w) max_w = l;
            }
            size_width = max_w;
        }
    }

    if (!human_readable) {
        long long temp_size = max_size;
        while (temp_size >= 10) {
            size_width++;
            temp_size /= 10;
        }
    }

    // Display each file in long format
    for (int i = 0; i < count; i++) {
        printf("%s ", file_infos[i].permissions);
        printf("%*d ", max_links, file_infos[i].link_count);
        printf("%-*s ", max_owner_len, file_infos[i].owner ? file_infos[i].owner : "");
        printf("%-*s ", max_group_len, file_infos[i].group ? file_infos[i].group : "");

        if (human_readable && size_strs != NULL) {
            printf("%*s ", size_width, size_strs[i]);
        } else {
            printf("%*lld ", size_width, file_infos[i].size);
        }

        printf("%s ", file_infos[i].date_string ? file_infos[i].date_string : "            ");
        printf("%s\n", file_infos[i].name ? file_infos[i].name : "");
    }

    if (size_strs != NULL) {
        for (int i = 0; i < count; i++) {
            if (size_strs[i] != NULL) free(size_strs[i]);
        }
        free(size_strs);
    }
}

