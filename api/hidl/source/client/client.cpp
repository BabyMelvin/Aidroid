#include <android/hardware/melvin/1.0/IMelvin.h>
#include <hidl/Status.h>
#include <hidl/LegacySupport.h>
#include <utils/misc.h>
#include <hidl/HidlSupport.h>
#include <stdio.h>

using android::hardware::melvin::V1_0::IMelvin;
using android::sp;
using android::hardware::hidl_string;

int main(){
    int ret;
    android::sp<IMelvin> service = IMelvin::getService();
    if(service == nullptr){
        printf("Failed to get service\n");
        return -1;
    }

    service->helloWorld("MelvinCao",[&](hidl_string result){
        printf("%s\n",reuslt,c_str());
    });
}
