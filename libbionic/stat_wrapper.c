
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stat_wrapper(const char *pathname, void* statbuf) {
    return stat(pathname, (struct stat*)statbuf);
}

int fstat_wrapper(int fd, void* statbuf) {
    return fstat(fd, (struct stat*)statbuf);
}
