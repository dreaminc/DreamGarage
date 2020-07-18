#include "DreamTestClientSurfaceView.h"

#include <ctime>

#include "DreamTestClient.h"

#include <string>

//#if defined(__ANDROID__) 

extern "C" {

	// Activity lifecycle

	JNIEXPORT jlong JNICALL
	Java_com_dos_testclient_GLES3JNILib_onCreate(JNIEnv* pJNIEnvironment, UNUSED jobject pJObj, jobject pJObjActivity) 
	{
		DEBUG_LINEOUT("GLES3JNILib::onCreate()");


	}

	JNIEXPORT void JNICALL
	Java_com_dos_testclient_GLES3JNILib_onStart(UNUSED JNIEnv* pJNIEnvironment, UNUSED jobject pJObj, jlong handle) 
	{
		DEBUG_LINEOUT("GLES3JNILib::onStart()");
	}

	JNIEXPORT void JNICALL
	Java_com_dos_testclient_GLES3JNILib_onResume(UNUSED JNIEnv* pJNIEnvironment, UNUSED jobject pJObj, jlong handle) 
	{
		DEBUG_LINEOUT("GLES3JNILib::onResume()");
	}

	JNIEXPORT void JNICALL
	Java_com_dos_testclient_GLES3JNILib_onPause(UNUSED JNIEnv* pJNIEnvironment, UNUSED jobject pJObj, jlong handle) 
	{
		DEBUG_LINEOUT("GLES3JNILib::onPause()");
	}

	JNIEXPORT void JNICALL
	Java_com_dos_testclient_GLES3JNILib_onStop(UNUSED JNIEnv* pJNIEnvironment, UNUSED jobject pJObj, jlong handle) 
	{
		DEBUG_LINEOUT("GLES3JNILib::onStop()");
	}

	JNIEXPORT void JNICALL
	Java_com_dos_testclient_GLES3JNILib_onDestroy(JNIEnv* pJNIEnvironment, UNUSED jobject pJObj, jlong handle) 
	{
		DEBUG_LINEOUT("GLES3JNILib::onDestroy()");
	}

	// Surface lifecycle

	JNIEXPORT void JNICALL 
	Java_com_dos_testclient_GLES3JNILib_onSurfaceCreated(
		JNIEnv* pJNIEnvironment,
		UNUSED jobject pJObj,
		jlong handle,
		jobject pJObjSurface)
	{
		DEBUG_LINEOUT("GLES3JNILib::onSurfaceCreated()");
	}

	JNIEXPORT void JNICALL 
	Java_com_dos_testclient_GLES3JNILib_onSurfaceChanged(
		JNIEnv* pJNIEnvironment,
		UNUSED jobject pJObj,
		jlong handle,
		jobject pJObjSurface)
	{
		DEBUG_LINEOUT("GLES3JNILib::onSurfaceChanged()");
	}

	JNIEXPORT void JNICALL 
	Java_com_dos_testclient_GLES3JNILib_onSurfaceDestroyed(
		UNUSED JNIEnv* pJNIEnvironment,
		UNUSED jobject pJObj,
		jlong handle)
	{
		DEBUG_LINEOUT("GLES3JNILib::onSurfaceDestroyed()");
		
	}

	// Input

	JNIEXPORT void JNICALL 
	Java_com_dos_testclient_GLES3JNILib_onKeyEvent(
		UNUSED JNIEnv* pJNIEnvironment,
		UNUSED jobject pJObj,
		jlong handle,
		int keyCode,
		int action)
	{
		if (action == AKEY_EVENT_ACTION_UP) {
			DEBUG_LINEOUT("GLES3JNILib::onKeyEvent( %d, %d )", keyCode, action);
		}
	}

} // ! extern "C"

//#endif // ! ifdef __ANDROID__
