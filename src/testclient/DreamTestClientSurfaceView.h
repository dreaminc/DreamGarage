#ifndef DREAM_TEST_CLIENT_SURFACE_VIEW_H_
#define DREAM_TEST_CLIENT_SURFACE_VIEW_H_

#include "core/ehm/EHM.h"

// Dream Sandbox Android Test Client Surface View
// dos/src/testclient/DreamTestClientSurfaceView.h

// Dream OS Android Test Client Surface View 

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

static const int CPU_LEVEL = 2;
static const int GPU_LEVEL = 3;
static const int NUM_MULTI_SAMPLES = 4;

typedef enum {
	MQ_WAIT_NONE, // don't wait
	MQ_WAIT_RECEIVED, // wait until the consumer thread has received the message
	MQ_WAIT_PROCESSED // wait until the consumer thread has processed the message
} ovrMQWait;

#define MAX_MESSAGE_PARMS 8
#define MAX_MESSAGES 1024

typedef struct {
	int Id;
	ovrMQWait Wait;
	long long Parms[MAX_MESSAGE_PARMS];
} ovrMessage;

// Cyclic queue with messages.
typedef struct {
	ovrMessage Messages[MAX_MESSAGES];
	volatile int Head; // dequeue at the head
	volatile int Tail; // enqueue at the tail
	ovrMQWait Wait;
	volatile bool EnabledFlag;
	volatile bool PostedFlag;
	volatile bool ReceivedFlag;
	volatile bool ProcessedFlag;
	pthread_mutex_t Mutex;
	pthread_cond_t PostedCondition;
	pthread_cond_t ReceivedCondition;
	pthread_cond_t ProcessedCondition;
} ovrMessageQueue;

typedef struct {
	JavaVM* JavaVm;
	jobject ActivityObject;
	pthread_t Thread;
	ovrMessageQueue MessageQueue;
	ANativeWindow* NativeWindow;
} ovrAppThread;


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


#endif // ! DREAM_TEST_CLIENT_SURFACE_VIEW_H_