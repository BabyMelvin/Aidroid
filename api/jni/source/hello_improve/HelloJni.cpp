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

//TODO list
JNIEXPORT void JNICALL Java_HelloJni_setUserList (JNIEnv *env, jobject thiz, jobject userList)
{
    int index;
    // class ArrayList
    jclass clsArrayList = env->GetObjectClass(userList);

    // method in class ArrayList
    jmethodID arrayListGet = env->GetMethodID(clsArrayList, "get", "(I)Ljava/lang/Object;");
    jmethodID arrayListSize = env->GetMethodID(clsArrayList, "size", "()I");
    jint len = env->CallIntMethod(userList, arrayListSize);
    printf("get java ArrayList<User> object by C++, then print it ...\n");
    for (index = 0; index < len; index ++) {
        jobject objUser = env->CallObjectMethod(userList, arrayListGet, index);
        jclass clsUser = env->GetObjectClass(objUser);
        jmethodID userGetId = env->GetMethodID(clsUser, "getId", "()J");
        jmethodID userGetAge = env->GetMethodID(clsUser, "getAge", "()I");
        jmethodID userGetUserName = env->GetMethodID(clsUser, "getUserName", "()Ljava/lang/String;");
        jmethodID userIsMan = env->GetMethodID(clsUser, "isMan", "()Z");
        jstring name = (jstring)env->CallObjectMethod(objUser, userGetUserName);
        jboolean b = true;
        const char *namePtr = env->GetStringUTFChars(name, &b);
        jlong id = env->CallLongMethod(objUser, userGetId);
        jboolean sex = env->CallBooleanMethod(objUser, userIsMan);
        jint age = env->CallIntMethod(objUser, userGetAge);
        printf("id:%d, name:%s, isMan?%d, Age:%d\n", id, namePtr, sex, age);
        env->ReleaseStringUTFChars(name, namePtr);
    }
}

JNIEXPORT jobject JNICALL Java_HelloJni_getUserList (JNIEnv *env, jobject jthiz)
{
    int index;
    // ArrayList object
    jclass clsArrayList = env->FindClass("java/util/ArrayList");
    jmethodID construct = env->GetMethodID(clsArrayList, "<init>", "()V");
    jobject objArrayList = env->NewObject(clsArrayList, construct);

    jmethodID arrayListAdd = env->GetMethodID(clsArrayList, "add", "(Ljava/lang/Object;)Z");

    //User Object
    jclass clsUser = env->FindClass("User");
    //none argument construct function
    jmethodID construtUser = env->GetMethodID(clsUser, "<init>", "()V");
    //new Object
    //jobject objUser = env->NewObject(clsUser, construtUser, "");
    //jmethodID userSetId = env->GetMethodID(clsUser, "setId", "(J)V");
    //jmethodID userSetUserName = env->GetMethodID(clsUser, "setUserName", "(Ljava/lang/String;)V");
    //jmethodID userSetAge = env->GetMethodID(clsUser, "setAge", "(I)V");
    for (index = 0; index < 10; index++) {
        // new Object
        jobject objUser = env->NewObject(clsUser, construtUser);
        // get filed id
        jfieldID userId = env->GetFieldID(clsUser, "id", "J");
        jfieldID userName = env->GetFieldID(clsUser, "userName", "Ljava/lang/String;");
        jfieldID userIsMan = env->GetFieldID(clsUser, "isMan", "Z");
        jfieldID userAge = env->GetFieldID(clsUser, "age", "I");
        env->SetLongField(objUser, userId, index);
        env->SetObjectField(objUser, userName, env->NewStringUTF("melvin cao"));
        env->SetBooleanField(objUser, userIsMan, 1);
        env->SetIntField(objUser, userAge, 21);
        env->CallObjectMethod(objArrayList, arrayListAdd, objUser);
    }

    return objArrayList;
}


