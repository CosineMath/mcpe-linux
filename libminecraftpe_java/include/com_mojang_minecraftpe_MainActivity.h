#pragma once
#include <android_app_NativeActivity.h>

#include <stdarg.h>

class MainActivity : public NativeActivity {
private:
    int mScreenWidth;
    int mScreenHeight;
    jstring mDeviceModel;
    jstring mLocale;
    jstring mExternalStoragePath;
    jstring mUUID;
    int assetsRootFd;
    jboolean (*nativeKeyHandler)(JNIEnv*, jobject, jint, jint);
public:
    MainActivity(JavaVM* vm, JNIEnv* env);
    virtual void onCreate();
    virtual void onDestroy();
    virtual void main(int width, int height);
    virtual bool dispatchKeyEvent(int keycode, int action);
public:
    int getScreenWidth();
    int getScreenHeight();
    int getAndroidVersion();
    jstring getDeviceModel();
    jstring getLocale();
    jstring getExternalStoragePath();
    jobjectArray getBroadcastAddresses();
    jobjectArray getIPAddresses();
    bool isFirstSnooperStart();
    bool hasHardwareChanged();
    bool isTablet();
    bool hasWriteExternalStoragePermission();
    long long getTotalMemory();
    float getPixelsPerMillimeter();
    bool isNetworkEnabled();
    jbyteArray getFileDataBytes(va_list ap);
    jstring createUUID();
    jintArray getImageData(va_list ap);
    void tick();
    jint getCursorPosition();
    void initializeXboxLive(va_list ap);

    void setAssetsRoot(const char* root);
    void setSdcardRoot(const char* root);
};
