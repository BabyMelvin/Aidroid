#ifndef ANDROID_HARDWARE_MELVIN_V1_0_MELVIN_H
#define ANDROID_HARDWARE_MELVIN_V1_0_MELVIN_H

#include <android/hardware/melvin/1.0/IMelvin.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace android {
namespace hardware {
namespace melvin {
namespace V1_0 {
namespace implementation {

using ::android::hardware::melvin::V1_0::IMelvin;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct Melvin : public IMelvin {
    // Methods from ::android::hardware::melvin::V1_0::IMelvin follow.
    Return<void> helloWorld(const hidl_string& name, helloWorld_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

extern "C" IMelvin* HIDL_FETCH_IMelvin(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace melvin
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_MELVIN_V1_0_MELVIN_H
