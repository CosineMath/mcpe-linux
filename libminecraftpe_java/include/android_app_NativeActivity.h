#pragma once
#include <android/native_activity.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <android/input.h>

class NativeActivity {
public:
    ANativeActivity native_act;
    union {
        ANativeWindow* a_win;
        Window x11_win;
    } native_win;
    void* native_lib;
    GLFWwindow* glfw_window;
    AInputQueue* queue;
private:
    struct ANativeActivityCallbacks act_callbacks;
    void (*ANativeActivity_onCreate)(ANativeActivity*, void*, size_t);
public:
    NativeActivity(JavaVM* vm, JNIEnv* env, jobject clazz, void* lib);
    virtual ~NativeActivity();
    virtual void onCreate();
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
    virtual void main(int width, int height);
    virtual bool dispatchKeyEvent(int keycode, int action);
public:
    void onMouseButtonEvent(int button, int action, int mods);
    void onCursorPosEvent(double xpos, double ypos);
    void onKeyEvent(int key, int scancode, int action, int mods);
};
