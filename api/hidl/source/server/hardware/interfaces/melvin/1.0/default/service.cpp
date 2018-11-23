#define LOG_TAG "android.hardware.melvin@1.0-service"

//TODO 这里生成的文件 Android.bp中指定1
#include <android/hardware/melvin/1.0/IMelvin.h>
#include <hidl/LegacySupport.h>

using android::hardware::melvin::V1_0::IMelvin;
using android::hardware::defaultPassthroughServiceImplementation;

//TODO 不传入参数
int main(){
    return defaultPassthroughServiceImplementation<IMelvin>();
}
