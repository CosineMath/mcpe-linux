
#include <unistd.h>
#include <sys/syscall.h>

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

long __futex_wait(long ftx, long val, long timeout) {
    return syscall(__NR_futex, ftx, FUTEX_WAIT, val, timeout);
}

long __futex_wake(long ftx, long count) {
    return syscall(__NR_futex, ftx, FUTEX_WAKE, count);
}
