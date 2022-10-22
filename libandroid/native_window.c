
#include <android/native_window.h>
#include <stdio.h>

int32_t ANativeWindow_setBuffersGeometry(ANativeWindow* window,
        int32_t width, int32_t height, int32_t format) {
    fprintf(stderr, "ANativeWindow: Cannot change the format!\n");
    return 0;
}
