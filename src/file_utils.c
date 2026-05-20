#include "file_utils.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool file_utils_mkdir_parents(const char *path) {
    char  tmp[1024];
    char *end = tmp + sizeof(tmp) - 1;

    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (p < end && mkdir(tmp, 0755) != 0 && errno != EEXIST)
                return false;
            *p = '/';
        }
    }
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
        return false;
    return true;
}

bool file_utils_remove_directory(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL)
        return false;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char subpath[1280];
        snprintf(subpath, sizeof(subpath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(subpath, &st) != 0)
            continue;

        if (S_ISDIR(st.st_mode)) {
            file_utils_remove_directory(subpath);
        } else {
            unlink(subpath);
        }
    }
    closedir(dir);
    rmdir(path);
    return true;
}
