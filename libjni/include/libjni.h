#ifndef _LIB_JNI_H
#define _LIB_JNI_H

#include "jni.h"

#ifdef __cplusplus
extern "C" {
#endif

JavaVM* libjni_get_default_java_vm();
JNIEnv* libjni_get_default_jni_env();

#define TYPE_MEMBER 1
#define TYPE_STATIC 0

void libjni_init();
jclass jclass_new(const char* name, void* ptr);
jclass add_class(const char* name, void* ptr);
jmethodID jmethod_new(int type, char* full_name, void* ptr);
jmethodID jclass_add_method(jclass clz, int type, const char *name, const char *sig, void* ptr);
jfieldID jfield_new(int type, char* full_name, void* ptr);
jfieldID jclass_add_field(jclass clz, int type, const char *name, const char *sig, void* ptr);
jobject jobject_new(jclass clz, const char* name, void* ptr);
const char* jstring_get(jstring str);
jstring jstring_new(const char* utf);
jbyteArray jbyteArray_new(int length);
char* jbyteArray_get(jbyteArray arr);
void jbyteArray_delete(jbyteArray arr);
jintArray jintArray_new(int length);
int* jintArray_get(jintArray arr);
void jintArray_delete(jintArray arr);
jobjectArray jobjectArray_new(int length);
void jobjectArray_delete(jobjectArray arr);

#ifdef __cplusplus
}
#endif

#endif
