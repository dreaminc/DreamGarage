#ifndef OPEN_GL_UTILITY_H_
#define OPEN_GL_UTILITY_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/OpenGLUtility.h
// OpenGL Utility functions
// Generally state-less so should be static

#include "OpenGLCommon.h"

class OpenGLUtility {

public:
	static const char *GetOGLTypeString(GLushort GLType) {
		switch (GLType) {
			case GL_BYTE:										return "GL Byte"; break;
			case GL_UNSIGNED_BYTE:								return "GL Unsigned Byte"; break;
			case GL_SHORT:										return "GL Short"; break;
			case GL_UNSIGNED_SHORT:								return "GL Unsigned Short"; break;
			case GL_INT:										return "GL Int"; break;
			case GL_UNSIGNED_INT:								return "GL Unsigned Int"; break;
			case GL_FLOAT:										return "GL Float"; break;
			case GL_2_BYTES:									return "GL 2 Bytes"; break;
			case GL_3_BYTES:									return "GL 3 Bytes"; break;
			case GL_4_BYTES:									return "GL 4 Bytes"; break;
			case GL_DOUBLE:										return "GL Double"; break;
			case GL_FLOAT_VEC3:									return "GL Float Vec3"; break;
			case GL_FLOAT_VEC4:									return "GL Float Vec4"; break;
			case GL_FLOAT_VEC2:									return "GL Float Vec2"; break;
			case GL_DOUBLE_VEC2:								return "GL Double Vec2"; break;
			case GL_DOUBLE_VEC3:								return "GL Double Vec3"; break;
			case GL_DOUBLE_VEC4:								return "GL Double Vec4"; break;
			case GL_INT_VEC2:									return "GL Int Vec2"; break;
			case GL_INT_VEC3:									return "GL Int Vec3"; break;
			case GL_INT_VEC4:									return "GL Int Vec4"; break;
			case GL_UNSIGNED_INT_VEC2:							return "GL Unsigned Int Vec2"; break;
			case GL_UNSIGNED_INT_VEC3:							return "GL Unsigned Int Vec3"; break;
			case GL_UNSIGNED_INT_VEC4:							return "GL Unsigned Int Vec4"; break;
			case GL_BOOL:										return "GL Boolean"; break;
			case GL_BOOL_VEC2:									return "GL Boolean Vec2"; break;
			case GL_BOOL_VEC3:									return "GL Boolean Vec3"; break;
			case GL_BOOL_VEC4:									return "GL Boolean Vec4"; break;
			case GL_FLOAT_MAT2:									return "GL Float Matrix 2"; break;
			case GL_FLOAT_MAT3:									return "GL Float Matrix 3"; break;
			case GL_FLOAT_MAT4:									return "GL Float Matrix 4"; break;
			case GL_FLOAT_MAT2x3:								return "GL Float Matrix 2x3"; break;
			case GL_FLOAT_MAT2x4:								return "GL Float Matrix 2x4"; break;
			case GL_FLOAT_MAT3x2:								return "GL Float Matrix 3x2"; break;
			case GL_FLOAT_MAT3x4:								return "GL Float Matrix 3x4"; break;
			case GL_FLOAT_MAT4x2:								return "GL Float Matrix 4x2"; break;
			case GL_FLOAT_MAT4x3:								return "GL Float Matrix 4x3"; break;
			case GL_DOUBLE_MAT2:								return "GL Double Matrix 2"; break;
			case GL_DOUBLE_MAT3:								return "GL Double Matrix 3"; break;
			case GL_DOUBLE_MAT4:								return "GL Double Matrix 4"; break;
			case GL_DOUBLE_MAT2x3:								return "GL Double Matrix 2x3"; break;
			case GL_DOUBLE_MAT2x4:								return "GL Double Matrix 2x4"; break;
			case GL_DOUBLE_MAT3x2:								return "GL Double Matrix 3x2"; break;
			case GL_DOUBLE_MAT3x4:								return "GL Double Matrix 3x4"; break;
			case GL_DOUBLE_MAT4x2:								return "GL Double Matrix 4x2"; break;
			case GL_DOUBLE_MAT4x3:								return "GL Double Matrix 4x3"; break;
			case GL_SAMPLER_1D:									return "GL Sampler 1D"; break;
			case GL_SAMPLER_2D:									return "GL Sampler 2D"; break;
			case GL_SAMPLER_3D:									return "GL Sampler 3D"; break;
			case GL_SAMPLER_CUBE:								return "GL Sampler Cube"; break;
			case GL_SAMPLER_1D_SHADOW:							return "GL Sampler 1D Shadow"; break;
			case GL_SAMPLER_2D_SHADOW:							return "GL Sampler 2D Shadow"; break;
			case GL_SAMPLER_1D_ARRAY:							return "GL Sampler 1D Array"; break;
			case GL_SAMPLER_2D_ARRAY:							return "GL Sampler 2D Array"; break;
			case GL_SAMPLER_1D_ARRAY_SHADOW:					return "GL Sampler 1D Array Shadow"; break;
			case GL_SAMPLER_2D_ARRAY_SHADOW:					return "GL Sampler 2D Array Shadow"; break;
			case GL_SAMPLER_2D_MULTISAMPLE:						return "GL Sampler 2D Multisample"; break;
			case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:				return "GL Sampler 2D Multisample Array"; break;
			case GL_SAMPLER_CUBE_SHADOW:						return "GL Sampler Cube Shadow"; break;
			case GL_SAMPLER_BUFFER:								return "GL Sampler Buffer"; break;
			case GL_SAMPLER_2D_RECT:							return "GL Sampler 2D Rect"; break;
			case GL_SAMPLER_2D_RECT_SHADOW:						return "GL Sampler 2D Rect Shadow"; break;
			case GL_INT_SAMPLER_1D:								return "GL Int Sampler 1D"; break;
			case GL_INT_SAMPLER_2D:								return "GL Int Sampler 2D"; break;
			case GL_INT_SAMPLER_3D:								return "GL Int Sampler 3D"; break;
			case GL_INT_SAMPLER_CUBE:							return "GL Int Sampler Cube"; break;
			case GL_INT_SAMPLER_1D_ARRAY:						return "GL Int Sampler 1D Array"; break;
			case GL_INT_SAMPLER_2D_ARRAY:						return "GL Int Sampler 2D Array"; break;
			case GL_INT_SAMPLER_2D_MULTISAMPLE:					return "GL Int Sampler 2D Multisample"; break;
			case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:			return "GL Int Sampler 2D Multisample Array"; break;
			case GL_INT_SAMPLER_BUFFER:							return "GL Int Sampler Buffer"; break;
			case GL_INT_SAMPLER_2D_RECT:						return "GL Int Sampler 2D Rect"; break;
			case GL_UNSIGNED_INT_SAMPLER_1D:					return "GL Unsigned Int Sampler 1D"; break;
			case GL_UNSIGNED_INT_SAMPLER_2D:					return "GL Unsigned Int Sampler 2D"; break;
			case GL_UNSIGNED_INT_SAMPLER_3D:					return "GL Unsigned Int Sampler 3D"; break;
			case GL_UNSIGNED_INT_SAMPLER_CUBE:					return "GL Unsigned Int Sampler Cube"; break;
			case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:				return "GL Unsigned Int Sampler 1D Array"; break;
			case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:				return "GL Unsigned Int Sampler 2D Array"; break;
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:		return "GL Unsigned Int Sampler 2D Multisample"; break;
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:	return "GL Unsigned Int Sampler 2D Multisample Array"; break;
			case GL_UNSIGNED_INT_SAMPLER_BUFFER:				return "GL Unsigned Int Sampler Buffer"; break;
			case GL_UNSIGNED_INT_SAMPLER_2D_RECT:				return "GL Unsigned Int Sampler 2D Rect"; break;

			default: return "Unhandled GL Type"; break;
		}
		return NULL;
	}
};

#endif // ! OPEN_GL_UTILITY_H_
