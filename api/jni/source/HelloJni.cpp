#include "HelloJni.h"

int i = 1;
jobject user;

JNIEXPORT void JNICALL Java_HelloJni_sayHello (JNIEnv *env, jclass cls)
{
    printf("Hello World in JNI\n");
}

JNIEXPORT jint JNICALL Java_HelloJni_getInt(JNIEnv *env, jobject thiz)
{
    return i;
}

JNIEXPORT void JNICALL Java_HelloJni_setInt(JNIEnv *env, jobject thiz, jint ja)
{
    i = ja + 3;
}

JNIEXPORT void JNICALL Java_HelloJni_setUser(JNIEnv *env, jobject thiz, jstring name)
{
    // 先找到class
    jclass userClass = env->FindClass("User");
    // 获取构造函数
    jmethodID userMethod = env->GetMethodID(userClass, "<init>", "()V");
    // 获取属性
    jfieldID mId = env->GetFieldID(userClass, "id", "J");
    jfieldID mUserName = env->GetFieldID(userClass, "userName", "Ljava/lang/String;");
    jfieldID mIsMan = env->GetFieldID(userClass, "isMan", "Z");
    jfieldID mAge = env->GetFieldID(userClass, "age", "I");

    //通过class来实例化对象object
    jobject userObject = env->NewObject(userClass, userMethod);
    //调用属性
    env->SetObjectField(userObject, mUserName, name);
    env->SetLongField(userObject, mId, 1001);
    env->SetBooleanField(userObject, mIsMan, 1);
    env->SetIntField(userObject, mAge, 21);
    user = userObject;
}

JNIEXPORT jobject JNICALL Java_HelloJni_getUser(JNIEnv *env, jobject thiz)
{
    return user;
}



