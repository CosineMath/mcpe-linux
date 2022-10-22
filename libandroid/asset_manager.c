#define _GNU_SOURCE

#include <android/asset_manager.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

struct AAssetManager {
    int fd;
};

struct AAsset {
    int fd;
    void* buffer;
};

struct AAssetDir {
    DIR* dirp;
    char path_name[PATH_MAX];
    char name_buf[PATH_MAX];
};

// Implementation
AAssetManager* AAssetManager_create(int fd) {
    AAssetManager* mgr = malloc(sizeof(AAssetManager));
    mgr->fd = fd;
    return mgr;
}

AAssetDir* AAssetManager_openDir(AAssetManager* mgr, const char* dirName) {
    int fd = openat(mgr->fd, dirName, O_RDONLY|O_DIRECTORY);
    if (fd == -1) {
        // fprintf(stderr, "AAssetManager_openDir(%s) failed: %s\n", dirName, strerror(errno));
        return NULL;
    }
    AAssetDir* assetDir = malloc(sizeof(AAssetDir));
    assetDir->dirp = fdopendir(fd);
    strncpy(assetDir->path_name, dirName, PATH_MAX - 1);
    return assetDir;
}

const char* AAssetDir_getNextFileName(AAssetDir* assetDir) {
    if (assetDir == NULL) {
        return NULL;
    }
    struct dirent* ent = readdir(assetDir->dirp);
    if (ent == NULL) {
        return NULL;
    }
    strncpy(assetDir->name_buf, assetDir->path_name, PATH_MAX - 1);
    strncat(assetDir->name_buf, ent->d_name, PATH_MAX - 1);
    return assetDir->name_buf;
}

void AAssetDir_close(AAssetDir* assetDir) {
    if (assetDir == NULL) {
        return;
    }
    closedir(assetDir->dirp);
    free(assetDir);
}

AAsset* AAssetManager_open(AAssetManager* mgr, const char* filename, int mode) {
    int fd = openat(mgr->fd, filename, O_RDONLY);
    if (fd == -1) {
        // fprintf(stderr, "AAssetManager_open(%s) failed: %s\n", filename, strerror(errno));
        return NULL;
    }
    AAsset* asset = malloc(sizeof(AAsset));
    asset->fd = fd;
    asset->buffer = NULL;
    return asset;
}

int AAsset_read(AAsset* asset, void* buf, size_t count) {
    return read(asset->fd, buf, count);
}

off64_t AAsset_seek64(AAsset* asset, off64_t offset, int whence) {
    return lseek64(asset->fd, offset, whence);
}

void AAsset_close(AAsset* asset) {
    close(asset->fd);
    if (asset->buffer != NULL) {
        free(asset->buffer);
    }
    free(asset);
}

const void* AAsset_getBuffer(AAsset* asset) {
    if (asset->buffer != NULL) {
        free(asset->buffer);
    }
    size_t count = AAsset_getLength64(asset);
    asset->buffer = malloc(count);
    ssize_t ret = pread64(asset->fd, asset->buffer, count, 0);
    if (ret == -1) {
        free(asset->buffer);
        asset->buffer = NULL;
    }
    return asset->buffer;
}

off_t AAsset_getLength(AAsset* asset) {
    struct stat buf;
    fstat(asset->fd, &buf);
    return buf.st_size;
}

off64_t AAsset_getLength64(AAsset* asset) {
    struct stat64 buf;
    fstat64(asset->fd, &buf);
    return buf.st_size;
}

off64_t AAsset_getRemainingLength64(AAsset* asset) {
    return AAsset_getLength64(asset) - lseek64(asset->fd, 0, SEEK_CUR);
}
