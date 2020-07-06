
LOCAL_PATH:= $(call my-dir)

#--------------------------------------------------------
# libdostestclient.so
#--------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE			:= dostestclient
#LOCAL_CFLAGS			:= -std=c99 -Werror
CMAKE_CXX_FLAGS			:= /Zi /JMC /W3 /WX /sdl /MP /Od /Gm- /EHsc /RTC1 /MTd /GS /fp:precise
LOCAL_SRC_FILES			:= ../../../../src/sandbox/android/DOSTestClient_SurfaceView.cpp
LOCAL_LDLIBS			:= -llog -landroid -lGLESv3 -lEGL		# include default libraries

LOCAL_SHARED_LIBRARIES	:= vrapi

include $(BUILD_SHARED_LIBRARY)

$(call import-module,Android/VrApi/Projects/AndroidPrebuilt/jni)
