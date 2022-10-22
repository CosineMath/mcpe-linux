#ifndef _AGLINKER_H_
#define _AGLINKER_H_

#include <dlfcn.h>

#ifdef __cplusplus
#define AGLINKER_EXPORT extern "C" __attribute__((visibility("default")))
#else
#define AGLINKER_EXPORT __attribute__((visibility("default")))
#endif

AGLINKER_EXPORT void aglinker_init(const char* ld_debug, const char* ld_preload, const char* ld_library_path);
AGLINKER_EXPORT const char* aglinker_dlerror();
AGLINKER_EXPORT void android_update_LD_LIBRARY_PATH(const char* ld_library_path);
AGLINKER_EXPORT void* aglinker_dlopen(const char* filename, int flags);
AGLINKER_EXPORT void* aglinker_dlextlib(const char* filename, void* (*ext_dlsym)(void*, const char*));
AGLINKER_EXPORT void* aglinker_dlsym(void* handle, const char* symbol);
AGLINKER_EXPORT int aglinker_dlclose(void* handle);

#endif
