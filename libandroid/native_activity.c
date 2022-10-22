#define _LARGEFILE64_SOURCE
#include <android/native_activity.h>

void ANativeActivity_finish(ANativeActivity* activity) {
    fprintf(stderr, "ANativeActivity_finish() is called!\n");
}

