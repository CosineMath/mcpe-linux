
#include <libjni.h>
#include <com_mojang_minecraftpe_MainActivity.h>
#include <aglinker.h>

#include <string.h>
#include <stdlib.h>
#include <sys/param.h>

class File {
private:
    jstring path;
public:
    File(jstring p) {
        path = p;
    }
    jstring getPath() {
        return path;
    }
public:
    static jclass clazz;
    static void cinit() {
        clazz = add_class("java/io/File", NULL);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
        jclass_add_method(clazz, TYPE_MEMBER, "getPath", "()Ljava/lang/String;", (void*)&File::getPath);
#pragma GCC diagnostic pop
    }
};

jclass File::clazz;

class ContextWrapper {
private:
    static jobject mFilesDir;
    static jobject mCacheDir;
public:
    static jobject getFilesDir(void* r) {
        return mFilesDir;
    }
    static jobject getCacheDir(void* r) {
        return mCacheDir;
    }
public:
    static jclass clazz;
    static void cinit(const char* dataDir, const char* tmpDir) {
        mFilesDir = jobject_new(File::clazz, "tmp_file", new File(jstring_new(dataDir)));
        mCacheDir = jobject_new(File::clazz, "tmp_file", new File(jstring_new(tmpDir)));
        clazz = add_class("android/content/ContextWrapper", NULL);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
        jclass_add_method(clazz, TYPE_MEMBER, "getFilesDir", "()Ljava/io/File;", (void*)&ContextWrapper::getFilesDir);
        jclass_add_method(clazz, TYPE_MEMBER, "getCacheDir", "()Ljava/io/File;", (void*)&ContextWrapper::getCacheDir);
#pragma GCC diagnostic pop
    }
};

jobject ContextWrapper::mFilesDir;
jobject ContextWrapper::mCacheDir;
jclass ContextWrapper::clazz;

class Signature {
public:
    static jboolean verify(void* r) {
        return true;
    }
public:
    static jclass clazz;
    static void cinit() {
        clazz = add_class("java/security/Signature", NULL);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
        jclass_add_method(clazz, TYPE_MEMBER, "verify", "([B)Z", (void*)&Signature::verify);
#pragma GCC diagnostic pop
    }
};

jclass Signature::clazz;

class Base64 {
public:
    static jbyteArray decode(va_list ap) {
        fprintf(stderr, "android/util/Base64::decode() is called!\n");
        jbyteArray retval = jbyteArray_new(4);
        char* arrbuf = jbyteArray_get(retval);
        arrbuf[0] = 1;
        arrbuf[1] = 0;
        arrbuf[2] = 0;
        arrbuf[3] = 0;
        return retval;
    }
public:
    static jclass clazz;
    static void cinit() {
        clazz = add_class("android/util/Base64", NULL);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
        jclass_add_method(clazz, TYPE_STATIC, "decode", "(Ljava/lang/String;I)[B", (void*)&Base64::decode);
#pragma GCC diagnostic pop
    }
};

jclass Base64::clazz;

class Store {
public:
    static jboolean receivedLicenseResponse(void* r) {
        return JNI_TRUE;
    }
public:
    static jclass clazz;
    static void cinit() {
        clazz = add_class("com/mojang/minecraftpe/store/Store", NULL);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
        jclass_add_method(clazz, TYPE_MEMBER, "receivedLicenseResponse", "()Z", (void*)&Store::receivedLicenseResponse);
#pragma GCC diagnostic pop
    }
};

jclass Store::clazz;

static __attribute__((noreturn)) void bad_argument(char* argv) {
    fprintf(stderr, "Bad argument: %s\n", argv);
    fprintf(stderr, "Help:\n");
    fprintf(stderr, "    -mcpe <path>\n");
    fprintf(stderr, "    -sdcard <path>\n");
    fprintf(stderr, "    -datadir <path>\n");
    fprintf(stderr, "    -tmpdir <path>\n");
    fprintf(stderr, "    -width <int>\n");
    fprintf(stderr, "    -height <int>\n");
    exit(-1);
}

extern "C" void runMainThread(int argc, char** argv) {
    libjni_init();
    JavaVM* vm = libjni_get_default_java_vm();
    JNIEnv* env = libjni_get_default_jni_env();

    const char* mcpeDir = NULL;
    const char* sdcardDir = getenv("HOME");
    const char* dataDir = "/tmp";
    const char* tmpDir = "/tmp";
    int width = 854;
    int height = 480;
    
    while (argc > 0) {
        if (argv[0] == NULL || strcmp(argv[0], "") == 0) {
            bad_argument(argv[0]);
        }
        else if (strcmp(argv[0], "-mcpe") == 0) {
            if (argc <= 1) {
                bad_argument(argv[0]);
            }
            mcpeDir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-sdcard") == 0) {
            if (argc <= 1) {
                bad_argument(argv[0]);
            }
            sdcardDir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-datadir") == 0) {
            if (argc <= 1) {
                bad_argument(argv[0]);
            }
            dataDir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-tmpdir") == 0) {
            if (argc <= 1) {
                bad_argument(argv[0]);
            }
            tmpDir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-width") == 0) {
            if (argc <= 1) {
                bad_argument(argv[0]);
            }
            width = atoi(argv[1]);
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-height") == 0) {
            if (argc <= 1) {
                bad_argument(argv[0]);
            }
            height = atoi(argv[1]);
            argc -= 2;
            argv += 2;
        }
        else {
            bad_argument(argv[0]);
        }
    }

    File::cinit();
    ContextWrapper::cinit(dataDir, tmpDir);
    Signature::cinit();
    Base64::cinit();
    Store::cinit();

    if (mcpeDir == NULL) {
        fprintf(stderr, "MCPE installation not specified.\n");
        bad_argument(argv[0]);
    }
    char pathBuf[PATH_MAX + 1];
    strncpy(pathBuf, mcpeDir, PATH_MAX);
    int mcpeDirPathLen = strlen(pathBuf);
    if (pathBuf[mcpeDirPathLen - 1] == '/') {
        pathBuf[mcpeDirPathLen--] = 0;
    }
    strncpy(pathBuf + mcpeDirPathLen, "/lib/x86", PATH_MAX - mcpeDirPathLen);
    fprintf(stderr, "android_update_LD_LIBRARY_PATH(%s)\n", pathBuf);
    android_update_LD_LIBRARY_PATH(pathBuf);

    strncpy(pathBuf + mcpeDirPathLen, "/assets", PATH_MAX - mcpeDirPathLen);

    MainActivity* main_act = new MainActivity(vm, env);
    fprintf(stderr, "MainActivity::setAssetsRoot(%s)\n", pathBuf);
    main_act->setAssetsRoot(pathBuf);
    main_act->setSdcardRoot(sdcardDir);
    main_act->main(width, height);    
}
