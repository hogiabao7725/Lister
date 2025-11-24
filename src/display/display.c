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

void display_long_format(FileInfo *file_infos, int count, int human_readable) {
    if (file_infos == NULL) {
        return;
    }

    // First, collect all file info and calculate maximum field widths
    int max_links = 0;
    int max_owner_len = 0;
    int max_group_len = 0;
    int max_size_str_len = 0;

    // Calculate maximum size string length for human-readable or regular format
    char size_buffer[16];
    for (int i = 0; i < count; i++) {
        // Count digits in link count
        int link_digits = 1;
        int temp_links = file_infos[i].link_count;
        while (temp_links >= 10) {
            link_digits++;
            temp_links /= 10;
        }
        if (link_digits > max_links) {
            max_links = link_digits;
        }

        // Check owner name length
        if (file_infos[i].owner != NULL) {
            int owner_len = (int)strlen(file_infos[i].owner);
            if (owner_len > max_owner_len) {
                max_owner_len = owner_len;
            }
        }

        // Check group name length
        if (file_infos[i].group != NULL) {
            int group_len = (int)strlen(file_infos[i].group);
            if (group_len > max_group_len) {
                max_group_len = group_len;
            }
        }

        // Calculate size string length
        if (human_readable) {
            format_human_readable_size(file_infos[i].size, size_buffer, sizeof(size_buffer));
            int size_str_len = (int)strlen(size_buffer);
            if (size_str_len > max_size_str_len) {
                max_size_str_len = size_str_len;
            }
        } else {
            // Calculate digits in size
            int size_digits = 1;
            long long temp_size = file_infos[i].size;
            while (temp_size >= 10) {
                size_digits++;
                temp_size /= 10;
            }
            if (size_digits > max_size_str_len) {
                max_size_str_len = size_digits;
            }
        }
    }

    // Display each file in long format
    for (int i = 0; i < count; i++) {
        // Permissions
        printf("%s ", file_infos[i].permissions);

        // Link count
        printf("%*d ", max_links, file_infos[i].link_count);

        // Owner (left-aligned)
        if (file_infos[i].owner != NULL) {
            printf("%-*s ", max_owner_len, file_infos[i].owner);
        } else {
            printf("%-*s ", max_owner_len, "");
        }

        // Group (left-aligned)
        if (file_infos[i].group != NULL) {
            printf("%-*s ", max_group_len, file_infos[i].group);
        } else {
            printf("%-*s ", max_group_len, "");
        }

        // Size (right-aligned)
        if (human_readable) {
            format_human_readable_size(file_infos[i].size, size_buffer, sizeof(size_buffer));
            printf("%*s ", max_size_str_len, size_buffer);
        } else {
            printf("%*lld ", max_size_str_len, file_infos[i].size);
        }

        // Date
        if (file_infos[i].date_string != NULL) {
            printf("%s ", file_infos[i].date_string);
        } else {
            printf("            ");
        }

        // File name
        if (file_infos[i].name != NULL) {
            printf("%s", file_infos[i].name);
        }

        printf("\n");
    }
}
