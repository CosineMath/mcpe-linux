
#include <string.h>

const char* get_bionic_symbol_name(const char* symbol) {
#define matches_symbol(name) \
    if (strcmp(symbol, #name) == 0) { \
        return "bionic_"#name; \
    }

    matches_symbol(pthread_self)
    matches_symbol(pthread_attr_init)
    matches_symbol(pthread_attr_destroy)
    matches_symbol(pthread_attr_setdetachstate)
    matches_symbol(pthread_attr_getdetachstate)
    matches_symbol(pthread_attr_setstacksize)
    matches_symbol(pthread_attr_setschedparam)
    matches_symbol(pthread_create)
    matches_symbol(pthread_cond_init)
    matches_symbol(pthread_cond_destroy)
    matches_symbol(pthread_cond_broadcast)
    matches_symbol(pthread_cond_signal)
    matches_symbol(pthread_cond_timedwait)
    matches_symbol(pthread_cond_wait)
    matches_symbol(pthread_mutexattr_init)
    matches_symbol(pthread_mutexattr_destroy)
    matches_symbol(pthread_mutexattr_settype)
    matches_symbol(pthread_mutex_init)
    matches_symbol(pthread_mutex_destroy)
    matches_symbol(pthread_mutex_lock)
    matches_symbol(pthread_mutex_unlock)
    matches_symbol(pthread_mutex_trylock)
    matches_symbol(pthread_rwlock_init)
    matches_symbol(pthread_rwlock_destroy)
    matches_symbol(pthread_rwlock_rdlock)
    matches_symbol(pthread_rwlock_wrlock)
    matches_symbol(pthread_rwlock_unlock)
    matches_symbol(pthread_atfork)
    matches_symbol(pthread_once)
    matches_symbol(pthread_key_create)
    matches_symbol(pthread_key_delete)
    matches_symbol(pthread_getspecific)
    matches_symbol(pthread_setspecific)
    matches_symbol(pthread_equal)
    matches_symbol(pthread_kill)
    matches_symbol(pthread_join)

    if (strcmp(symbol, "__pthread_cleanup_push") == 0) {
        return "bionic_pthread_cleanup_push";
    }
    if (strcmp(symbol, "__pthread_cleanup_pop") == 0) {
        return "bionic_pthread_cleanup_pop";
    }

    matches_symbol(sem_init)
    matches_symbol(sem_destroy)
    matches_symbol(sem_wait)
    matches_symbol(sem_post)

    // stdio
    matches_symbol(fopen)
    matches_symbol(fdopen)
    matches_symbol(fclose)
    matches_symbol(fflush)
    matches_symbol(fread)
    matches_symbol(fwrite)
    matches_symbol(fputs)
    matches_symbol(fgets)
    matches_symbol(fprintf)
    matches_symbol(fscanf)
    matches_symbol(getc)
    matches_symbol(ungetc)
    matches_symbol(putc)
    matches_symbol(fgetc)
    matches_symbol(fputc)
    matches_symbol(fseek)
    matches_symbol(fseeko)
    matches_symbol(ftell)
    matches_symbol(ftello)
    matches_symbol(setvbuf)
    matches_symbol(feof)
    matches_symbol(getwc)
    matches_symbol(ungetwc)
    matches_symbol(putwc)
    matches_symbol(rewind)
    matches_symbol(vfprintf)
    matches_symbol(vfscanf)
    // dirent
    matches_symbol(opendir)
    matches_symbol(fdopendir)
    matches_symbol(closedir)
    matches_symbol(rewinddir)
    matches_symbol(dirfd)
    matches_symbol(readdir)
    // netdb
    matches_symbol(getaddrinfo)
    matches_symbol(freeaddrinfo)
    matches_symbol(getnameinfo)
#undef matches_symbol
    return symbol;
}
