package onclite

import (
    "android/soong/android"
)

func init() {
    android.RegisterModuleType("xiaomi_onclite_init_library_static", initLibraryFactory)
}
