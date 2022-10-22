
#include <libjni.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "hash_map.h"

// #define LIBJNI_DEBUG

#ifdef LIBJNI_DEBUG
#define TRACE(x...) fprintf(stderr, x)
#else
#define TRACE(x...)
#endif

char* strdup5(const char* s1, const char* s2, const char* s3, const char* s4, const char* s5) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    size_t len3 = strlen(s3);
    size_t len4 = strlen(s4);
    size_t len5 = strlen(s5);
    char* str = malloc(len1 + len2 + len3 + len4 + len5 + 1);
    strcpy(str, s1);
    strcpy(str + len1, s2);
    strcpy(str + len1 + len2, s3);
    strcpy(str + len1 + len2 + len3, s4);
    strcpy(str + len1 + len2 + len3 + len4, s5);
    return str;
}

static HashMap mClassMap;

struct _jclass;

struct _jobject {
    const char* name;
    void* ptr;
    struct _jclass* clazz;
};

struct _jclass {
    struct _jobject base;
    HashMap method_map;
    HashMap field_map;
};

jclass jclass_new(const char* name, void* ptr) {
    struct _jclass* clazz = malloc(sizeof(struct _jclass));
    clazz->base.name = strdup(name);
    clazz->base.clazz = clazz;
    clazz->base.ptr = ptr;
    HashMap_init(&clazz->method_map);
    HashMap_init(&clazz->field_map);
    return &clazz->base;
}

jclass add_class(const char* name, void* ptr) {
    jclass retval = HashMap_get(&mClassMap, name);
    if (retval == NULL) {
        retval = jclass_new(name, ptr);
        HashMap_put(&mClassMap, name, retval);
    }
    return retval;
}

jobject jobject_new(jclass clazz, const char* name, void* ptr) {
    jobject retval = malloc(sizeof(struct _jobject));
    retval->name = strdup(name);
    retval->clazz = (struct _jclass*)clazz;
    retval->ptr = ptr;
    return retval;
}

void jobject_delete(jobject obj) {
    free((void*)obj->name);
    free(obj);
}

struct _jmethodID {
    const char* name;
    void* fake_func;
    int type;
};

jmethodID jmethod_new(int type, char* full_name, void* ptr) {
    jmethodID method = malloc(sizeof(struct _jmethodID));
    method->name = full_name;
    method->fake_func = ptr;
    method->type = type;
    return method;
}

jmethodID jclass_add_method(jclass clz, int type, const char *name, const char *sig, void* ptr) {
    struct _jclass* clazz = (struct _jclass*)clz;
    char* full_name = strdup5(clazz->base.name, "$", name, "$", sig);
    jmethodID retval = HashMap_get(&clazz->method_map, full_name);
    if (retval == NULL) {
        retval = jmethod_new(type, full_name, ptr);
        HashMap_put(&clazz->method_map, full_name, retval);
    }
    else {
        free(full_name);
    }
    return retval;
}

struct _jfieldID {
    const char* name;
    void* fake_ptr;
    int type;
};

jfieldID jfield_new(int type, char* full_name, void* ptr) {
    jfieldID field = malloc(sizeof(struct _jfieldID));
    field->name = full_name;
    field->fake_ptr = ptr;
    field->type = type;
    return field;
}

jfieldID jclass_add_field(jclass clz, int type, const char *name, const char *sig, void* ptr) {
    struct _jclass* clazz = (struct _jclass*)clz;
    char* full_name = strdup5(clazz->base.name, "$", name, "$", sig);
    jfieldID retval = HashMap_get(&clazz->field_map, full_name);
    if (retval == NULL) {
        retval = jfield_new(type, full_name, ptr);
        HashMap_put(&clazz->field_map, full_name, retval);
    }
    else {
        free(full_name);
    }
    return retval;
}

const char* jstring_get(jstring str) {
    if (str == NULL) {
        return NULL;
    }
    return str->ptr;
}

jstring jstring_new(const char* utf) {
    jclass clazz = add_class("java/lang/String", NULL);
    jstring retval = jobject_new(clazz, "tmp_str", strdup(utf));
    return retval;
}

struct _jarray {
    struct _jobject base;
    int length;
};

jarray jarray_new(int size, int length) {
    struct _jarray* retval = malloc(sizeof(struct _jarray));
    retval->base.name = strdup("tmp_arr");
    retval->base.clazz = NULL;
    if (length != 0) {
        retval->base.ptr = calloc(length, size);
    }
    else {
        retval->base.ptr = NULL;
    }
    retval->length = length;
    return (jarray)retval;
}

void* jarray_get(jarray arr) {
    return arr->ptr;
}

void jarray_delete(jarray arr) {
    free(arr->ptr);
    jobject_delete(arr);
}

jbyteArray jbyteArray_new(int length) {
    return jarray_new(sizeof(jbyte), length);
}

char* jbyteArray_get(jbyteArray arr) {
    return jarray_get(arr);
}

void jbyteArray_delete(jbyteArray arr) {
    jarray_delete(arr);
}

jintArray jintArray_new(int length) {
    return jarray_new(sizeof(jint), length);
}

int* jintArray_get(jintArray arr) {
    return jarray_get(arr);
}

void jintArray_delete(jintArray arr) {
    jarray_delete(arr);
}

jobjectArray jobjectArray_new(int length) {
    return jarray_new(sizeof(jobject), length);
}

void jobjectArray_delete(jobjectArray arr) {
    jarray_delete(arr);
}

jclass JNICALL FindClass(JNIEnv *env, const char *name) {
    jclass retval = add_class(name, NULL);
    TRACE("JNIEnv::FindClass(%s) = %p\n", name, retval);
    return retval;
}

jobject JNICALL NewGlobalRef(JNIEnv *env, jobject lobj) {
    TRACE("JNIEnv::NewGlobalRef is called!\n");
    return lobj;
}

void JNICALL DeleteGlobalRef(JNIEnv *env, jobject gref) {
    TRACE("JNIEnv::DeleteGlobalRef is called!\n");
}

void JNICALL DeleteLocalRef(JNIEnv *env, jobject obj) {
    TRACE("JNIEnv::DeleteLocalRef is called!\n");
}

jint JNICALL EnsureLocalCapacity(JNIEnv *env, jint capacity) {
    return 0;
}

jthrowable JNICALL ExceptionOccurred(JNIEnv *env) {
    TRACE("JNIEnv::ExceptionOccurred is called!\n");
    return (jthrowable)1;
}

void JNICALL ExceptionDescribe(JNIEnv *env) {
    TRACE("JNIEnv::ExceptionDescribe is called!\n");
}

void JNICALL ExceptionClear(JNIEnv *env) {
    TRACE("JNIEnv::ExceptionClear is called!\n");
}

jint JNICALL PushLocalFrame(JNIEnv *env, jint capacity) {
    TRACE("JNIEnv::PushLocalFrame is called!\n");
    return 0;
}

jobject JNICALL PopLocalFrame(JNIEnv *env, jobject result) {
    TRACE("JNIEnv::PopLocalFrame is called!\n");
    return NULL;
}

jclass JNICALL GetObjectClass(JNIEnv *env, jobject obj) {
    if (obj == NULL) {
        return NULL;
    }
    TRACE("JNIEnv::GetObjectClass(%p) = %p(%s)\n", obj, obj->clazz, obj->clazz->base.name);
    return (jclass)obj->clazz;
}

jmethodID JNICALL GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
    if (clazz == NULL) {
        fprintf(stderr, "JNIEnv::GetStaticMethodID(%s, %s) clazz is NULL!\n", name, sig);
        return NULL;
    }
    jmethodID retval = jclass_add_method(clazz, TYPE_STATIC, name, sig, NULL);
    TRACE("JNIEnv::GetStaticMethodID(%s, %s, %s) = %p\n", clazz->name, name, sig, retval);
    return retval;
}

jmethodID JNICALL GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
    jmethodID retval = jclass_add_method(clazz, TYPE_MEMBER, name, sig, NULL);
    TRACE("JNIEnv::GetMethodID(%s, %s, %s) = %p\n", clazz->name, name, sig, retval);
    return retval;
}

jobject JNICALL NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
    jobject retval = jobject_new(clazz, "tmp_obj", NULL);
    fprintf(stderr, "JNIEnv::NewObjectV(%s, %s) = %p\n", clazz->name, methodID->name, retval);
    return retval;
}

jint JNICALL CallIntMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
    jint (*func)(jobject, va_list) = methodID->fake_func;
    if (func) {
        return func(obj->ptr, args);
    }
    fprintf(stderr, "JNIEnv::CallIntMethodV(%s)\n", methodID->name);
    return 0;
}

jobject JNICALL CallObjectMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
    jobject (*func)(jobject, va_list) = methodID->fake_func;
    if (func) {
        return func(obj->ptr, args);
    }
    fprintf(stderr, "JNIEnv::CallObjectMethodV(%s)\n", methodID->name);
    return 0;
}

jboolean JNICALL CallBooleanMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
    jboolean (*func)(jobject, va_list) = methodID->fake_func;
    if (func) {
        if (obj) {
            return func(obj->ptr, args);
        }
        else {
            return func(NULL, args);
        }
    }
    fprintf(stderr, "JNIEnv::CallBooleanMethodV(%s)\n", methodID->name);
    return 0;
}

jlong JNICALL CallLongMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
    jlong (*func)(jobject, va_list) = methodID->fake_func;
    if (func) {
        return func(obj->ptr, args);
    }
    fprintf(stderr, "JNIEnv::CallLongMethodV(%s)\n", methodID->name);
    return 0;
}

jfloat JNICALL CallFloatMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
    jfloat (*func)(jobject, va_list) = methodID->fake_func;
    if (func) {
        return func(obj->ptr, args);
    }
    fprintf(stderr, "JNIEnv::CallFloatMethodV(%s)\n", methodID->name);
    return 0.0f;
}

void JNICALL CallVoidMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
    void (*func)(jobject, va_list) = methodID->fake_func;
    if (func) {
        return func(obj->ptr, args);
    }
    fprintf(stderr, "JNIEnv::CallVoidMethodV(%s)\n", methodID->name);
}

jobject JNICALL CallStaticObjectMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
    jobject (*func)(va_list) = methodID->fake_func;
    if (func) {
        return func(args);
    }
    fprintf(stderr, "JNIEnv::CallStaticObjectMethodV(%s)\n", methodID->name);
    return 0;
}

void JNICALL CallStaticVoidMethodV(JNIEnv *env, jclass cls, jmethodID methodID, va_list args) {
    void (*func)(va_list) = methodID->fake_func;
    if (func) {
        return func(args);
    }
    fprintf(stderr, "JNIEnv::CallStaticVoidMethodV(%s)\n", methodID->name);
    return;
}

jint JNICALL CallStaticIntMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
    jint (*func)(va_list) = methodID->fake_func;
    if (func) {
        return func(args);
    }
    fprintf(stderr, "JNIEnv::CallStaticIntMethodV(%s)\n", methodID->name);
    return 0;
}

jint JNICALL GetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
    fprintf(stderr, "JNIEnv::GetStaticIntField(%s)\n", fieldID->name);
    return 0;
}

jobject JNICALL GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
    fprintf(stderr, "JNIEnv::GetStaticObjectField(%s)\n", fieldID->name);
    return NULL;
}

jstring JNICALL NewString(JNIEnv *env, const jchar *unicode, jsize len) {
    jstring retval = jstring_new((char*)unicode);
    TRACE("JNIEnv::NewString(%s, %d) = %p(%s)\n", unicode, len, retval, jstring_get(retval));
    return retval;
}

const char* JNICALL GetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy) {
    if (isCopy) {
        *isCopy = 0;
    }
    const char* retval = jstring_get(str);
    TRACE("JNIEnv::GetStringUTFChars(%p) = %s\n", str, retval);
    return retval;
}

void JNICALL ReleaseStringUTFChars(JNIEnv *env, jstring str, const char* chars) {
    TRACE("JNIEnv::ReleaseStringUTFChars(%p, %s) is called!\n", str, chars);
}

jstring JNICALL NewStringUTF(JNIEnv *env, const char *utf) {
    jstring retval = jstring_new(utf);
    TRACE("JNIEnv::NewStringUTF(%s) = %p(%s)\n", utf, retval, jstring_get(retval));
    return retval;
}

jsize JNICALL GetArrayLength(JNIEnv *env, jarray array) {
    struct _jarray* arr = (void*)array;
    if (array == NULL) {
        fprintf(stderr, "JNIEnv::GetArrayLength() array is NULL!\n");
        return 0;
    }
    TRACE("JNIEnv::GetArrayLength(%p) = %d\n", array, arr->length);
    return arr->length;
}

jbyteArray JNICALL NewByteArray(JNIEnv *env, jsize len) {
    TRACE("JNIEnv::NewByteArray(%d)\n", len);
    return jbyteArray_new(len);
}

jbyte * JNICALL GetByteArrayElements(JNIEnv *env, jbyteArray array, jboolean *isCopy) {
    if (isCopy) {
        *isCopy = 0;
    }
    if (array == NULL) {
        fprintf(stderr, "JNIEnv::GetByteArrayElements() array is NULL!\n");
        return NULL;
    }
    TRACE("JNIEnv::GetByteArrayElements(%p) = %p\n", array, array->ptr);
    return (jbyte*)array->ptr;
}

void JNICALL ReleaseByteArrayElements(JNIEnv *env, jbyteArray array, jbyte *elems, jint mode) {
    TRACE("JNIEnv::ReleaseByteArrayElements(%p, %p, %d) is called!\n", array, elems, mode);
}

jint * JNICALL GetIntArrayElements(JNIEnv *env, jintArray array, jboolean *isCopy) {
    if (isCopy) {
        *isCopy = 0;
    }
    TRACE("JNIEnv::GetIntArrayElements(%p) = %p\n", array, array->ptr);
    return (jint*)array->ptr;
}

void JNICALL ReleaseIntArrayElements(JNIEnv *env, jintArray array, jint *elems, jint mode) {
    TRACE("JNIEnv::ReleaseIntArrayElements(%p, %p, %d) is called!\n", array, elems, mode);
}

jfieldID JNICALL GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
    jfieldID retval = jclass_add_field(clazz, TYPE_STATIC, name, sig, NULL);
    TRACE("JNIEnv::GetStaticFieldID(%s, %s, %s) = %p\n", clazz->name, name, sig, retval);
    return retval;
}

jfieldID JNICALL GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
    jfieldID retval = jclass_add_field(clazz, TYPE_MEMBER, name, sig, NULL);
    TRACE("JNIEnv::GetFieldID(%s, %s, %s) = %p\n", clazz->name, name, sig, retval);
    return retval;
}

jboolean JNICALL ExceptionCheck(JNIEnv *env) {
    TRACE("JNIEnv::ExceptionCheck() is called!\n");
    return JNI_FALSE;
}

jint JNICALL RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods) {
    TRACE("JNIEnv::RegisterNatives() is called!\n");
    return 0;
}

void JNICALL SetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, const jbyte *buf) {
    TRACE("JNIEnv::SetByteArrayRegion(%p, %d, %d, %p)\n", array, start, len, buf);
    struct _jarray* arr = (struct _jarray*)array;
    if (start >= arr->length || (start + len) > arr->length) {
        fprintf(stderr, "JNIEnv::SetByteArrayRegion(%d, %d, %d): ArrayIndexOutOfBoundsException\n", arr->length, start, len);
        return;
    }
    char* ptr = jbyteArray_get(array);
    memcpy(ptr + start, buf, sizeof(jbyte) * len);
}

const struct JNINativeInterface_ jni_env_funcs = {
    .FindClass = FindClass,
    .NewGlobalRef = NewGlobalRef,
    .DeleteLocalRef = DeleteLocalRef,
    .DeleteGlobalRef = DeleteGlobalRef,
    .EnsureLocalCapacity = EnsureLocalCapacity,
    .ExceptionOccurred = ExceptionOccurred,
    .ExceptionDescribe = ExceptionDescribe,
    .ExceptionClear = ExceptionClear,
    .PushLocalFrame = PushLocalFrame,
    .PopLocalFrame = PopLocalFrame,
    .GetObjectClass = GetObjectClass,
    .GetMethodID = GetMethodID,
    .GetFieldID = GetFieldID,
    .NewObjectV = NewObjectV,
    .CallIntMethodV = CallIntMethodV,
    .CallObjectMethodV = CallObjectMethodV,
    .CallBooleanMethodV = CallBooleanMethodV,
    .CallLongMethodV = CallLongMethodV,
    .CallFloatMethodV = CallFloatMethodV,
    .CallVoidMethodV = CallVoidMethodV,
    
    .NewString = NewString,

    .NewStringUTF = NewStringUTF,
    .GetStringUTFChars = GetStringUTFChars,
    .ReleaseStringUTFChars = ReleaseStringUTFChars,
    .GetArrayLength = GetArrayLength,
    .NewByteArray = NewByteArray,
    .GetByteArrayElements = GetByteArrayElements,
    .ReleaseByteArrayElements = ReleaseByteArrayElements,
    .GetIntArrayElements = GetIntArrayElements,
    .ReleaseIntArrayElements = ReleaseIntArrayElements,

    .GetStaticMethodID = GetStaticMethodID,
    .GetStaticFieldID = GetStaticFieldID,
    .CallStaticObjectMethodV = CallStaticObjectMethodV,
    .CallStaticIntMethodV = CallStaticIntMethodV,
    .CallStaticVoidMethodV = CallStaticVoidMethodV,
    .GetStaticIntField = GetStaticIntField,
    .GetStaticObjectField = GetStaticObjectField,
    
    .ExceptionCheck = ExceptionCheck,
    .RegisterNatives = RegisterNatives,
    
    .SetByteArrayRegion = SetByteArrayRegion,
};

JNIEnv default_jni_env = &jni_env_funcs;

jint JNICALL DestroyJavaVM(JavaVM* vm) {
    TRACE("JavaVM::DestroyJavaVM is called!\n");
    return 0;
}

jint JNICALL AttachCurrentThread(JavaVM *vm, void **penv, void *args) {
    TRACE("JavaVM::AttachCurrentThread is called!\n");
    *penv = &default_jni_env;
    return JNI_OK;
}

jint JNICALL DetachCurrentThread(JavaVM *vm) {
    TRACE("JavaVM::DetachCurrentThread is called!\n");
    return 0;
}

jint JNICALL GetEnv(JavaVM *vm, void **penv, jint version) {
    TRACE("JavaVM::GetEnv is called!\n");
    *penv = &default_jni_env;
    return JNI_OK;
}

jint JNICALL AttachCurrentThreadAsDaemon(JavaVM *vm, void **penv, void *args) {
    TRACE("JavaVM::AttachCurrentThreadAsDaemon is called!\n");
    *penv = &default_jni_env;
    return JNI_OK;
}

const struct JNIInvokeInterface_ java_vm_funcs = {
    .DestroyJavaVM = DestroyJavaVM,
    .AttachCurrentThread = AttachCurrentThread,
    .DetachCurrentThread = DetachCurrentThread,
    .GetEnv = GetEnv,
    .AttachCurrentThreadAsDaemon = AttachCurrentThreadAsDaemon,
};

JavaVM default_java_vm = &java_vm_funcs;

void libjni_init() {
    HashMap_init(&mClassMap);
}

JavaVM* libjni_get_default_java_vm() {
    return &default_java_vm;
}

JNIEnv* libjni_get_default_jni_env() {
    return &default_jni_env;
}
