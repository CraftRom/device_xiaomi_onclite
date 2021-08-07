/*
   Copyright (c) 2016, The CyanogenMod Project
   Copyright (C) 2019 The LineageOS Project.
   Copyright (C) 2020-2021 The MoKee Open Source Project
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <android-base/properties.h>
#include <stdlib.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <stdio.h>
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>
#include <sys/system_properties.h>
#include <vector>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;

char const *heapstartsize;
char const *heapgrowthlimit;
char const *heapsize;
char const *heapminfree;
char const *heapmaxfree;
char const *heaptargetutilization;

void property_override(char const prop[], char const value[]) {
  prop_info *pi;

  pi = (prop_info *)__system_property_find(prop);
  if (pi)
    __system_property_update(pi, value, strlen(value));
  else
    __system_property_add(prop, strlen(prop), value, strlen(value));
}

void full_property_override(const std::string &prop, const char value[], const bool product) {
    const int prop_count = 6;
    const std::vector<std::string> prop_types
        {"", "odm.", "product.", "system.", "system_ext.", "vendor."};

    for (int i = 0; i < prop_count; i++) {
        std::string prop_name = (product ? "ro.product." : "ro.") + prop_types[i] + prop;
        property_override(prop_name.c_str(), value);
    }
}

void check_device()
{
    struct sysinfo sys;

    sysinfo(&sys);

    if (sys.totalram > 3072ull * 1024 * 1024) {
        // Set for 4GB RAM
        // from - phone-xxhdpi-4096-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "256m";
        heapsize = "512m";
        heaptargetutilization = "0.6";
        heapminfree = "8m";
        heapmaxfree = "16m";
    } else if (sys.totalram > 2048ull * 1024 * 1024) {
        // Set for 3GB RAM
        // from - phone-xhdpi-2048-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "192m";
        heapsize = "512m";
        heaptargetutilization = "0.75";
        heapminfree = "512k";
        heapmaxfree = "8m";
    } else {
        // Set for 2GB RAM
        // from go_defaults_common.prop
        heapstartsize = "8m";
        heapgrowthlimit = "128m";
        heapsize = "256m";
        heaptargetutilization = "0.75";
        heapminfree = "512k";
        heapmaxfree = "8m";
    }
}

void vendor_load_properties()
{
	const char *fingerprint = "google/redfin/redfin:11/RQ3A.210805.001.A1/7474174:user/release-keys";
	const char *description = "redfin-user 11 RQ3A.210805.001.A1/7474174 release-keys";
    
	check_device();

	property_override("dalvik.vm.heapstartsize", heapstartsize);
	property_override("dalvik.vm.heapgrowthlimit", heapgrowthlimit);
	property_override("dalvik.vm.heapsize", heapsize);
	property_override("dalvik.vm.heaptargetutilization", heaptargetutilization);
	property_override("dalvik.vm.heapminfree", heapminfree);
	property_override("dalvik.vm.heapmaxfree", heapmaxfree);
    
	full_property_override("build.fingerprint", fingerprint, false);
	full_property_override("build.description", description, false);
	property_override("ro.boot.verifiedbootstate", "green");
	property_override("ro.boot.flash.locked", "1");
	
    std::string boot_cert = android::base::GetProperty("ro.boot.product.cert", "");

    if (boot_cert == "M1810F6LG" || boot_cert == "M1810F6LH" || boot_cert == "M1810F6LI"
            || boot_cert == "M1810F6LE" || boot_cert == "M1810F6LT" || boot_cert == "M1810F6LC") {
            for (int i = 0; i <= 1; i++) {
            full_property_override("model", "Redmi 7", i);
            full_property_override("device", "onclite", i);
            full_property_override("name", "onclite", i);
            }
            }
    else {
    for (int i = 0; i <= 1; i++) {
    full_property_override("model", "Redmi Y3", i);
    full_property_override("device", "onc", i);
    full_property_override("name", "onc", i);
    }
    }
}
