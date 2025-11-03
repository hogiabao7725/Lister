#ifndef DIRECTORY_READER_H
#define DIRECTORY_READER_H

/**
 * @brief Structure to hold directory contents
 */
typedef struct {
    char **entries;  // Array of entry names
    int count;       // Number of entries
} DirectoryContent;

/**
 * @brief Read directory contents
 * 
 * @param path Path to the directory to read
 * @param show_all If 1, include hidden files (starting with '.'); if 0, exclude them
 * @return DirectoryContent Structure containing array of entry names and count
 */
DirectoryContent read_directory(const char *path, int show_all);

/**
 * @brief Free memory allocated for DirectoryContent structure
 * 
 * @param content DirectoryContent structure to free
 */
void free_directory_content(DirectoryContent content);

#endif
