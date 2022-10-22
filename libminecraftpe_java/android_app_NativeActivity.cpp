#define _LARGEFILE64_SOURCE

#include <android_app_NativeActivity.h>
#include <aglinker.h>
#include <stdlib.h>

extern "C" AInputEvent* AInputEvent_create(int32_t type, int32_t source, ...);
extern "C" AInputQueue* AInputQueue_create();
extern "C" void AInputQueue_pushEvent(AInputQueue* queue, AInputEvent* inEvent);

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "NAX GLFW Error: %s\n", description);
}

NativeActivity::NativeActivity(JavaVM* vm, JNIEnv* env, jobject clazz, void* lib) {
    if (lib == NULL) {
        fprintf(stderr, "%s\n", aglinker_dlerror());
        exit(-1);
    }
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return;

    native_act.vm = vm;
    native_act.env = env;
    native_act.clazz = clazz;
    native_act.sdkVersion = 19;
    native_act.callbacks = &act_callbacks;
    
    native_lib = lib;
    this->ANativeActivity_onCreate = (void (*)(ANativeActivity*, void*, size_t))aglinker_dlsym(native_lib, "ANativeActivity_onCreate");
}

NativeActivity::~NativeActivity() {

}

void NativeActivity::onCreate() {
    this->ANativeActivity_onCreate(&native_act, NULL, 0);
}

void NativeActivity::onStart() {
    if (native_act.callbacks->onStart) {
        native_act.callbacks->onStart(&native_act);
    }
}

void NativeActivity::onResume() {
    if (native_act.callbacks->onResume) {
        native_act.callbacks->onResume(&native_act);
    }
}

void NativeActivity::onPause() {
    if (native_act.callbacks->onPause) {
        native_act.callbacks->onPause(&native_act);
    }
}

void NativeActivity::onStop() {
    if (native_act.callbacks->onStop) {
        native_act.callbacks->onStop(&native_act);
    }
}

void NativeActivity::onDestroy() {
    if (native_act.callbacks->onDestroy) {
        native_act.callbacks->onDestroy(&native_act);
    }
}

bool NativeActivity::dispatchKeyEvent(int keycode, int action) {
    return false;
}

void NativeActivity::onMouseButtonEvent(int button, int action, int mods) {
    double xpos;
    double ypos;
    glfwGetCursorPos(glfw_window, &xpos, &ypos);
    int motion_action = AMOTION_EVENT_ACTION_MOVE;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            motion_action = AMOTION_EVENT_ACTION_DOWN;
        }
        else {
            motion_action = AMOTION_EVENT_ACTION_UP;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            motion_action = AMOTION_EVENT_ACTION_DOWN;
        }
    }
    AInputEvent* motion_event =  AInputEvent_create(AINPUT_EVENT_TYPE_MOTION, AINPUT_SOURCE_MOUSE,
                                                    motion_action, xpos, ypos);
    AInputQueue_pushEvent(queue, motion_event);
}

void NativeActivity::onCursorPosEvent(double xpos, double ypos) {
    int state = glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_LEFT);
    int motion_action = AMOTION_EVENT_ACTION_MOVE;
    AInputEvent* motion_event =  AInputEvent_create(AINPUT_EVENT_TYPE_MOTION, AINPUT_SOURCE_MOUSE,
                                                    motion_action, xpos, ypos);
    AInputQueue_pushEvent(queue, motion_event);
}

static int glfw_key_to_android_key(int glfw_key) {
    switch (glfw_key) {
#define CASE_FOR(letter) case GLFW_KEY_##letter: return AKEYCODE_##letter
        CASE_FOR(1);
        CASE_FOR(2);
        CASE_FOR(3);
        CASE_FOR(4);
        CASE_FOR(5);
        CASE_FOR(6);
        CASE_FOR(7);
        CASE_FOR(8);
        CASE_FOR(9);
        CASE_FOR(0);
        CASE_FOR(A);
        CASE_FOR(B);
        CASE_FOR(C);
        CASE_FOR(D);
        CASE_FOR(E);
        CASE_FOR(F);
        CASE_FOR(G);
        CASE_FOR(H);
        CASE_FOR(I);
        CASE_FOR(J);
        CASE_FOR(K);
        CASE_FOR(L);
        CASE_FOR(M);
        CASE_FOR(N);
        CASE_FOR(O);
        CASE_FOR(P);
        CASE_FOR(Q);
        CASE_FOR(R);
        CASE_FOR(S);
        CASE_FOR(T);
        CASE_FOR(U);
        CASE_FOR(V);
        CASE_FOR(W);
        CASE_FOR(X);
        CASE_FOR(Y);
        CASE_FOR(Z);
        CASE_FOR(ENTER);
        CASE_FOR(SPACE);
        CASE_FOR(ESCAPE);
        CASE_FOR(TAB);
#undef CASE_FOR
        case GLFW_KEY_LEFT_SHIFT:
            return AKEYCODE_SHIFT_LEFT;
        case GLFW_KEY_RIGHT_SHIFT:
            return AKEYCODE_SHIFT_RIGHT;
        case GLFW_KEY_LEFT_ALT:
            return AKEYCODE_ALT_LEFT;
        case GLFW_KEY_RIGHT_ALT:
            return AKEYCODE_ALT_RIGHT;
        case GLFW_KEY_LEFT_CONTROL:
            return AKEYCODE_CTRL_LEFT;
        case GLFW_KEY_RIGHT_CONTROL:
            return AKEYCODE_CTRL_RIGHT;
        case GLFW_KEY_LEFT_SUPER:
            return AKEYCODE_BACK;
    }
    return AKEYCODE_UNKNOWN;
}

void NativeActivity::onKeyEvent(int key, int scancode, int action, int mods) {
    int key_code = glfw_key_to_android_key(key);
    int key_action = (action == GLFW_RELEASE) ? AKEY_EVENT_ACTION_UP : AKEY_EVENT_ACTION_DOWN;
    if (this->dispatchKeyEvent(key_code, key_action)) {
        return;
    }
    AInputEvent* key_event =  AInputEvent_create(AINPUT_EVENT_TYPE_KEY, AINPUT_SOURCE_KEYBOARD,
                                                 key_action, key_code, 0, 0);
    AInputQueue_pushEvent(queue, key_event);
}

static void glfw_window_size_callback(GLFWwindow* window, int width, int height) {

}

static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    NativeActivity* na = (NativeActivity*)glfwGetWindowUserPointer(window);
    na->onMouseButtonEvent(button, action, mods);
}

static void glfw_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    NativeActivity* na = (NativeActivity*)glfwGetWindowUserPointer(window);
    na->onCursorPosEvent(xpos, ypos);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    NativeActivity* na = (NativeActivity*)glfwGetWindowUserPointer(window);
    na->onKeyEvent(key, scancode, action, mods);
}

void NativeActivity::main(int width, int height) {
    this->onCreate();
    this->onStart();
    this->onResume();
    // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfw_window = glfwCreateWindow(width, height, "NativeActivity over X11", NULL, NULL);
    glfwSetWindowUserPointer(glfw_window, this);
    native_win.x11_win = glfwGetX11Window(glfw_window);
    
    glfwSetWindowSizeCallback(glfw_window, glfw_window_size_callback);
    
    if (native_act.callbacks->onNativeWindowCreated) {
        native_act.callbacks->onNativeWindowCreated(&native_act, native_win.a_win);
    }

    queue = AInputQueue_create();
    if (native_act.callbacks->onInputQueueCreated) {
        native_act.callbacks->onInputQueueCreated(&native_act, queue);
    }
    glfwSetMouseButtonCallback(glfw_window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(glfw_window, glfw_cursor_pos_callback);
    glfwSetKeyCallback(glfw_window, glfw_key_callback);

    while (!glfwWindowShouldClose(glfw_window)) {
        glfwPollEvents();
    }
    if (native_act.callbacks->onInputQueueDestroyed) {
        native_act.callbacks->onInputQueueDestroyed(&native_act, queue);
    }
    
    this->onPause();
    this->onStop();
    this->onDestroy();
}
