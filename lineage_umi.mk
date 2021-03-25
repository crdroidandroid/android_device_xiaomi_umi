#
# Copyright (C) 2020 Project 404
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Inherit device configuration
$(call inherit-product, device/xiaomi/umi/kona.mk)

# Inherit some common LineageOS stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit GMS
$(call inherit-product, vendor/google/gms/config.mk)

# Boot animation
TARGET_SCREEN_HEIGHT := 2400
TARGET_SCREEN_WIDTH := 1080
TARGET_BOOT_ANIMATION_RES := 1080

TARGET_BUILD_WITH_GAPPS := true

# crDroid Specific
TARGET_HAS_FOD := true
EXTRA_FOD_ANIMATIONS := true

# Override build properties
PRODUCT_NAME := lineage_umi
PRODUCT_DEVICE := umi
PRODUCT_BRAND := Xiaomi
PRODUCT_MODEL := Mi 10
PRODUCT_MANUFACTURER := Xiaomi

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DISC="redfin-user 11 RQ2A.210305.006 7119741 release-keys"

BUILD_FINGERPRINT := "google/redfin/redfin:11/RQ2A.210305.006/7119741:user/release-keys"

PRODUCT_PROPERTY_OVERRIDES += \
    ro.build.fingerprint=$(BUILD_FINGERPRINT)

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi
