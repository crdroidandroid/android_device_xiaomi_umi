/*
 * Copyright (C) 2019-2020 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "FingerprintInscreenService"

#include "FingerprintInscreen.h"

#include <android-base/logging.h>
#include <fstream>
#include <cmath>
#include <thread>

#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>

#define FINGERPRINT_ACQUIRED_VENDOR 6

#define COMMAND_NIT 10
#define PARAM_NIT_FOD 1
#define PARAM_NIT_NONE 0

#define DISPPARAM_PATH "/sys/devices/platform/soc/ae00000.qcom,mdss_mdp/drm/card0/card0-DSI-1/disp_param"
#define DISPPARAM_FOD_HBM_OFF "0xE0000"

#define TOUCH_FOD_ENABLE 10

#define FOD_UI_PATH "/sys/devices/platform/soc/soc:qcom,dsi-display-primary/fod_ui"

#define FOD_SENSOR_X 439
#define FOD_SENSOR_Y 1806
#define FOD_SENSOR_SIZE 197

namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_0 {
namespace implementation {

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

static bool readBool(int fd) {
    char c;
    int rc;

    rc = lseek(fd, 0, SEEK_SET);
    if (rc) {
        LOG(ERROR) << "failed to seek fd, err: " << rc;
        return false;
    }

    rc = read(fd, &c, sizeof(char));
    if (rc != 1) {
        LOG(ERROR) << "failed to read bool from fd, err: " << rc;
        return false;
    }

    return c != '0';
}

FingerprintInscreen::FingerprintInscreen() {
    xiaomiTouchFeatureService = ITouchFeature::getService();
    xiaomiFingerprintService = IXiaomiFingerprint::getService();

        std::thread([this]() {
        int fd = open(FOD_UI_PATH, O_RDONLY);
        if (fd < 0) {
            LOG(ERROR) << "failed to open fd, err: " << fd;
            return;
        }

        struct pollfd fodUiPoll = {
            .fd = fd,
            .events = POLLERR | POLLPRI,
            .revents = 0,
        };

        while (true) {
            int rc = poll(&fodUiPoll, 1, -1);
            if (rc < 0) {
                LOG(ERROR) << "failed to poll fd, err: " << rc;
                continue;
            }

            xiaomiFingerprintService->extCmd(COMMAND_NIT,
                    readBool(fd) ? PARAM_NIT_FOD : PARAM_NIT_NONE);
        }
    }).detach();
}

Return<int32_t> FingerprintInscreen::getPositionX() {
    return FOD_SENSOR_X;
}

Return<int32_t> FingerprintInscreen::getPositionY() {
    return FOD_SENSOR_Y;
}

Return<int32_t> FingerprintInscreen::getSize() {
    return FOD_SENSOR_SIZE;
}

Return<void> FingerprintInscreen::onStartEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onFinishEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onPress() {
    xiaomiTouchFeatureService->setTouchMode(TOUCH_FOD_ENABLE, 1);
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    set(DISPPARAM_PATH, DISPPARAM_FOD_HBM_OFF);
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t acquiredInfo, int32_t vendorCode) {
    std::lock_guard<std::mutex> _lock(mCallbackLock);
    if (mCallback == nullptr) {
        return false;
    }

    if (acquiredInfo == FINGERPRINT_ACQUIRED_VENDOR) {
        if (vendorCode == 0) {
            Return<void> ret = mCallback->onFingerDown();
            if (!ret.isOk()) {
                LOG(ERROR) << "FingerDown() error: " << ret.description();
            }
            return true;
        }

        if (vendorCode == 1) {
            Return<void> ret = mCallback->onFingerUp();
            if (!ret.isOk()) {
                LOG(ERROR) << "FingerUp() error: " << ret.description();
            }
            return true;
        }
    }

    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t error, int32_t vendorCode) {
    LOG(ERROR) << "error: " << error << ", vendorCode: " << vendorCode;
    return false;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t /* brightness */) {
    return 0;
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}

Return<void> FingerprintInscreen::setCallback(const sp<IFingerprintInscreenCallback>& callback) {
    {
        std::lock_guard<std::mutex> _lock(mCallbackLock);
        mCallback = callback;
    }

    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor
