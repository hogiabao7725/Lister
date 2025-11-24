#ifndef SORT_H
#define SORT_H

typedef enum {
    SORT_MODE_ALPHA,
    SORT_MODE_MTIME
} SortMode;

/**
 * @brief Sort directory entry names.
 *
 * @param entries Array of entry strings.
 * @param count Number of entries.
 * @param mode Sorting criteria.
 * @param dir_path Directory path needed for time-based sorting.
 * @param reverse If non-zero, reverse the sort order.
 */
void sort_entries(char **entries, int count, SortMode mode, const char *dir_path, int reverse);

#endif

