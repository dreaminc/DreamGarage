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
// TestClientRenderer.h
// Renderer for teapots
//--------------------------------------------------------------------------------
#ifndef _MoreTeapotsRenderer_H
#define _MoreTeapotsRenderer_H

//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include <jni.h>
#include <errno.h>
#include <random>
#include <vector>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>

#define CLASS_NAME "android/app/NativeActivity"
#define APPLICATION_CLASS_NAME "com/sample/moreteapots/MoreTeapotsApplication"

#include "NDKHelper.h"

#define BUFFER_OFFSET(i) (void*)(((char*)(nullptr) + (i)))

struct TEAPOT_VERTEX {
	float pos[3];
	float normal[3];
};

enum SHADER_ATTRIBUTES {
	ATTRIB_VERTEX,
	ATTRIB_NORMAL,
	ATTRIB_COLOR,
	ATTRIB_UV
};

struct SHADER_PARAMS {
	GLuint program_;
	GLuint light0_;
	GLuint material_diffuse_;
	GLuint material_ambient_;
	GLuint material_specular_;

	GLuint matrix_projection_;
	GLuint matrix_view_;
};

struct TEAPOT_MATERIALS {
	float specular_color[4];
	float ambient_color[3];
};

class TestClientTeapotRenderer {
	int32_t m_indices_n;
	int32_t m_vertices_n;
	GLuint m_glIBO;
	GLuint m_glVBO;
	GLuint m_glUBO;

	SHADER_PARAMS m_shaderParams;

	bool LoadShaders(SHADER_PARAMS* pShaderParams, const char* pszVertexShader, const char* pszFragmentShader);
	bool LoadShadersES3(SHADER_PARAMS* pShaderParams, const char* pszVertextShader, const char* pszFragmentShader, 
		std::map<std::string, std::string>& shaderParameters);

	ndk_helper::Mat4 m_mat4Projection;
	ndk_helper::Mat4 m_mat4View;
	std::vector<ndk_helper::Mat4> m_mat4Models;
	std::vector<ndk_helper::Vec3> m_mat4ModelColors;
	std::vector<ndk_helper::Vec2> m_vec2ModelRotations;
	std::vector<ndk_helper::Vec2> m_vec2ModelCurrentRotations;

	ndk_helper::TapCamera* m_pTapCamera;

	int32_t m_teapotX;
	int32_t m_teapotY;
	int32_t m_teapotZ;
	int32_t m_UBOMatrixStride;
	int32_t m_UBOVectorStride;
	bool m_fGeometryInstancingSupported;
	bool m_fARBSupported;

	std::string ToString(const int32_t i);

public:
	TestClientTeapotRenderer();
	virtual ~TestClientTeapotRenderer();
	void Init(const int32_t numX, const int32_t numY, const int32_t numZ);
	void Render();
	void Update(float dTime);
	bool Bind(ndk_helper::TapCamera* camera);
	void Unload();
	void UpdateViewport();
};

#endif
