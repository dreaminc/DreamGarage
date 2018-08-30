#include "OpenGLCommon.h"

GLenum GetOpenGLPixelFormat(PIXEL_FORMAT pixelFormat, int channels) {
	switch (pixelFormat) {
	case PIXEL_FORMAT::Unspecified: {
		if (channels == 3)
			return GL_RGB;
		else
			return GL_RGBA;
	} break;

	case PIXEL_FORMAT::RGB: {
		return GL_RGB;
	} break;

	case PIXEL_FORMAT::RGBA: {
		return GL_RGBA;
	} break;

	case PIXEL_FORMAT::BGR: {
		return GL_BGR;
	} break;

	case PIXEL_FORMAT::BGRA: {
		return GL_BGRA;
	} break;
	}

	return 0; // no format for unknown
}