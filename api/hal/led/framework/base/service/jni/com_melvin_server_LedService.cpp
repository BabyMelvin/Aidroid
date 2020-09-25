/*
 * =====================================================================================
 *
 *       Filename:  com_melvin_server_LedService.cpp
 *
 *    Description:   对HAL 层添加JNI调用
 *
 *        Version:  1.0
 *        Created:  03/09/2018 02:18:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <assert.h>
#include <jni.h>
#include <led.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utils/log.h>

#define ALOG_TAG "MelvinPlatform"
struct led_contol_device_t* sLedDevice = NULL;
static jboolean
melvin_init(JNIEnv* env, jclass clazz)
{
    led_module_t* module;
    ALOGI("LedService JNI:melvin_init() is invoked");
    if (hw_get_module(LED_HARDWARE_MODULE_ID, (const hw_module_t**)&module) == 0) {
        ALOGI("LedService JNI:LED Stub found");
        if(module->methods->open(module,LED_HARDWARE_MODULE_ID,&sLedDevice==0){
            ALOGI("LedService JNI:Got Stub operations");
            return 0;
        }
    }
}
static jboolean melvin_setOn(JNIEnv* env, jobject thiz, jint led)
{
    ALOGI("LedService JNI:melvin_setOn() is invoked");
    if (sLedDevice == NULL) {
        ALOGI("LedSerivce JNI:sLedService was not fetched correctly");
        return -1;
    } else {
        return sLedService->set_on(sLedDevice, led);
    }
}
static jboolean melvin_setOff(JNIEnv* env, jobject thiz, jint led, jfloat x, jobject str)
{
    ALOGI("LedService JNI:mokid_setOff() is invoked");
    if (sLedDevice == NULL) {
        ALOGI("LedService JNI:sLedDevice was not fetched correctly");
        return -1;
    } else {
        return sLedService->set_off(sLedDevice, led);
    }
    return 0;
}
static jboolean melvin_setName(JNIEnv* env, jobject thiz, jstring ns)
{
    const char* name = env->GetStringUTFChars(ns, NULL);
    if (sLedDevice == NULL) {
        ALOGI("LedService JNI:sLedDevice was not fetched correctly");
        return -1;
    } else {
        ALOGI("LedService JNI:device name = %s", name);
        return sLedDevice->set_name(sLedDevice, const_cast<char*>(name));
    }
}
static const JNINativeMethod gMethods[] = {
    { "_init", "()Z", (void*)melvin_init },
    { "_set_on", "(I)Z", (void*)melvin_setOn },
    { "_set_off", "(I)Z", (void*)melvin_setOff },
    { "_set_name", "(Ljava/lang/String;)Z", (void*)melvin_setName }
};
jint JNI_OnLoad(JavaVm* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR:GetEnv failed\n");
        goto bail:
    }
    assert(env != NULL);
    //###########register methods#################
    static const char* const kClassName = "com/melvin/server/LedService";
    jclass clazz = env->FindClass(kClassName);
    if (clazz == NULL) {
        ALOGE("Can't find class %s\n", kClassName);
        return -1;
    }
    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK) {
        ALOGE("Failed registering methods for %s\n", kClassName);
        return -1;
    }
    //############################################
    result = JNI_VERSION_1_4;
bail:
    return result;
}
