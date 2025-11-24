#define _POSIX_C_SOURCE 200809L
#include "file_info.h"
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

void mode_to_permissions(mode_t mode, char *perm_string) {
    if (perm_string == NULL) {
        return;
    }

    // Determine file type character
    if (S_ISDIR(mode)) {
        perm_string[0] = 'd';  // Directory
    } else if (S_ISLNK(mode)) {
        perm_string[0] = 'l';  // Symbolic link
    } else if (S_ISCHR(mode)) {
        perm_string[0] = 'c';  // Character device
    } else if (S_ISBLK(mode)) {
        perm_string[0] = 'b';  // Block device
    } else if (S_ISFIFO(mode)) {
        perm_string[0] = 'p';  // FIFO/pipe
    } else if (S_ISSOCK(mode)) {
        perm_string[0] = 's';  // Socket
    } else {
        perm_string[0] = '-';  // Regular file
    }

    // Owner (user) permissions: read, write, execute
    perm_string[1] = (mode & S_IRUSR) ? 'r' : '-';
    perm_string[2] = (mode & S_IWUSR) ? 'w' : '-';
    perm_string[3] = (mode & S_IXUSR) ? 'x' : '-';

    // Group permissions: read, write, execute
    perm_string[4] = (mode & S_IRGRP) ? 'r' : '-';
    perm_string[5] = (mode & S_IWGRP) ? 'w' : '-';
    perm_string[6] = (mode & S_IXGRP) ? 'x' : '-';

    // Other (world) permissions: read, write, execute
    perm_string[7] = (mode & S_IROTH) ? 'r' : '-';
    perm_string[8] = (mode & S_IWOTH) ? 'w' : '-';
    perm_string[9] = (mode & S_IXOTH) ? 'x' : '-';

    perm_string[10] = '\0';
}

FileInfo get_file_info(const char *filepath, const char *filename) {
    FileInfo info;

    // Initialize all fields to safe defaults
    info.name = NULL;
    info.permissions[0] = '\0';
    info.type_char = '-';
    info.size = 0;
    info.owner = NULL;
    info.group = NULL;
    info.date_string = NULL;
    info.link_count = 0;
    memset(&info.stat_info, 0, sizeof(struct stat));

    if (filepath == NULL || filename == NULL) {
        return info;
    }

    // Get file statistics
    if (stat(filepath, &info.stat_info) != 0) {
        return info;
    }

    // Copy filename
    size_t name_len = strlen(filename);
    info.name = (char *)malloc((name_len + 1) * sizeof(char));
    if (info.name == NULL) {
        return info;
    }
    strcpy(info.name, filename);

    // Get permissions string and file type
    mode_to_permissions(info.stat_info.st_mode, info.permissions);
    info.type_char = info.permissions[0];

    // Get file size
    info.size = (long long)info.stat_info.st_size;

    // Get hard link count
    info.link_count = (int)info.stat_info.st_nlink;

    // Get owner name from UID
    struct passwd *pw = getpwuid(info.stat_info.st_uid);
    if (pw != NULL) {
        size_t owner_len = strlen(pw->pw_name);
        info.owner = (char *)malloc((owner_len + 1) * sizeof(char));
        if (info.owner != NULL) {
            strcpy(info.owner, pw->pw_name);
        }
    } else {
        // Fallback: convert UID to string if getpwuid fails
        char uid_str[32];
        snprintf(uid_str, sizeof(uid_str), "%u", info.stat_info.st_uid);
        size_t uid_str_len = strlen(uid_str);
        info.owner = (char *)malloc((uid_str_len + 1) * sizeof(char));
        if (info.owner != NULL) {
            strcpy(info.owner, uid_str);
        }
    }

    // Get group name from GID
    struct group *gr = getgrgid(info.stat_info.st_gid);
    if (gr != NULL) {
        size_t group_len = strlen(gr->gr_name);
        info.group = (char *)malloc((group_len + 1) * sizeof(char));
        if (info.group != NULL) {
            strcpy(info.group, gr->gr_name);
        }
    } else {
        // Fallback: convert GID to string if getgrgid fails
        char gid_str[32];
        snprintf(gid_str, sizeof(gid_str), "%u", info.stat_info.st_gid);
        size_t gid_str_len = strlen(gid_str);
        info.group = (char *)malloc((gid_str_len + 1) * sizeof(char));
        if (info.group != NULL) {
            strcpy(info.group, gid_str);
        }
    }

    // Format modification date as "MMM DD HH:MM"
    struct tm *time_info = localtime(&info.stat_info.st_mtime);
    if (time_info != NULL) {
        info.date_string = (char *)malloc(13 * sizeof(char));
        if (info.date_string != NULL) {
            const char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
            snprintf(info.date_string, 13, "%s %2d %02d:%02d",
                    months[time_info->tm_mon],
                    time_info->tm_mday,
                    time_info->tm_hour,
                    time_info->tm_min);
        }
    }

    return info;
}

void free_file_info(FileInfo info) {
    if (info.name != NULL) {
        free(info.name);
    }
    if (info.owner != NULL) {
        free(info.owner);
    }
    if (info.group != NULL) {
        free(info.group);
    }
    if (info.date_string != NULL) {
        free(info.date_string);
    }
}

void format_human_readable_size(long long size, char *buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        return;
    }

    const char *units[] = {"B", "K", "M", "G", "T", "P"};
    int unit_index = 0;
    double size_double = (double)size;

    // Convert to appropriate unit (B, K, M, G, T, P)
    while (size_double >= 1024.0 && unit_index < 5) {
        size_double /= 1024.0;
        unit_index++;
    }

    // Format the string based on unit
    if (unit_index == 0) {
        // Bytes: show as integer
        snprintf(buffer, buffer_size, "%lld%s", (long long)size_double, units[unit_index]);
    } else {
        // KB and above: show with 1 decimal place if less than 10, otherwise integer
        if (size_double < 10.0) {
            snprintf(buffer, buffer_size, "%.1f%s", size_double, units[unit_index]);
        } else {
            snprintf(buffer, buffer_size, "%.0f%s", size_double, units[unit_index]);
        }
    }
}
