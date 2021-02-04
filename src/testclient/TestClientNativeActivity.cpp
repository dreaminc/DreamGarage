/*
 * Copyright 2013 The Android Open Source Project
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

//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include <jni.h>
#include <errno.h>

#include <vector>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>

#include "TestClientTeapotRenderer.h"

#include "core/ehm/EHM.h"

//-------------------------------------------------------------------------
// Preprocessor
//-------------------------------------------------------------------------

#define HELPER_CLASS_NAME "com/sample/helper/NDKHelper"  // Class name of helper function

//-------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------
const int32_t NUM_TEAPOTS_X = 8;
const int32_t NUM_TEAPOTS_Y = 8;
const int32_t NUM_TEAPOTS_Z = 8;

//-------------------------------------------------------------------------
// Shared state for our app.
//-------------------------------------------------------------------------
struct android_app;

class TestNativeActivityTeapotEngine {
  TestClientTeapotRenderer m_testClientTeapotRenderer;

  ndk_helper::GLContext* gl_context_;

  bool m_fRescourcesInitialized;
  bool m_fHasFocus;

  ndk_helper::DoubletapDetector doubletap_detector_;
  ndk_helper::PinchDetector pinch_detector_;
  ndk_helper::DragDetector drag_detector_;
  ndk_helper::PerfMonitor monitor_;

  ndk_helper::TapCamera tap_camera_;

  android_app* m_pAndroidAppState;

  ASensorManager* pAndroidSensorManager = nullptr;
  const ASensor* pAndroidAccelerometerSensor = nullptr;
  ASensorEventQueue* pAndroidSensorEventQueue = nullptr;

  void UpdateFPS(float fps);
  void ShowUI();
  void TransformPosition(ndk_helper::Vec2& vec);

 public:
  static void HandleCmd(struct android_app* app, int32_t cmd);
  static int32_t HandleInput(android_app* app, AInputEvent* event);

  TestNativeActivityTeapotEngine();
  ~TestNativeActivityTeapotEngine();
  void SetState(android_app* app);
  int InitDisplay(android_app* app);
  void LoadResources();
  void UnloadResources();
  void DrawFrame();
  void TermDisplay();
  void TrimMemory();
  bool IsReady();

  void UpdatePosition(AInputEvent* event, int32_t index, float& x, float& y);

  void InitSensors();
  void ProcessSensors(int32_t id);
  void SuspendSensors();
  void ResumeSensors();
};

//-------------------------------------------------------------------------
// Ctor
//-------------------------------------------------------------------------
TestNativeActivityTeapotEngine::TestNativeActivityTeapotEngine()
    : m_fRescourcesInitialized(false),
      m_fHasFocus(false),
      m_pAndroidAppState(NULL),
      pAndroidSensorManager(NULL),
      pAndroidAccelerometerSensor(NULL),
      pAndroidSensorEventQueue(NULL) {
  gl_context_ = ndk_helper::GLContext::GetInstance();
}

//-------------------------------------------------------------------------
// Dtor
//-------------------------------------------------------------------------
TestNativeActivityTeapotEngine::~TestNativeActivityTeapotEngine() {}

/**
 * Load resources
 */
void TestNativeActivityTeapotEngine::LoadResources() {
  m_testClientTeapotRenderer.Init(NUM_TEAPOTS_X, NUM_TEAPOTS_Y, NUM_TEAPOTS_Z);
  m_testClientTeapotRenderer.Bind(&tap_camera_);
}

/**
 * Unload resources
 */
void TestNativeActivityTeapotEngine::UnloadResources() { m_testClientTeapotRenderer.Unload(); }

/**
 * Initialize an EGL context for the current display.
 */
int TestNativeActivityTeapotEngine::InitDisplay(android_app *app) {
  if (!m_fRescourcesInitialized) {
    gl_context_->Init(m_pAndroidAppState->window);
    LoadResources();
    m_fRescourcesInitialized = true;
  } 
  else if(app->window != gl_context_->GetANativeWindow()) {
    // Re-initialize ANativeWindow.
    // On some devices, ANativeWindow is re-created when the app is resumed
    assert(gl_context_->GetANativeWindow());
    UnloadResources();
    gl_context_->Invalidate();
    m_pAndroidAppState = app;
    gl_context_->Init(app->window);
    LoadResources();
    m_fRescourcesInitialized = true;
  } 
  else {
    // initialize OpenGL ES and EGL
    if (EGL_SUCCESS == gl_context_->Resume(m_pAndroidAppState->window)) {
      UnloadResources();
      LoadResources();
    } 
    else {
      assert(0);
    }
  }

  ShowUI();

  // Initialize GL state.
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Note that screen size might have been changed
  glViewport(0, 0, gl_context_->GetScreenWidth(), gl_context_->GetScreenHeight());
  m_testClientTeapotRenderer.UpdateViewport();

  tap_camera_.SetFlip(1.f, -1.f, -1.f);
  tap_camera_.SetPinchTransformFactor(10.f, 10.f, 8.f);

  return 0;
}

/**
 * Just the current frame in the display.
 */
void TestNativeActivityTeapotEngine::DrawFrame() {
  float fps;

  if (monitor_.Update(fps)) {
    UpdateFPS(fps);
  }
  
  double dTime = monitor_.GetCurrentTime();
  m_testClientTeapotRenderer.Update(dTime);

  // Just fill the screen with a color.
  glClearColor(0.5f, 0.5f, 0.5f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_testClientTeapotRenderer.Render();

  // Swap
  if (EGL_SUCCESS != gl_context_->Swap()) {
    UnloadResources();
    LoadResources();
  }
}


// Tear down the EGL context currently associated with the display.
void TestNativeActivityTeapotEngine::TermDisplay() { gl_context_->Suspend(); }

void TestNativeActivityTeapotEngine::TrimMemory() {
  LOGI("Trimming memory");
  gl_context_->Invalidate();
}

// Process the next input event.
int32_t TestNativeActivityTeapotEngine::HandleInput(android_app* app, AInputEvent* event) {
  TestNativeActivityTeapotEngine* eng = (TestNativeActivityTeapotEngine*)app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    ndk_helper::GESTURE_STATE doubleTapState =
        eng->doubletap_detector_.Detect(event);
    ndk_helper::GESTURE_STATE dragState = eng->drag_detector_.Detect(event);
    ndk_helper::GESTURE_STATE pinchState = eng->pinch_detector_.Detect(event);

    // Double tap detector has a priority over other detectors
    if (doubleTapState == ndk_helper::GESTURE_STATE_ACTION) {
      // Detect double tap
      eng->tap_camera_.Reset(true);
    } 
    else {
      // Handle drag state
      if (dragState & ndk_helper::GESTURE_STATE_START) {
        // Otherwise, start dragging
        ndk_helper::Vec2 v;
        eng->drag_detector_.GetPointer(v);
        eng->TransformPosition(v);
        eng->tap_camera_.BeginDrag(v);
      } 
      else if (dragState & ndk_helper::GESTURE_STATE_MOVE) {
        ndk_helper::Vec2 v;
        eng->drag_detector_.GetPointer(v);
        eng->TransformPosition(v);
        eng->tap_camera_.Drag(v);
      } 
      else if (dragState & ndk_helper::GESTURE_STATE_END) {
        eng->tap_camera_.EndDrag();
      }

      // Handle pinch state
      if (pinchState & ndk_helper::GESTURE_STATE_START) {
        // Start new pinch
        ndk_helper::Vec2 v1;
        ndk_helper::Vec2 v2;
        eng->pinch_detector_.GetPointers(v1, v2);
        eng->TransformPosition(v1);
        eng->TransformPosition(v2);
        eng->tap_camera_.BeginPinch(v1, v2);
      } 
      else if (pinchState & ndk_helper::GESTURE_STATE_MOVE) {
        // Multi touch
        // Start new pinch
        ndk_helper::Vec2 v1;
        ndk_helper::Vec2 v2;
        eng->pinch_detector_.GetPointers(v1, v2);
        eng->TransformPosition(v1);
        eng->TransformPosition(v2);
        eng->tap_camera_.Pinch(v1, v2);
      }
    }
    return 1;

  }

  return 0;
}

// Process the next main command.
void TestNativeActivityTeapotEngine::HandleCmd(struct android_app* app, int32_t cmd) {

  TestNativeActivityTeapotEngine* eng = (TestNativeActivityTeapotEngine*)app->userData;
  
  switch (cmd) {
    case APP_CMD_SAVE_STATE: {
        // 
    } break;

    case APP_CMD_INIT_WINDOW: {
        // The window is being shown, get it ready.
        if (app->window != NULL) {
            eng->InitDisplay(app);
            eng->m_fHasFocus = true;
            eng->DrawFrame();
        }
    } break;

    case APP_CMD_TERM_WINDOW: {
        // The window is being hidden or closed, clean it up.
        eng->TermDisplay();
        eng->m_fHasFocus = false;
    } break;

    case APP_CMD_STOP: {
        //
    } break;

    case APP_CMD_GAINED_FOCUS: {
        eng->ResumeSensors();
        // Start animation
        eng->m_fHasFocus = true;
    } break;

    case APP_CMD_LOST_FOCUS: {
        eng->SuspendSensors();
        // Also stop animating.
        eng->m_fHasFocus = false;
        eng->DrawFrame();
    } break;

    case APP_CMD_LOW_MEMORY: {
        // Free up GL resources
        eng->TrimMemory();
    } break;
  }
}

//-------------------------------------------------------------------------
// Sensor handlers
//-------------------------------------------------------------------------
void TestNativeActivityTeapotEngine::InitSensors() {
  pAndroidSensorManager = ndk_helper::AcquireASensorManagerInstance(m_pAndroidAppState);

  pAndroidAccelerometerSensor = ASensorManager_getDefaultSensor(pAndroidSensorManager, ASENSOR_TYPE_ACCELEROMETER);
  pAndroidSensorEventQueue = ASensorManager_createEventQueue(pAndroidSensorManager, m_pAndroidAppState->looper, LOOPER_ID_USER, nullptr, nullptr);
}

void TestNativeActivityTeapotEngine::ProcessSensors(int32_t id) {
  
    // If a sensor has data, process it now.

  if (id == LOOPER_ID_USER) {
    if (pAndroidAccelerometerSensor != nullptr) {
      ASensorEvent event;
      while (ASensorEventQueue_getEvents(pAndroidSensorEventQueue, &event, 1) > 0) {
          /* stub */
      }
    }
  }
}

void TestNativeActivityTeapotEngine::ResumeSensors() {
  
    // When our app gains focus, we start monitoring the accelerometer.

    if (pAndroidAccelerometerSensor != nullptr) {
        ASensorEventQueue_enableSensor(pAndroidSensorEventQueue, pAndroidAccelerometerSensor);

        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(pAndroidSensorEventQueue, pAndroidAccelerometerSensor, (1000L / 60) * 1000);
    }
}

void TestNativeActivityTeapotEngine::SuspendSensors() {
  
    // When our app loses focus, we stop monitoring the accelerometer.
  // This is to avoid consuming battery while not being used.

  if (pAndroidAccelerometerSensor != nullptr) {
    ASensorEventQueue_disableSensor(pAndroidSensorEventQueue, pAndroidAccelerometerSensor);
  }
}

// Misc
void TestNativeActivityTeapotEngine::SetState(android_app* pAndroidAppState) {
  
    m_pAndroidAppState = pAndroidAppState;

  doubletap_detector_.SetConfiguration(m_pAndroidAppState->config);
  drag_detector_.SetConfiguration(m_pAndroidAppState->config);
  pinch_detector_.SetConfiguration(m_pAndroidAppState->config);
}

bool TestNativeActivityTeapotEngine::IsReady() {

  if (m_fHasFocus) 
      return true;

  return false;
}

void TestNativeActivityTeapotEngine::TransformPosition(ndk_helper::Vec2& vec) {
  vec = ndk_helper::Vec2(2.0f, 2.0f) * vec /
            ndk_helper::Vec2(gl_context_->GetScreenWidth(),
                             gl_context_->GetScreenHeight()) - ndk_helper::Vec2(1.f, 1.f);
}

void TestNativeActivityTeapotEngine::ShowUI() {
  JNIEnv* pJNIEnvironment = nullptr;

  m_pAndroidAppState->activity->vm->AttachCurrentThread(&pJNIEnvironment, nullptr);

  // Default class retrieval
  jclass clazz = pJNIEnvironment->GetObjectClass(m_pAndroidAppState->activity->clazz);
  jmethodID methodID = pJNIEnvironment->GetMethodID(clazz, "showUI", "()V");
  pJNIEnvironment->CallVoidMethod(m_pAndroidAppState->activity->clazz, methodID);

  m_pAndroidAppState->activity->vm->DetachCurrentThread();

  return;
}

void TestNativeActivityTeapotEngine::UpdateFPS(float fps) {
  JNIEnv* pJNIEnvironment = nullptr;

  m_pAndroidAppState->activity->vm->AttachCurrentThread(&pJNIEnvironment, nullptr);

  // Default class retrieval
  jclass clazz = pJNIEnvironment->GetObjectClass(m_pAndroidAppState->activity->clazz);
  jmethodID methodID = pJNIEnvironment->GetMethodID(clazz, "updateFPS", "(F)V");
  pJNIEnvironment->CallVoidMethod(m_pAndroidAppState->activity->clazz, methodID, fps);

  m_pAndroidAppState->activity->vm->DetachCurrentThread();

  return;
}

#include "DreamTestClient.h"

TestNativeActivityTeapotEngine g_TestNativeActivityTeapotEngine;
DreamTestClient g_DreamTestClient;

// This is the main entry point of a native application that is using
// android_native_app_glue.  It runs in its own thread, with its own
// event loop for receiving input events and doing other things.
void android_main(android_app* pAndroidAppState) {
    RESULT r = R_PASS;

    DEBUG_LINEOUT("TestClient:android_main starting ... ");

    g_TestNativeActivityTeapotEngine.SetState(pAndroidAppState);

    // Init helper functions
    ndk_helper::JNIHelper::GetInstance()->Init(pAndroidAppState->activity, HELPER_CLASS_NAME);

    pAndroidAppState->userData = &g_TestNativeActivityTeapotEngine;
    pAndroidAppState->onAppCmd = TestNativeActivityTeapotEngine::HandleCmd;
    pAndroidAppState->onInputEvent = TestNativeActivityTeapotEngine::HandleInput;

    #ifdef USE_NDK_PROFILER
        monstartup("libMoreTeapotsNativeActivity.so");
    #endif

    // Prepare to monitor accelerometer
    g_TestNativeActivityTeapotEngine.InitSensors();

    // Try out sticking the dream test client in here
    g_DreamTestClient = DreamTestClient();
    //CRM(g_DreamTestClient.Initialize(argc, (const char**)argv), "Failed to initialize Dream Garage");
    CRM(g_DreamTestClient.Initialize(0, nullptr), "Failed to initialize Dream Garage");
    CRM(g_DreamTestClient.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine

    // loop waiting for stuff to do.
    while (true) {

        // Read all pending events.
        int id;
        int events;
        android_poll_source* pAndoridPollSource = nullptr;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((id = ALooper_pollAll(g_TestNativeActivityTeapotEngine.IsReady() ? 0 : -1, 
			   nullptr,
			   &events,
			   (void**)&pAndoridPollSource)) >= 0)
        {
        // Process this event.
        if (pAndoridPollSource != nullptr) {
            pAndoridPollSource->process(pAndroidAppState, pAndoridPollSource);
        }

        g_TestNativeActivityTeapotEngine.ProcessSensors(id);

        // Check if we are exiting.
        if (pAndroidAppState->destroyRequested != 0) {
            g_TestNativeActivityTeapotEngine.TermDisplay();
            return;
        }
        }

        if (g_TestNativeActivityTeapotEngine.IsReady()) {
            // Drawing is throttled to the screen update rate, so there is no need to do timing here.
            g_TestNativeActivityTeapotEngine.DrawFrame();
        }
    }

Success:
    DreamLogger::instance()->Flush();
    return;// (int)(r);

Error:
    DEBUG_LINEOUT("DREAM OS Exiting with Error 0x%x result", r);
    DreamLogger::instance()->Flush();
    DEBUG_SYSTEM_PAUSE();

    return;// (int)(r);
}
