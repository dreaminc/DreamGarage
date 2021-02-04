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
// TestClientRenderer.cpp
// Render teapots
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "TestClientRenderer.h"

#include <string.h>

//--------------------------------------------------------------------------------
// Teapot model data
//--------------------------------------------------------------------------------
#include "teapot.inl"

//--------------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------------
TestClientTeapotRenderer::TestClientTeapotRenderer()
    : m_fGeometryInstancingSupported(false) {}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
TestClientTeapotRenderer::~TestClientTeapotRenderer() { Unload(); }

//--------------------------------------------------------------------------------
// Init
//--------------------------------------------------------------------------------
void TestClientTeapotRenderer::Init(const int32_t numX, const int32_t numY, const int32_t numZ) {

  if (ndk_helper::GLContext::GetInstance()->GetGLVersion() >= 3.0) {
    m_fGeometryInstancingSupported = true;
  }
  else if (ndk_helper::GLContext::GetInstance()->CheckExtension(
                 "GL_NV_draw_instanced") &&
             ndk_helper::GLContext::GetInstance()->CheckExtension(
                 "GL_NV_uniform_buffer_object"))
  {
    LOGI("Supported via extension!");
    //_bGeometryInstancingSupport = true;
    //_bARBSupport = true; //Need to patch shaders
    // Currently this has been disabled
  }

  // Settings
  glFrontFace(GL_CCW);

  // Create Index buffer
  m_indices_n = sizeof(teapotIndices) / sizeof(teapotIndices[0]);
  glGenBuffers(1, &m_glIBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapotIndices), teapotIndices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VBO
  m_vertices_n = sizeof(teapotPositions) / sizeof(teapotPositions[0]) / 3;
  int32_t stride = sizeof(TEAPOT_VERTEX);
  int32_t index = 0;
  TEAPOT_VERTEX* p = new TEAPOT_VERTEX[m_vertices_n];
  for (int32_t i = 0; i < m_vertices_n; ++i) {
    p[i].pos[0] = teapotPositions[index];
    p[i].pos[1] = teapotPositions[index + 1];
    p[i].pos[2] = teapotPositions[index + 2];

    p[i].normal[0] = teapotNormals[index];
    p[i].normal[1] = teapotNormals[index + 1];
    p[i].normal[2] = teapotNormals[index + 2];
    index += 3;
  }
  glGenBuffers(1, &m_glVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);
  glBufferData(GL_ARRAY_BUFFER, stride * m_vertices_n, p, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  delete[] p;

  // Init Projection matrices
  m_teapotX = numX;
  m_teapotY = numY;
  m_teapotZ = numZ;
  m_mat4Models.reserve(m_teapotX * m_teapotY * m_teapotZ);

  UpdateViewport();

  const float total_width = 500.f;
  float gap_x = total_width / (m_teapotX - 1);
  float gap_y = total_width / (m_teapotY - 1);
  float gap_z = total_width / (m_teapotZ - 1);
  float offset_x = -total_width / 2.f;
  float offset_y = -total_width / 2.f;
  float offset_z = -total_width / 2.f;

  for (int32_t x = 0; x < m_teapotX; ++x)
    for (int32_t y = 0; y < m_teapotY; ++y)
      for (int32_t z = 0; z < m_teapotZ; ++z) {
        m_mat4Models.push_back(ndk_helper::Mat4::Translation(
            x * gap_x + offset_x, y * gap_y + offset_y,
            z * gap_z + offset_z));
        m_mat4ModelColors.push_back(ndk_helper::Vec3(
            random() / float(RAND_MAX * 1.1), random() / float(RAND_MAX * 1.1),
            random() / float(RAND_MAX * 1.1)));

        float rotation_x = random() / float(RAND_MAX) - 0.5f;
        float rotation_y = random() / float(RAND_MAX) - 0.5f;
        m_vec2ModelRotations.push_back(ndk_helper::Vec2(rotation_x * 0.05f, rotation_y * 0.05f));
        m_vec2ModelCurrentRotations.push_back(
            ndk_helper::Vec2(rotation_x * M_PI, rotation_y * M_PI));
      }

  if (m_fGeometryInstancingSupported) {
    //
    // Create parameter dictionary for shader patch
    std::map<std::string, std::string> param;
    param[std::string("%NUM_TEAPOT%")] =
        ToString(m_teapotX * m_teapotY * m_teapotZ);
    param[std::string("%LOCATION_VERTEX%")] = ToString(ATTRIB_VERTEX);
    param[std::string("%LOCATION_NORMAL%")] = ToString(ATTRIB_NORMAL);
    if (m_fARBSupported)
      param[std::string("%ARB%")] = std::string("ARB");
    else
      param[std::string("%ARB%")] = std::string("");

    // Load shader
    bool b = LoadShadersES3(&m_shaderParams, "Shaders/VS_ShaderPlainES3.vsh",
                            "Shaders/ShaderPlainES3.fsh", param);
    if (b) {
      //
      // Create uniform buffer
      //
      GLuint bindingPoint = 1;
      GLuint blockIndex;
      blockIndex = glGetUniformBlockIndex(m_shaderParams.program_, "ParamBlock");
      glUniformBlockBinding(m_shaderParams.program_, blockIndex, bindingPoint);

      // Retrieve array stride value
      int32_t num_indices;
      glGetActiveUniformBlockiv(m_shaderParams.program_, blockIndex,
                                GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_indices);
      GLint i[num_indices];
      GLint stride[num_indices];
      glGetActiveUniformBlockiv(m_shaderParams.program_, blockIndex,
                                GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, i);
      glGetActiveUniformsiv(m_shaderParams.program_, num_indices, (GLuint*)i,
                            GL_UNIFORM_ARRAY_STRIDE, stride);

      m_UBOMatrixStride = stride[0] / sizeof(float);
      m_UBOVectorStride = stride[2] / sizeof(float);

      glGenBuffers(1, &m_glUBO);
      glBindBuffer(GL_UNIFORM_BUFFER, m_glUBO);
      glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_glUBO);

      // Store color value which wouldn't be updated every frame
      int32_t size = m_teapotX * m_teapotY * m_teapotZ *
                      (m_UBOMatrixStride + m_UBOMatrixStride +
                       m_UBOVectorStride);  // Mat4 + Mat4 + Vec3 + 1 stride
      float* pBuffer = new float[size];
      float* pColor =
          pBuffer + m_teapotX * m_teapotY * m_teapotZ * m_UBOMatrixStride * 2;
      for (int32_t i = 0; i < m_teapotX * m_teapotY * m_teapotZ; ++i) {
        memcpy(pColor, &m_mat4ModelColors[i], 3 * sizeof(float));
        pColor += m_UBOVectorStride;  // Assuming std140 layout which is 4
                                       // DWORD stride for vectors
      }

      glBufferData(GL_UNIFORM_BUFFER, size * sizeof(float), pBuffer,
                   GL_DYNAMIC_DRAW);
      delete[] pBuffer;
    } else {
      LOGI("Shader compilation failed!! Falls back to ES2.0 pass");
      // This happens some devices.
      m_fGeometryInstancingSupported = false;
      // Load shader for GLES2.0
      LoadShaders(&m_shaderParams, "Shaders/VS_ShaderPlain.vsh",
                  "Shaders/ShaderPlain.fsh");
    }
  } else {
    // Load shader for GLES2.0
    LoadShaders(&m_shaderParams, "Shaders/VS_ShaderPlain.vsh",
                "Shaders/ShaderPlain.fsh");
  }
}

void TestClientTeapotRenderer::UpdateViewport() {
  int32_t viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  const float CAM_NEAR = 5.f;
  const float CAM_FAR = 10000.f;
  if (viewport[2] < viewport[3]) {
    float aspect =
            static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    m_mat4Projection =
            ndk_helper::Mat4::Perspective(aspect, 1.0f, CAM_NEAR, CAM_FAR);
  } else {
    float aspect =
            static_cast<float>(viewport[3]) / static_cast<float>(viewport[2]);
    m_mat4Projection =
            ndk_helper::Mat4::Perspective(1.0f, aspect, CAM_NEAR, CAM_FAR);
  }
}

//--------------------------------------------------------------------------------
// Unload
//--------------------------------------------------------------------------------
void TestClientTeapotRenderer::Unload() {
  if (m_glVBO) {
    glDeleteBuffers(1, &m_glVBO);
    m_glVBO = 0;
  }
  if (m_glUBO) {
    glDeleteBuffers(1, &m_glUBO);
    m_glUBO = 0;
  }
  if (m_glIBO) {
    glDeleteBuffers(1, &m_glIBO);
    m_glIBO = 0;
  }
  if (m_shaderParams.program_) {
    glDeleteProgram(m_shaderParams.program_);
    m_shaderParams.program_ = 0;
  }
}

//--------------------------------------------------------------------------------
// Update
//--------------------------------------------------------------------------------
void TestClientTeapotRenderer::Update(float fTime) {
  const float CAM_X = 0.f;
  const float CAM_Y = 0.f;
  const float CAM_Z = 2000.f;

  m_mat4View = ndk_helper::Mat4::LookAt(ndk_helper::Vec3(CAM_X, CAM_Y, CAM_Z),
                                       ndk_helper::Vec3(0.f, 0.f, 0.f),
                                       ndk_helper::Vec3(0.f, 1.f, 0.f));

  if (m_pTapCamera) {
    m_pTapCamera->Update();
    m_mat4View = m_pTapCamera->GetTransformMatrix() * m_mat4View *
                m_pTapCamera->GetRotationMatrix();
  }
}

//--------------------------------------------------------------------------------
// Render
//--------------------------------------------------------------------------------
void TestClientTeapotRenderer::Render() {
  // Bind the VBO
  glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);

  int32_t iStride = sizeof(TEAPOT_VERTEX);
  // Pass the vertex data
  glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                        BUFFER_OFFSET(0));
  glEnableVertexAttribArray(ATTRIB_VERTEX);

  glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
                        BUFFER_OFFSET(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(ATTRIB_NORMAL);

  // Bind the IB
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);

  glUseProgram(m_shaderParams.program_);

  TEAPOT_MATERIALS material = {{1.0f, 1.0f, 1.0f, 10.f}, {0.1f, 0.1f, 0.1f}, };

  // Update uniforms
  //
  // using glUniform3fv here was troublesome..
  //
  glUniform4f(m_shaderParams.material_specular_, material.specular_color[0],
              material.specular_color[1], material.specular_color[2],
              material.specular_color[3]);
  glUniform3f(m_shaderParams.material_ambient_, material.ambient_color[0],
              material.ambient_color[1], material.ambient_color[2]);

  glUniform3f(m_shaderParams.light0_, 100.f, -200.f, -600.f);

  if (m_fGeometryInstancingSupported) {
    //
    // Geometry instancing, new feature in GLES3.0
    //

    // Update UBO
    glBindBuffer(GL_UNIFORM_BUFFER, m_glUBO);
    float* p = (float*)glMapBufferRange(
        GL_UNIFORM_BUFFER, 0, m_teapotX * m_teapotY * m_teapotZ *
                                  (m_UBOMatrixStride * 2) * sizeof(float),
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    float* mat_mvp = p;
    float* mat_mv = p + m_teapotX * m_teapotY * m_teapotZ * m_UBOMatrixStride;
    for (int32_t i = 0; i < m_teapotX * m_teapotY * m_teapotZ; ++i) {
      // Rotation
      float x, y;
      m_vec2ModelCurrentRotations[i] += m_vec2ModelRotations[i];
      m_vec2ModelCurrentRotations[i].Value(x, y);
      ndk_helper::Mat4 mat_rotation =
          ndk_helper::Mat4::RotationX(x) * ndk_helper::Mat4::RotationY(y);

      // Feed Projection and Model View matrices to the shaders
      ndk_helper::Mat4 mat_v = m_mat4View * m_mat4Models[i] * mat_rotation;
      ndk_helper::Mat4 mat_vp = m_mat4Projection * mat_v;

      memcpy(mat_mvp, mat_vp.Ptr(), sizeof(mat_v));
      mat_mvp += m_UBOMatrixStride;

      memcpy(mat_mv, mat_v.Ptr(), sizeof(mat_v));
      mat_mv += m_UBOMatrixStride;
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    // Instanced rendering
    glDrawElementsInstanced(GL_TRIANGLES, m_indices_n, GL_UNSIGNED_SHORT,
                            BUFFER_OFFSET(0),
                            m_teapotX * m_teapotY * m_teapotZ);

  } else {
    // Regular rendering pass
    for (int32_t i = 0; i < m_teapotX * m_teapotY * m_teapotZ; ++i) {
      // Set diffuse
      float x, y, z;
      m_mat4ModelColors[i].Value(x, y, z);
      glUniform4f(m_shaderParams.material_diffuse_, x, y, z, 1.f);

      // Rotation
      m_vec2ModelCurrentRotations[i] += m_vec2ModelRotations[i];
      m_vec2ModelCurrentRotations[i].Value(x, y);
      ndk_helper::Mat4 mat_rotation =
          ndk_helper::Mat4::RotationX(x) * ndk_helper::Mat4::RotationY(y);

      // Feed Projection and Model View matrices to the shaders
      ndk_helper::Mat4 mat_v = m_mat4View * m_mat4Models[i] * mat_rotation;
      ndk_helper::Mat4 mat_vp = m_mat4Projection * mat_v;
      glUniformMatrix4fv(m_shaderParams.matrix_projection_, 1, GL_FALSE,
                         mat_vp.Ptr());
      glUniformMatrix4fv(m_shaderParams.matrix_view_, 1, GL_FALSE, mat_v.Ptr());

      glDrawElements(GL_TRIANGLES, m_indices_n, GL_UNSIGNED_SHORT,
                     BUFFER_OFFSET(0));
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------------------------
// LoadShaders
//--------------------------------------------------------------------------------
bool TestClientTeapotRenderer::LoadShaders(SHADER_PARAMS* params, const char* strVsh,
                                     const char* strFsh) {
  //
  // Shader load for GLES2
  // In GLES2.0, shader attribute locations need to be explicitly specified
  // before linking
  //
  GLuint program;
  GLuint vertShader, fragShader;

  // Create shader program
  program = glCreateProgram();
  LOGI("Created Shader %d", program);

  // Create and compile vertex shader
  if (!ndk_helper::shader::CompileShader(&vertShader, GL_VERTEX_SHADER,
                                         strVsh)) {
    LOGI("Failed to compile vertex shader");
    glDeleteProgram(program);
    return false;
  }

  // Create and compile fragment shader
  if (!ndk_helper::shader::CompileShader(&fragShader, GL_FRAGMENT_SHADER,
                                         strFsh)) {
    LOGI("Failed to compile fragment shader");
    glDeleteProgram(program);
    return false;
  }

  // Attach vertex shader to program
  glAttachShader(program, vertShader);

  // Attach fragment shader to program
  glAttachShader(program, fragShader);

  // Bind attribute locations
  // this needs to be done prior to linking
  glBindAttribLocation(program, ATTRIB_VERTEX, "myVertex");
  glBindAttribLocation(program, ATTRIB_NORMAL, "myNormal");

  // Link program
  if (!ndk_helper::shader::LinkProgram(program)) {
    LOGI("Failed to link program: %d", program);

    if (vertShader) {
      glDeleteShader(vertShader);
      vertShader = 0;
    }
    if (fragShader) {
      glDeleteShader(fragShader);
      fragShader = 0;
    }
    if (program) {
      glDeleteProgram(program);
    }
    return false;
  }

  // Get uniform locations
  params->matrix_projection_ = glGetUniformLocation(program, "uPMatrix");
  params->matrix_view_ = glGetUniformLocation(program, "uMVMatrix");

  params->light0_ = glGetUniformLocation(program, "vLight0");
  params->material_diffuse_ = glGetUniformLocation(program, "vMaterialDiffuse");
  params->material_ambient_ = glGetUniformLocation(program, "vMaterialAmbient");
  params->material_specular_ =
      glGetUniformLocation(program, "vMaterialSpecular");

  // Release vertex and fragment shaders
  if (vertShader) glDeleteShader(vertShader);
  if (fragShader) glDeleteShader(fragShader);

  params->program_ = program;
  return true;
}

bool TestClientTeapotRenderer::LoadShadersES3(
    SHADER_PARAMS* params, const char* strVsh, const char* strFsh,
    std::map<std::string, std::string>& shaderParams) {
  //
  // Shader load for GLES3
  // In GLES3.0, shader attribute index can be described in a shader code
  // directly with layout() attribute
  //
  GLuint program;
  GLuint vertShader, fragShader;

  // Create shader program
  program = glCreateProgram();
  LOGI("Created Shader %d", program);

  // Create and compile vertex shader
  if (!ndk_helper::shader::CompileShader(&vertShader, GL_VERTEX_SHADER, strVsh,
                                         shaderParams)) {
    LOGI("Failed to compile vertex shader");
    glDeleteProgram(program);
    return false;
  }

  // Create and compile fragment shader
  if (!ndk_helper::shader::CompileShader(&fragShader, GL_FRAGMENT_SHADER,
                                         strFsh, shaderParams)) {
    LOGI("Failed to compile fragment shader");
    glDeleteProgram(program);
    return false;
  }

  // Attach vertex shader to program
  glAttachShader(program, vertShader);

  // Attach fragment shader to program
  glAttachShader(program, fragShader);

  // Link program
  if (!ndk_helper::shader::LinkProgram(program)) {
    LOGI("Failed to link program: %d", program);

    if (vertShader) {
      glDeleteShader(vertShader);
      vertShader = 0;
    }
    if (fragShader) {
      glDeleteShader(fragShader);
      fragShader = 0;
    }
    if (program) {
      glDeleteProgram(program);
    }

    return false;
  }

  // Get uniform locations
  params->light0_ = glGetUniformLocation(program, "vLight0");
  params->material_ambient_ = glGetUniformLocation(program, "vMaterialAmbient");
  params->material_specular_ =
      glGetUniformLocation(program, "vMaterialSpecular");

  // Release vertex and fragment shaders
  if (vertShader) glDeleteShader(vertShader);
  if (fragShader) glDeleteShader(fragShader);

  params->program_ = program;
  return true;
}

//--------------------------------------------------------------------------------
// Bind
//--------------------------------------------------------------------------------
bool TestClientTeapotRenderer::Bind(ndk_helper::TapCamera* camera) {
  m_pTapCamera = camera;
  return true;
}

//--------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------
std::string TestClientTeapotRenderer::ToString(const int32_t i) {
  char str[64];
  snprintf(str, sizeof(str), "%d", i);
  return std::string(str);
}
