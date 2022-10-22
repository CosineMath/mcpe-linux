
#include <libjni.h>
#include <com_mojang_minecraftpe_MainActivity.h>
#include <aglinker.h>

#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <png.h>

class ScopedFd {
public:
    int fd;
public:
    ScopedFd(int i) {
        fd = i;
    }
    ~ScopedFd() {
        close(fd);
    }
};

class ScopedFp {
public:
    FILE* fp;
public:
    ScopedFp(FILE* p) {
        fp = p;
    }
    ~ScopedFp() {
        fclose(fp);
    }
};

MainActivity::MainActivity(JavaVM* vm, JNIEnv* env) : 
    NativeActivity(vm, env, (jobject)add_class("com/mojang/minecraftpe/MainActivity", this),
                   aglinker_dlopen("libminecraftpe.so", RTLD_LAZY)) {

    jclass clazz = (jclass)native_act.clazz;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
    jclass_add_method(clazz, TYPE_MEMBER, "getScreenWidth", "()I", (void*)&MainActivity::getScreenWidth);
    jclass_add_method(clazz, TYPE_MEMBER, "getScreenHeight", "()I", (void*)&MainActivity::getScreenHeight);
    jclass_add_method(clazz, TYPE_MEMBER, "getAndroidVersion", "()I", (void*)&MainActivity::getAndroidVersion);
    jclass_add_method(clazz, TYPE_MEMBER, "getDeviceModel", "()Ljava/lang/String;", (void*)&MainActivity::getDeviceModel);
    jclass_add_method(clazz, TYPE_MEMBER, "getDeviceId", "()Ljava/lang/String;", (void*)&MainActivity::getDeviceModel);
    jclass_add_method(clazz, TYPE_MEMBER, "getPlatformStringVar", "(I)Ljava/lang/String;", (void*)&MainActivity::getDeviceModel);
    jclass_add_method(clazz, TYPE_MEMBER, "getLocale", "()Ljava/lang/String;", (void*)&MainActivity::getLocale);
    jclass_add_method(clazz, TYPE_MEMBER, "getExternalStoragePath", "()Ljava/lang/String;", (void*)&MainActivity::getExternalStoragePath);
    jclass_add_method(clazz, TYPE_MEMBER, "getBroadcastAddresses", "()[Ljava/lang/String;", (void*)&MainActivity::getBroadcastAddresses);
    jclass_add_method(clazz, TYPE_MEMBER, "getIPAddresses", "()[Ljava/lang/String;", (void*)&MainActivity::getIPAddresses);
    jclass_add_method(clazz, TYPE_MEMBER, "isFirstSnooperStart", "()Z", (void*)&MainActivity::isFirstSnooperStart);
    jclass_add_method(clazz, TYPE_MEMBER, "hasHardwareChanged", "()Z", (void*)&MainActivity::hasHardwareChanged);
    jclass_add_method(clazz, TYPE_MEMBER, "isTablet", "()Z", (void*)&MainActivity::isTablet);
    jclass_add_method(clazz, TYPE_MEMBER, "hasWriteExternalStoragePermission", "()Z", (void*)&MainActivity::hasWriteExternalStoragePermission);
    jclass_add_method(clazz, TYPE_MEMBER, "getTotalMemory", "()J", (void*)&MainActivity::getTotalMemory);
    jclass_add_method(clazz, TYPE_MEMBER, "getMemoryLimit", "()J", (void*)&MainActivity::getTotalMemory);
    jclass_add_method(clazz, TYPE_MEMBER, "getPixelsPerMillimeter", "()F", (void*)&MainActivity::getPixelsPerMillimeter);
    jclass_add_method(clazz, TYPE_MEMBER, "isNetworkEnabled", "(Z)Z", (void*)&MainActivity::isNetworkEnabled);
    jclass_add_method(clazz, TYPE_MEMBER, "getFileDataBytes", "(Ljava/lang/String;)[B", (void*)&MainActivity::getFileDataBytes);
    jclass_add_method(clazz, TYPE_MEMBER, "createUUID", "()Ljava/lang/String;", (void*)&MainActivity::createUUID);
    jclass_add_method(clazz, TYPE_MEMBER, "getImageData", "(Ljava/lang/String;Z)[I", (void*)&MainActivity::getImageData);
    jclass_add_method(clazz, TYPE_MEMBER, "getImageData", "(Ljava/lang/String;)[I", (void*)&MainActivity::getImageData);
    jclass_add_method(clazz, TYPE_MEMBER, "initializeXboxLive", "(JJ)V", (void*)&MainActivity::initializeXboxLive);
    jclass_add_method(clazz, TYPE_MEMBER, "tick", "()V", (void*)&MainActivity::tick);
    jclass_add_method(clazz, TYPE_MEMBER, "getCursorPosition", "()I", (void*)&MainActivity::getCursorPosition);
#pragma GCC diagnostic pop

    mDeviceModel = NULL;
    mLocale = NULL;
    mExternalStoragePath = NULL;
    mUUID = NULL;
    
    nativeKeyHandler = (jboolean (*)(JNIEnv*, jobject, jint, jint))
        aglinker_dlsym(native_lib, "Java_com_mojang_minecraftpe_MainActivity_nativeKeyHandler");
    
    jint (*JNI_OnLoad)(JavaVM*, void*) = (jint (*)(JavaVM*, void*))aglinker_dlsym(native_lib, "JNI_OnLoad");
    JNI_OnLoad(vm, NULL);
}

void MainActivity::onCreate() {
    NativeActivity::onCreate();
    void (*nativeRegisterThis)(JNIEnv*, jobject) = 
        (void (*)(JNIEnv*, jobject))aglinker_dlsym(native_lib, "Java_com_mojang_minecraftpe_MainActivity_nativeRegisterThis");
    if (nativeRegisterThis) {
        nativeRegisterThis(native_act.env, native_act.clazz);
    }
}

void MainActivity::onDestroy() {
    void (*nativeUnregisterThis)(JNIEnv*, jobject) = 
        (void (*)(JNIEnv*, jobject))aglinker_dlsym(native_lib, "Java_com_mojang_minecraftpe_MainActivity_nativeUnregisterThis");
    if (nativeUnregisterThis) {
        nativeUnregisterThis(native_act.env, native_act.clazz);
    }
    NativeActivity::onDestroy();
}

bool MainActivity::dispatchKeyEvent(int keycode, int action) {
    if (nativeKeyHandler(native_act.env, native_act.clazz, keycode, action)) {
         return true;
    }
    return NativeActivity::dispatchKeyEvent(keycode, action);
}

int MainActivity::getScreenWidth() {
    return mScreenWidth;
}

int MainActivity::getScreenHeight() {
    return mScreenHeight;
}

int MainActivity::getAndroidVersion() {
    return 19;
}

jstring MainActivity::getDeviceModel() {
    if (mDeviceModel == NULL) {
        mDeviceModel = jstring_new("i686-linux-gnu");
    }
    return mDeviceModel;
}

jstring MainActivity::getLocale() {
    if (mLocale == NULL) {
        mLocale = jstring_new("C");
    }
    return mLocale;
}

jstring MainActivity::getExternalStoragePath() {
    return mExternalStoragePath;
}

jobjectArray MainActivity::getBroadcastAddresses() {
    return jobjectArray_new(0);
}

jobjectArray MainActivity::getIPAddresses() {
    return jobjectArray_new(0);
}

jstring MainActivity::createUUID() {
    if (mUUID == NULL) {
        mUUID = jstring_new("f940db7497ea30aca0f3f57f0ab8bbf3");
    }
    return mUUID;
}

bool MainActivity::isFirstSnooperStart() {
    return false;
}

bool MainActivity::hasHardwareChanged() {
    return false;
}

bool MainActivity::isTablet() {
    return false;
}

bool MainActivity::hasWriteExternalStoragePermission() {
    return true;
}

long long MainActivity::getTotalMemory() {
    return 1 * 1024 * 1024 * 1024;
}

float MainActivity::getPixelsPerMillimeter() {
    return 1.0f;
}

bool MainActivity::isNetworkEnabled() {
    return true;
}

void MainActivity::tick() {
    
}

jint MainActivity::getCursorPosition() {
    return -1;
}

void MainActivity::initializeXboxLive(va_list ap) {
    jlong xal = va_arg(ap, jlong);
    jlong xbl = va_arg(ap, jlong);
    void (*nativeInitializeXboxLive)(JNIEnv*, jobject, jlong, jlong) = 
        (void (*)(JNIEnv*, jobject, jlong, jlong))aglinker_dlsym(native_lib, "Java_com_mojang_minecraftpe_MainActivity_nativeInitializeXboxLive");
    if (nativeInitializeXboxLive) {
        nativeInitializeXboxLive(native_act.env, native_act.clazz, xal, xbl);
    }
}

jbyteArray MainActivity::getFileDataBytes(va_list ap) {
    jstring str = va_arg(ap, jstring);
    const char* name = jstring_get(str);
    if (name == NULL || strlen(name) == 0) {
        return NULL;
    }
    const char* url_prefix = "file:/android_asset/";
    if (strstr(name, url_prefix) == name) {
        name = name + strlen(url_prefix);
    }

    int fd = openat(assetsRootFd, name, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open asset(%s): %s\n", name, strerror(errno));
        return NULL;
    }
    ScopedFd scoped(fd);

    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1) {
        fprintf(stderr, "Failed to get stat of asset: %s\n", strerror(errno));
        return NULL;
    }
    size_t size = statbuf.st_size;
    
    jbyteArray arr = jbyteArray_new(size);
    char* buf = jbyteArray_get(arr);
    int count = read(fd, buf, size);
    if (count == -1) {
        fprintf(stderr, "Failed to read asset: %s\n", strerror(errno));
        jbyteArray_delete(arr);
        arr = NULL;
    }

    // fprintf(stderr, "MainActivity::getFileDataBytes(%s)\n", name);
    return arr;
}

jintArray MainActivity::getImageData(va_list ap) {
    jstring str = va_arg(ap, jstring);
    const char* name = jstring_get(str);
    if (name == NULL || strlen(name) == 0) {
        return NULL;
    }
    const char* url_prefix = "file:/android_asset/";
    if (strstr(name, url_prefix) == name) {
        name = name + strlen(url_prefix);
    }
    // jboolean is promoted
    // jboolean inTextureFolder = va_arg(ap, jint);

    int fd = openat(assetsRootFd, name, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open asset(%s): %s\n", name, strerror(errno));
        return NULL;
    }
    FILE* fp = fdopen(fd, "rb");
    ScopedFp scoped(fp);

    png_struct* png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        return NULL;
    }
    png_info* info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);
    
    unsigned int width;
    unsigned int height;
    int depth;
    int colorType;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &depth, &colorType, NULL, NULL, NULL);
    if (colorType == PNG_COLOR_TYPE_RGB) {
        png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    }
    else if (colorType != PNG_COLOR_TYPE_RGBA && colorType != PNG_COLOR_TYPE_PALETTE) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    if (colorType == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_set_tRNS_to_alpha(png_ptr);
        }
        else {
            png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
        }
    }
    else if (depth < 8 || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_expand(png_ptr);
    }
    if (depth == 16) {
        png_set_strip_16(png_ptr);
    }
    png_read_update_info(png_ptr, info_ptr);
    png_set_bgr(png_ptr);

    png_size_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    if ((row_bytes % sizeof(jint)) != 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    png_byte** rows = (png_byte**)calloc(height, sizeof(png_byte *));
    jintArray arr = jintArray_new(2 + height * (row_bytes / sizeof(jint)));
    int* arrbuf = jintArray_get(arr);
    arrbuf[0] = width;
    arrbuf[1] = height;
    char* data = (char*)(void*)&arrbuf[2];
    
    for (unsigned int i = 0; i < height; i++) {
        rows[i] = (png_byte*)&data[i * row_bytes];
    }

    png_read_image(png_ptr, rows);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    free(rows);
    
    // fprintf(stderr, "MainActivity::getImageData(%s)\n", name);
    return arr;
}

extern "C" AAssetManager* AAssetManager_create(int fd);

void MainActivity::setAssetsRoot(const char* root) {
    assetsRootFd = open(root, O_DIRECTORY|O_RDONLY);
    if (assetsRootFd == -1) {
        fprintf(stderr, "Failed to open assets root directory(%s): %s\n", root, strerror(errno));
        return;
    }
    native_act.assetManager = AAssetManager_create(assetsRootFd);
}

void MainActivity::setSdcardRoot(const char* root) {
    int fd = open(root, O_DIRECTORY|O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open sdcard root directory(%s): %s\n", root, strerror(errno));
    }
    close(fd);
    mExternalStoragePath = jstring_new(root);
}

void MainActivity::main(int width, int height) {
    mScreenWidth = width;
    mScreenHeight = height;
    NativeActivity::main(width, height);
}
