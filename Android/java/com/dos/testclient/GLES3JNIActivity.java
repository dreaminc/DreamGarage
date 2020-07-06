// Copyright (c) Facebook Technologies, LLC and its affiliates. All Rights reserved.
package com.dos.testclient;

import android.app.Activity;
import android.content.Context;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;

public class GLES3JNIActivity extends Activity implements SurfaceHolder.Callback {
  // Load the gles3jni library right away to make sure JNI_OnLoad() gets called as the very first thing.
  static {
    System.loadLibrary("dostestclient");
  }

  private static final String TAG = "Dream_TestClient";

  private SurfaceView m_surfaceView;
  private SurfaceHolder m_surfaceHolder;
  private long m_hNative;
  private GLES3JNILib m_hGLESJNILib = null;

  private void adjustVolume(int direction) {
    AudioManager audio = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
    audio.adjustStreamVolume(AudioManager.STREAM_MUSIC, direction, 0);
  }

  // Call the native code
  //public native long nativeOnCreate(Activity obj);

  @Override
  protected void onCreate(Bundle icicle) {
    Log.v(TAG, "----------------------------------------------------------------");
    Log.v(TAG, "GLES3JNIActivity::onCreate()");
    super.onCreate(icicle);

    m_surfaceView = new SurfaceView(this);
    setContentView(m_surfaceView);
    m_surfaceView.getHolder().addCallback(this);

    m_hGLESJNILib = new GLES3JNILib();

    // Force the screen to stay on, rather than letting it dim and shut off
    // while the user is watching a movie.
    getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

    m_hNative = m_hGLESJNILib.onCreate((Activity)(this));
  }

  @Override
  protected void onStart() {
    Log.v(TAG, "GLES3JNIActivity::onStart()");
    super.onStart();
    m_hGLESJNILib.onStart(m_hNative);
  }

  @Override
  protected void onResume() {
    Log.v(TAG, "GLES3JNIActivity::onResume()");
    super.onResume();

    m_hGLESJNILib.onResume(m_hNative);
  }

  @Override
  protected void onPause() {
    Log.v(TAG, "GLES3JNIActivity::onPause()");
    m_hGLESJNILib.onPause(m_hNative);
    super.onPause();
  }

  @Override
  protected void onStop() {
    Log.v(TAG, "GLES3JNIActivity::onStop()");
    m_hGLESJNILib.onStop(m_hNative);
    super.onStop();
  }

  @Override
  protected void onDestroy() {
    Log.v(TAG, "GLES3JNIActivity::onDestroy()");
    if (m_surfaceHolder != null) {
      m_hGLESJNILib.onSurfaceDestroyed(m_hNative);
    }
    m_hGLESJNILib.onDestroy(m_hNative);
    super.onDestroy();
    m_hNative = 0;
  }

  @Override
  public void surfaceCreated(SurfaceHolder holder) {
    Log.v(TAG, "GLES3JNIActivity::surfaceCreated()");
    if (m_hNative != 0) {
      m_hGLESJNILib.onSurfaceCreated(m_hNative, holder.getSurface());
      m_surfaceHolder = holder;
    }
  }

  @Override
  public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    Log.v(TAG, "GLES3JNIActivity::surfaceChanged()");
    if (m_hNative != 0) {
      m_hGLESJNILib.onSurfaceChanged(m_hNative, holder.getSurface());
      m_surfaceHolder = holder;
    }
  }

  @Override
  public void surfaceDestroyed(SurfaceHolder holder) {
    Log.v(TAG, "GLES3JNIActivity::surfaceDestroyed()");
    if (m_hNative != 0) {
      m_hGLESJNILib.onSurfaceDestroyed(m_hNative);
      m_surfaceHolder = null;
    }
  }

  @Override
  public boolean dispatchKeyEvent(KeyEvent event) {
    if (m_hNative != 0) {
      int keyCode = event.getKeyCode();
      int action = event.getAction();

      if (action != KeyEvent.ACTION_DOWN && action != KeyEvent.ACTION_UP) {
        return super.dispatchKeyEvent(event);
      }

      if (keyCode == KeyEvent.KEYCODE_VOLUME_UP) {
        adjustVolume(1);
        return true;
      }

      if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN) {
        adjustVolume(-1);
        return true;
      }

      if (action == KeyEvent.ACTION_UP) {
        Log.v(TAG, "GLES3JNIActivity::dispatchKeyEvent( " + keyCode + ", " + action + " )");
      }

      m_hGLESJNILib.onKeyEvent(m_hNative, keyCode, action);

      return true;

    }

    return false;

  }
}
