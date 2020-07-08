#ifndef DOS_TEST_CLIENT_SURFACE_VIEW_H_
#define DOS_TEST_CLIENT_SURFACE_VIEW_H_

#include "core/ehm/EHM.h"

// Dream Sandbox Android Test Client Surface View
// dos/src/testclient/DOSTestClient_SuirfaceView.h

// Dream OS Android Test Client Surface View (TODO: this is the entry point)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h> // for prctl( PR_SET_NAME )

// Android
#include <android/log.h>
#include <android/native_window_jni.h> // for native window JNI
#include <android/input.h>

// EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#if !defined(EGL_OPENGL_ES3_BIT_KHR)
	#define EGL_OPENGL_ES3_BIT_KHR 0x0040
#endif

// EXT_texture_border_clamp
#ifndef GL_CLAMP_TO_BORDER
	#define GL_CLAMP_TO_BORDER 0x812D
#endif

#ifndef GL_TEXTURE_BORDER_COLOR
	#define GL_TEXTURE_BORDER_COLOR 0x1004
#endif

// OVR API
// TODO: Fix this in CMAKE
#include "third_party/OCULUS/ovr_sdk_mobile_1.34.0/VrApi/Include/VrApi.h"
#include "third_party/OCULUS/ovr_sdk_mobile_1.34.0/VrApi/Include/VrApi_Helpers.h"
#include "third_party/OCULUS/ovr_sdk_mobile_1.34.0/VrApi/Include/VrApi_SystemUtils.h"
#include "third_party/OCULUS/ovr_sdk_mobile_1.34.0/VrApi/Include/VrApi_Input.h"

#if !defined(GL_EXT_multisampled_render_to_texture)
typedef void(GL_APIENTRY* PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)(
	GLenum target,
	GLsizei samples,
	GLenum internalformat,
	GLsizei width,
	GLsizei height);

typedef void(GL_APIENTRY* PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)(
	GLenum target,
	GLenum attachment,
	GLenum textarget,
	GLuint texture,
	GLint level,
	GLsizei samples);
#endif

#if !defined(GL_OVR_multiview)
/// static const int GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR       = 0x9630;
/// static const int GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR = 0x9632;
/// static const int GL_MAX_VIEWS_OVR                                      = 0x9631;
typedef void(GL_APIENTRY* PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC)(
	GLenum target,
	GLenum attachment,
	GLuint texture,
	GLint level,
	GLint baseViewIndex,
	GLsizei numViews);
#endif

#if !defined(GL_OVR_multiview_multisampled_render_to_texture)
typedef void(GL_APIENTRY* PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC)(
	GLenum target,
	GLenum attachment,
	GLuint texture,
	GLint level,
	GLsizei samples,
	GLint baseViewIndex,
	GLsizei numViews);
#endif

#define DEBUG 1
#define OVR_LOG_TAG "DOS_TestClient"

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, OVR_LOG_TAG, __VA_ARGS__)

#if DEBUG
	#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, OVR_LOG_TAG, __VA_ARGS__)
#else
	#define ALOGV(...)
#endif

static const int CPU_LEVEL = 2;
static const int GPU_LEVEL = 3;
static const int NUM_MULTI_SAMPLES = 4;

#define MULTI_THREADED 0

extern "C" {

// Activity lifecycle

JNIEXPORT jlong JNICALL Java_com_dos_testclient_GLES3JNILib_onCreate(
	JNIEnv* pJNIEnvironment, 
	jobject pJObj, 
	jobject pJObjActivity);

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onStart(
	JNIEnv* pJNIEnvironment, 
	jobject pJObj, 
	jlong handle);

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onResume(
	JNIEnv* pJNIEnvironment, 
	jobject pJObj, 
	jlong handle);

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onPause(
	JNIEnv* pJNIEnvironment, 
	jobject pJObj, 
	jlong handle);

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onStop(
	JNIEnv* pJNIEnvironment, 
	jobject pJObj, 
	jlong handle);

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onDestroy(
	JNIEnv* pJNIEnvironment, 
	jobject pJObj, 
	jlong handle);

// Surface lifecycle

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onSurfaceCreated(
	JNIEnv* pJNIEnvironment,
	jobject pJObj,
	jlong handle,
	jobject pJObjSurface);

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onSurfaceChanged(
	JNIEnv* pJNIEnvironment,
	jobject pJObj,
	jlong handle,
	jobject pJObjSurface);

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onSurfaceDestroyed(
	JNIEnv* pJNIEnvironment,
	jobject pJObj,
	jlong handle);

// Input

JNIEXPORT void JNICALL Java_com_dos_testclient_GLES3JNILib_onKeyEvent(
	JNIEnv* pJNIEnvironment,
	jobject pJObj,
	jlong handle,
	int keyCode,
	int action);

} // ! extern "C"


#endif // ! DOS_TEST_CLIENT_SURFACE_VIEW_H_