
#include <errno.h>

volatile int*  __errno( void ) {
    return &errno;
}
