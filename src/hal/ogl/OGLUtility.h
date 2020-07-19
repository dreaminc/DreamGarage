#ifndef OPEN_GL_UTILITY_H_
#define OPEN_GL_UTILITY_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLUtility.h

// OpenGL Utility functions
// Generally state-less so should be static

#include "OGLCommon.h"

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

			// Don't exist for EGL (TODO: switch to full on EGL HAL?)
			#ifdef GL_2_BYTES
				case GL_2_BYTES:									return "GL 2 Bytes"; break;
            #endif

			#ifdef GL_3_BYTES
				case GL_3_BYTES:									return "GL 3 Bytes"; break;
            #endif

			#ifdef GL_4_BYTES
				case GL_4_BYTES:									return "GL 4 Bytes"; break;
			#endif

			#ifdef GL_DOUBLE
				case GL_DOUBLE:										return "GL Double"; break;
			#endif

			case GL_FLOAT_VEC3:									return "GL Float Vec3"; break;
			case GL_FLOAT_VEC4:									return "GL Float Vec4"; break;
			case GL_FLOAT_VEC2:									return "GL Float Vec2"; break;

#ifdef GL_DOUBLE
			case GL_DOUBLE_VEC2:								return "GL Double Vec2"; break;
			case GL_DOUBLE_VEC3:								return "GL Double Vec3"; break;
			case GL_DOUBLE_VEC4:								return "GL Double Vec4"; break;
#endif

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

#ifdef GL_DOUBLE
			case GL_DOUBLE_MAT2:								return "GL Double Matrix 2"; break;
			case GL_DOUBLE_MAT3:								return "GL Double Matrix 3"; break;
			case GL_DOUBLE_MAT4:								return "GL Double Matrix 4"; break;
			case GL_DOUBLE_MAT2x3:								return "GL Double Matrix 2x3"; break;
			case GL_DOUBLE_MAT2x4:								return "GL Double Matrix 2x4"; break;
			case GL_DOUBLE_MAT3x2:								return "GL Double Matrix 3x2"; break;
			case GL_DOUBLE_MAT3x4:								return "GL Double Matrix 3x4"; break;
			case GL_DOUBLE_MAT4x2:								return "GL Double Matrix 4x2"; break;
			case GL_DOUBLE_MAT4x3:								return "GL Double Matrix 4x3"; break;
#endif

#ifdef GL_SAMPLER_1D
			case GL_SAMPLER_1D:									return "GL Sampler 1D"; break;
#endif

			case GL_SAMPLER_2D:									return "GL Sampler 2D"; break;
			case GL_SAMPLER_3D:									return "GL Sampler 3D"; break;
			case GL_SAMPLER_CUBE:								return "GL Sampler Cube"; break;

#ifdef GL_SAMPLER_1D_SHADOW
			case GL_SAMPLER_1D_SHADOW:							return "GL Sampler 1D Shadow"; break;
#endif

			case GL_SAMPLER_2D_SHADOW:							return "GL Sampler 2D Shadow"; break;

#ifdef GL_SAMPLER_1D_ARRAY
			case GL_SAMPLER_1D_ARRAY:							return "GL Sampler 1D Array"; break;
#endif

			case GL_SAMPLER_2D_ARRAY:							return "GL Sampler 2D Array"; break;

#ifdef GL_SAMPLER_1D_ARRAY_SHADOW
			case GL_SAMPLER_1D_ARRAY_SHADOW:					return "GL Sampler 1D Array Shadow"; break;
#endif

			case GL_SAMPLER_2D_ARRAY_SHADOW:					return "GL Sampler 2D Array Shadow"; break;

#ifdef GL_SAMPLER_2D_MULTISAMPLE
			case GL_SAMPLER_2D_MULTISAMPLE:						return "GL Sampler 2D Multisample"; break;
			case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:				return "GL Sampler 2D Multisample Array"; break;
#endif

			case GL_SAMPLER_CUBE_SHADOW:						return "GL Sampler Cube Shadow"; break;

#ifdef GL_SAMPLER_BUFFER
			case GL_SAMPLER_BUFFER:								return "GL Sampler Buffer"; break;
			case GL_SAMPLER_2D_RECT:							return "GL Sampler 2D Rect"; break;
			case GL_SAMPLER_2D_RECT_SHADOW:						return "GL Sampler 2D Rect Shadow"; break;
#endif

#ifdef GL_INT_SAMPLER_1D
			case GL_INT_SAMPLER_1D:								return "GL Int Sampler 1D"; break;
#endif

			case GL_INT_SAMPLER_2D:								return "GL Int Sampler 2D"; break;
			case GL_INT_SAMPLER_3D:								return "GL Int Sampler 3D"; break;
			case GL_INT_SAMPLER_CUBE:							return "GL Int Sampler Cube"; break;

#ifdef GL_INT_SAMPLER_1D_ARRAY
			case GL_INT_SAMPLER_1D_ARRAY:						return "GL Int Sampler 1D Array"; break;
#endif

			case GL_INT_SAMPLER_2D_ARRAY:						return "GL Int Sampler 2D Array"; break;

#ifdef GL_INT_SAMPLER_2D_MULTISAMPLE
			case GL_INT_SAMPLER_2D_MULTISAMPLE:					return "GL Int Sampler 2D Multisample"; break;
			case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:			return "GL Int Sampler 2D Multisample Array"; break;
			case GL_INT_SAMPLER_BUFFER:							return "GL Int Sampler Buffer"; break;
			case GL_INT_SAMPLER_2D_RECT:						return "GL Int Sampler 2D Rect"; break;

			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:		return "GL Unsigned Int Sampler 2D Multisample"; break;
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:	return "GL Unsigned Int Sampler 2D Multisample Array"; break;
			case GL_UNSIGNED_INT_SAMPLER_BUFFER:				return "GL Unsigned Int Sampler Buffer"; break;
			case GL_UNSIGNED_INT_SAMPLER_2D_RECT:				return "GL Unsigned Int Sampler 2D Rect"; break;
#endif

#ifdef GL_UNSIGNED_INT_SAMPLER_1D
			case GL_UNSIGNED_INT_SAMPLER_1D:					return "GL Unsigned Int Sampler 1D"; break;
#endif

			case GL_UNSIGNED_INT_SAMPLER_2D:					return "GL Unsigned Int Sampler 2D"; break;
			case GL_UNSIGNED_INT_SAMPLER_3D:					return "GL Unsigned Int Sampler 3D"; break;
			case GL_UNSIGNED_INT_SAMPLER_CUBE:					return "GL Unsigned Int Sampler Cube"; break;

#ifdef GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
			case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:				return "GL Unsigned Int Sampler 1D Array"; break;
#endif

			case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:				return "GL Unsigned Int Sampler 2D Array"; break;


			default: return "Unhandled GL Type"; break;
		}
		return NULL;
	}
};

#endif // ! OPEN_GL_UTILITY_H_
