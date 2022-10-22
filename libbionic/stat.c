
int stat_wrapper(const char *pathname, void* statbuf);
int stat(const char *pathname, void* statbuf) {
    return stat_wrapper(pathname, statbuf);
}

int fstat_wrapper(int fd, void* statbuf);
int fstat(int fd, void* statbuf) {
    return fstat_wrapper(fd, statbuf);
}
