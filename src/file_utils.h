#ifndef SRC_FILE_UTILS_H_
#define SRC_FILE_UTILS_H_

#include <stdbool.h>

bool file_utils_mkdir_parents(const char *path);
bool file_utils_remove_directory(const char *path);

#endif
