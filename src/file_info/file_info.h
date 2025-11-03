#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

/**
 * @brief Structure to hold detailed file information
 */
typedef struct {
    char *name;              // File name
    char permissions[11];    // File permissions string (e.g., "-rwxr-xr-x")
    char type_char;          // File type character ('d' for directory, '-' for regular file, etc.)
    long long size;          // File size in bytes
    char *owner;             // Owner name
    char *group;             // Group name
    char *date_string;       // Last modification date string
    int link_count;          // Number of hard links
    struct stat stat_info;   // Complete stat structure
} FileInfo;

/**
 * @brief Get detailed information about a file
 * 
 * @param filepath Full path to the file
 * @param filename Name of the file (for display)
 * @return FileInfo Structure containing file information, or NULL fields on error
 */
FileInfo get_file_info(const char *filepath, const char *filename);

/**
 * @brief Free memory allocated in FileInfo structure
 * 
 * @param info FileInfo structure to free
 */
void free_file_info(FileInfo info);

/**
 * @brief Convert file mode to permission string
 * 
 * @param mode File mode from stat structure
 * @param perm_string Buffer to store permission string (must be at least 11 chars)
 */
void mode_to_permissions(mode_t mode, char *perm_string);

#endif
