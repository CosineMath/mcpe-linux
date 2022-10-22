
#include <stdio.h>
#include <stdlib.h>
#include <aglinker.h>

void* null_dlsym(void* soinfo, const char* symbol) {
    return NULL;
}

void* libc_dlsym(void* soinfo, const char* symbol) {
    static void* libc = NULL;
    if (libc == NULL) {
        libc = dlopen("libc.so.6", RTLD_LAZY);
    }
    static void* libbionic = NULL;
    static const char* (*get_bionic_symbol_name)(const char*) = NULL;
    if (libbionic == NULL) {
        libbionic = dlopen("libbionic.so", RTLD_LAZY);
        get_bionic_symbol_name = dlsym(libbionic, "get_bionic_symbol_name");
    }

    symbol = get_bionic_symbol_name(symbol);

    void* addr = dlsym(libbionic, symbol);
    if (addr) {
        return addr;
    }
    return dlsym(libc, symbol);
}

#define ext_dlsym_soname(name, soname) \
void* lib##name##_dlsym(void* soinfo, const char* symbol) { \
    static void* lib = NULL; \
    if (lib == NULL) { \
        lib = dlopen(soname, RTLD_LAZY); \
        if (lib == NULL) { \
            fprintf(stderr, "mcpe-linux: %s\n", dlerror()); \
            return NULL; \
        } \
    } \
    return dlsym(lib, symbol); \
}

#define ext_dlsym(name) ext_dlsym_soname(name, "lib"#name".so")

ext_dlsym_soname(z, "libz.so.1")
ext_dlsym_soname(m, "libm.so.6")

ext_dlsym(android)
ext_dlsym(log)
ext_dlsym(fmod)

ext_dlsym(EGL)
ext_dlsym(GLESv2)
ext_dlsym(GLESv1_CM)

extern void runMainThread(int argc, char** argv);

int main(int argc, char** argv) {
    
    aglinker_init(
        "1", 
        "libbionic.so",
        ""
    );
    
    aglinker_dlextlib("libbionic.so", libc_dlsym);
    aglinker_dlextlib("libstdc++.so", null_dlsym);
    aglinker_dlextlib("libOpenSLES.so", null_dlsym);

#define call_dlextlib(name) aglinker_dlextlib("lib"#name".so", lib##name##_dlsym)
    call_dlextlib(c);
    call_dlextlib(z);
    call_dlextlib(m);
    call_dlextlib(android);
    call_dlextlib(log);
    call_dlextlib(fmod);
    call_dlextlib(EGL);
    call_dlextlib(GLESv2);
    call_dlextlib(GLESv1_CM);
#undef call_dlextlib

    runMainThread(argc - 1, argv + 1);
}
