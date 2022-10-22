
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <syscall.h>

#include <stdbool.h>

// #define BIONIC_PTHREAD_DEBUG

#ifdef BIONIC_PTHREAD_DEBUG
static pthread_mutex_t debugMutex = PTHREAD_MUTEX_INITIALIZER;
#define TRACE(x...) do { \
    pthread_mutex_lock(&debugMutex); \
    fprintf(stderr, "libbionic: %d: %s() ", gettid_wrapper(), __func__); \
    fprintf(stderr, x); \
    pthread_mutex_unlock(&debugMutex); \
} while (0) 
#else
#define TRACE(x...)
#endif

pid_t gettid_wrapper() {
    return syscall(__NR_gettid);
}

typedef struct {
    pthread_attr_t* impl;
    // something else...
} bionic_pthread_attr_t;

typedef struct {
    pthread_mutex_t* impl;
    // something else...
} bionic_pthread_mutex_t;

typedef struct {
    pthread_cond_t* impl;
    // something else...
} bionic_pthread_cond_t;

pthread_t bionic_pthread_self() {
    return pthread_self();
}

int bionic_pthread_attr_init(bionic_pthread_attr_t *attr) {
    attr->impl = malloc(sizeof(pthread_attr_t));
    return pthread_attr_init(attr->impl);
}

int bionic_pthread_attr_destroy(bionic_pthread_attr_t *attr) {
    int ret = pthread_attr_destroy(attr->impl);
    free(attr->impl);
    attr->impl = NULL;
    return ret;
}

int bionic_pthread_attr_setdetachstate(bionic_pthread_attr_t *attr, int detachstate) {
    return pthread_attr_setdetachstate(attr->impl, detachstate);
}

int bionic_pthread_attr_getdetachstate(bionic_pthread_attr_t *attr,
                                       int *detachstate) {
    return pthread_attr_getdetachstate(attr->impl, detachstate);
}

int bionic_pthread_attr_setstacksize(bionic_pthread_attr_t *attr, size_t stacksize) {
    return pthread_attr_setstacksize(attr->impl, stacksize);
}

int bionic_pthread_attr_setschedparam(bionic_pthread_attr_t *restrict attr,
                                     const struct sched_param *restrict param) {
    return pthread_attr_setschedparam(attr->impl, param);
}

int bionic_pthread_create(pthread_t *thread, const bionic_pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg) {
    pthread_attr_t* impl = NULL;
    if (attr != NULL) {
        impl = attr->impl;
    }
    return pthread_create(thread, impl, start_routine, arg);
}

int bionic_pthread_cond_init(bionic_pthread_cond_t *restrict cond,
           const pthread_condattr_t *restrict attr) {
    // May leak if pthread_cond_init() is called mutiple times
    // But it is not my fault
    cond->impl = malloc(sizeof(pthread_cond_t));
    TRACE("face = %p, impl = %p\n", cond, cond->impl);
    return pthread_cond_init(cond->impl, attr);
}

static void bionic_static_PTHREAD_COND_INITIALIZER(bionic_pthread_cond_t *cond) {
    static pthread_mutex_t staticInitMutex = PTHREAD_MUTEX_INITIALIZER;
    if (cond == NULL) {
        return;
    }
    pthread_mutex_lock(&staticInitMutex);
    if (cond->impl == NULL) {
        bionic_pthread_cond_init(cond, NULL);
    }
    pthread_mutex_unlock(&staticInitMutex);
}

static pthread_cond_t* bionic_pthread_cond_get_impl(bionic_pthread_cond_t* cond) {
    if (cond->impl == NULL) {
        bionic_static_PTHREAD_COND_INITIALIZER(cond);
    }
    return cond->impl;
}
static pthread_mutex_t* bionic_pthread_mutex_get_impl(bionic_pthread_mutex_t* mutex);

int bionic_pthread_cond_destroy(bionic_pthread_cond_t *cond) {
    TRACE("face = %p, impl = %p\n", cond, cond->impl);
    if (cond->impl == NULL) {
        return 0;
    }
    int ret = pthread_cond_destroy(cond->impl);
    free(cond->impl);
    cond->impl = NULL;
    return ret;
}

int bionic_pthread_cond_broadcast(bionic_pthread_cond_t *cond) {
    bionic_pthread_cond_get_impl(cond);
    TRACE("face = %p, impl = %p\n", cond, cond->impl);
    return pthread_cond_broadcast(cond->impl);
}

int bionic_pthread_cond_signal(bionic_pthread_cond_t *cond) {
    bionic_pthread_cond_get_impl(cond);
    TRACE("face = %p, impl = %p\n", cond, cond->impl);
    return pthread_cond_signal(cond->impl);
}

int bionic_pthread_cond_timedwait(bionic_pthread_cond_t *restrict cond,
           bionic_pthread_mutex_t *restrict mutex,
           const struct timespec *restrict abstime) {
    bionic_pthread_cond_get_impl(cond);
    bionic_pthread_mutex_get_impl(mutex);
    TRACE("cond(face = %p, impl = %p), mutex(face = %p, impl = %p)\n", cond, cond->impl, mutex, mutex->impl);
    return pthread_cond_timedwait(cond->impl, mutex->impl, abstime);
}

int bionic_pthread_cond_wait(bionic_pthread_cond_t *restrict cond,
           bionic_pthread_mutex_t *restrict mutex) {
    bionic_pthread_cond_get_impl(cond);
    bionic_pthread_mutex_get_impl(mutex);
    TRACE("cond(face = %p, impl = %p), mutex(face = %p, impl = %p)\n", cond, cond->impl, mutex, mutex->impl);
    return pthread_cond_wait(cond->impl, mutex->impl);
}

int bionic_pthread_mutexattr_init(pthread_mutexattr_t *attr) {
    return pthread_mutexattr_init(attr);
}

int bionic_pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
    return pthread_mutexattr_destroy(attr);
}

int bionic_pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
    return pthread_mutexattr_settype(attr, type);
}

int bionic_pthread_mutex_init(bionic_pthread_mutex_t *restrict mutex,
           const pthread_mutexattr_t *restrict attr) {
    // May leak if pthread_mutex_init() is called mutiple times
    // But it is not my fault
    mutex->impl = malloc(sizeof(pthread_mutex_t));
    TRACE("face = %p, impl = %p\n", mutex, mutex->impl);
    return pthread_mutex_init(mutex->impl, attr);
}

#define  __PTHREAD_MUTEX_INIT_VALUE            0
#define  __PTHREAD_RECURSIVE_MUTEX_INIT_VALUE  0x4000
#define  __PTHREAD_ERRORCHECK_MUTEX_INIT_VALUE 0x8000

static int bionic_pthread_mutex_initialized(bionic_pthread_mutex_t* mutex) {
    return (mutex->impl != (void*)__PTHREAD_MUTEX_INIT_VALUE &&
            mutex->impl != (void*)__PTHREAD_RECURSIVE_MUTEX_INIT_VALUE &&
            mutex->impl != (void*)__PTHREAD_ERRORCHECK_MUTEX_INIT_VALUE);
}

static void bionic_static_PTHREAD_MUTEX_INITIALIZER(bionic_pthread_mutex_t *mutex) {
    static pthread_mutex_t staticInitMutex = PTHREAD_MUTEX_INITIALIZER;
    if (mutex == NULL) {
        return;
    }
    pthread_mutex_lock(&staticInitMutex);
    if (!bionic_pthread_mutex_initialized(mutex)) {
        int value = (int)(intptr_t)mutex->impl;
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        if (value == __PTHREAD_RECURSIVE_MUTEX_INIT_VALUE) {
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        }
        else if (value == __PTHREAD_ERRORCHECK_MUTEX_INIT_VALUE) {
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        }
        bionic_pthread_mutex_init(mutex, &attr);
        pthread_mutexattr_destroy(&attr);
    }
    pthread_mutex_unlock(&staticInitMutex);
}

static pthread_mutex_t* bionic_pthread_mutex_get_impl(bionic_pthread_mutex_t* mutex) {
    if (!bionic_pthread_mutex_initialized(mutex)) {
        bionic_static_PTHREAD_MUTEX_INITIALIZER(mutex);
    }
    return mutex->impl;
}

int bionic_pthread_mutex_destroy(bionic_pthread_mutex_t *mutex) {
    TRACE("face = %p, impl = %p\n", mutex, mutex->impl);
    if (!bionic_pthread_mutex_initialized(mutex)) {
        return 0;
    }
    int ret = pthread_mutex_destroy(mutex->impl);
    free(mutex->impl);
    mutex->impl = NULL;
    return ret;
}

int bionic_pthread_mutex_lock(bionic_pthread_mutex_t *mutex) {
    bionic_pthread_mutex_get_impl(mutex);
    TRACE("face = %p, impl = %p\n", mutex, mutex->impl);
    return pthread_mutex_lock(mutex->impl);
}

int bionic_pthread_mutex_unlock(bionic_pthread_mutex_t *mutex) {
    bionic_pthread_mutex_get_impl(mutex);
    TRACE("face = %p, impl = %p\n", mutex, mutex->impl);
    return pthread_mutex_unlock(mutex->impl);
}

int bionic_pthread_mutex_trylock(bionic_pthread_mutex_t *mutex) {
    bionic_pthread_mutex_get_impl(mutex);
    TRACE("face = %p, impl = %p\n", mutex, mutex->impl);
    return pthread_mutex_trylock(mutex->impl);
}

int bionic_pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
                               const pthread_rwlockattr_t *restrict attr) {
    return pthread_rwlock_init(rwlock, NULL);
}

int bionic_pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
    return pthread_rwlock_destroy(rwlock);
}

int bionic_pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
    return pthread_rwlock_rdlock(rwlock);
}

int bionic_pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
    return pthread_rwlock_wrlock(rwlock);
}

int bionic_pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
    return pthread_rwlock_unlock(rwlock);
}          

int bionic_pthread_atfork(void (*prepare)(void), void (*parent)(void),
                          void (*child)(void)) {
    return pthread_atfork(prepare, parent, child);
}

int bionic_pthread_once(pthread_once_t *once_control,
           void (*init_routine)(void)) {
    return pthread_once(once_control, init_routine);
}

int bionic_pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) {
    return pthread_key_create(key, destructor);
}

int bionic_pthread_key_delete(pthread_key_t key) {
    return pthread_key_delete(key);
}

void* bionic_pthread_getspecific(pthread_key_t key) {
    return pthread_getspecific(key);
}

int bionic_pthread_setspecific(pthread_key_t key, const void *value) {
    return pthread_setspecific(key, value);
}

int bionic_pthread_equal(pthread_t t1, pthread_t t2) {
    return pthread_equal(t1, t2);
}

int bionic_pthread_detach(pthread_t thread) {
    return pthread_detach(thread);
}

int bionic_pthread_kill(pthread_t thread, int sig) {
    return pthread_kill(thread, sig);
}

int bionic_pthread_join(pthread_t thread, void** retval) {
    return pthread_join(thread, retval);
}

void bionic_pthread_cleanup_push(void* c, void (*routine)(void *), void *arg) {
    // pthread_cleanup_push(routine, arg);
}

void bionic_pthread_cleanup_pop(void* c, int execute) {
    // pthread_cleanup_pop(execute);
}

typedef struct {
    sem_t* impl;
    // something else...
} fake_sem_t;

int bionic_sem_init(sem_t *s, int pshared, unsigned int value) {
    fake_sem_t* sem = (void*)s;
    if (sem->impl == NULL) {
        sem->impl = malloc(sizeof(sem_t));
    }
    return sem_init(sem->impl, pshared, value);
}

int bionic_sem_destroy(sem_t *s) {
    fake_sem_t* sem = (void*)s;
    int ret = sem_destroy(sem->impl);
    free(sem->impl);
    sem->impl = NULL;
    return ret;
}

int bionic_sem_wait(sem_t *s) {
    fake_sem_t* sem = (void*)s;
    return sem_wait(sem->impl);
}

int bionic_sem_post(sem_t *s) {
    fake_sem_t* sem = (void*)s;
    return sem_post(sem->impl);
}
