
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>

struct bionic_dirent {
  uint64_t         d_ino;
  int64_t          d_off;
  unsigned short   d_reclen;
  unsigned char    d_type;
  char             d_name[256];
};

typedef struct {
    DIR* dirp;
    struct bionic_dirent current;
} bionic_DIR;

static DIR* bionic_get_dirp(bionic_DIR* bionic_dirp) {
    return bionic_dirp->dirp;
}

static DIR* bionic_set_dirp(bionic_DIR* bionic_dirp, DIR* dirp) {
    bionic_dirp->dirp = dirp;
    return dirp;
}

static bionic_DIR* bionic_wrap_dirp(DIR* dirp) {
    if (dirp == NULL) {
        return NULL;
    }
    bionic_DIR* retval = malloc(sizeof(bionic_DIR));
    memset(retval, 0, sizeof(*retval));
    bionic_set_dirp(retval, dirp);
    return retval;
}

bionic_DIR* bionic_opendir(const char* name) {
    return bionic_wrap_dirp(opendir(name));
}

bionic_DIR* bionic_fdopendir(int fd) {
    return bionic_wrap_dirp(fdopendir(fd));
}

int bionic_closedir(bionic_DIR* d) {
    int ret = closedir(bionic_get_dirp(d));
    free(d);
    return ret;
}

void bionic_rewinddir(bionic_DIR* d) {
    rewinddir(bionic_get_dirp(d));
}

int bionic_dirfd(bionic_DIR* d) {
    return dirfd(bionic_get_dirp(d));
}

struct bionic_dirent* bionic_readdir(bionic_DIR* d) {
    struct dirent* ent = readdir(bionic_get_dirp(d));
    if (ent == NULL) {
        return NULL;
    }
    d->current.d_ino = ent->d_ino;
    d->current.d_off = ent->d_off;
    d->current.d_reclen = ent->d_reclen;
    d->current.d_type = ent->d_type;
    memcpy(d->current.d_name, ent->d_name, sizeof(d->current.d_name));
    d->current.d_name[sizeof(d->current.d_name) - 1] = 0;
    return &d->current;
}

