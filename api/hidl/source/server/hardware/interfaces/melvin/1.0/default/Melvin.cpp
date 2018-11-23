#include "Melvin.h"

namespace android {
namespace hardware {
namespace melvin {
namespace V1_0 {
namespace implementation {

// Methods from ::android::hardware::melvin::V1_0::IMelvin follow.
Return<void> Melvin::helloWorld(const hidl_string& name, helloWorld_cb _hidl_cb) {
    char buf[100];
    ::memset(buf,0x00,100);
    ::snprintf(buf,100,"Hello world,%s",name.c_str());
    
    hidl_string(result);
    _hidl_cb(result);
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

IMelvin* HIDL_FETCH_IMelvin(const char* /* name */) {
    return new Melvin();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace melvin
}  // namespace hardware
}  // namespace android
