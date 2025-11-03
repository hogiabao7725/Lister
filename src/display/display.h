#ifndef DISPLAY_H
#define DISPLAY_H

#include "file_info.h"

/**
 * @brief Display files in normal format (just names)
 * 
 * @param entries Array of file names
 * @param count Number of files
 */
void display_normal(const char **entries, int count);

/**
 * @brief Display files in long format (detailed information)
 * 
 * @param file_infos Array of FileInfo structures
 * @param count Number of files
 * @param human_readable If non-zero, display file sizes in human-readable format
 */
void display_long_format(FileInfo *file_infos, int count, int human_readable);

#endif
