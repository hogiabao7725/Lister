#ifndef PATH_H
#define PATH_H

/**
 * @brief Construct full path from base directory and filename
 *
 * @param base_path Base directory path
 * @param filename Filename
 * @return char* Newly allocated string containing full path (caller frees)
 */
char *construct_full_path(const char *base_path, const char *filename);

#endif

