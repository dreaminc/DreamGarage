// Copyright (c) Facebook Technologies, LLC and its affiliates. All Rights reserved.
package com.dos.testclient;

import android.app.Activity;
import android.view.Surface;

// Wrapper for native library

public class GLES3JNILib {

  // Activity lifecycle
  public native long onCreate(Activity obj);

  public native void onStart(long handle);

  public native void onResume(long handle);

  public native void onPause(long handle);

  public native void onStop(long handle);

  public native void onDestroy(long handle);

  // Surface lifecycle
  public native void onSurfaceCreated(long handle, Surface s);

  public native void onSurfaceChanged(long handle, Surface s);

  public native void onSurfaceDestroyed(long handle);

  // Input
  public native void onKeyEvent(long handle, int keyCode, int action);
}
