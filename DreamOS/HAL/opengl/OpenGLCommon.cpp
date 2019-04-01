#include "OpenGLCommon.h"

GLenum GetOpenGLPixelFormat(PIXEL_FORMAT pixelFormat, int channels) {
	switch (pixelFormat) {
		case PIXEL_FORMAT::GREYSCALE: {
			return GL_RED;
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

GLint GetInternalOpenGLPixelFormat(PIXEL_FORMAT pixelFormat, int bitsPerPixel, int channels) {
	switch (pixelFormat) {
		case PIXEL_FORMAT::GREYSCALE: {
			return GL_R8;
		} break;

		case PIXEL_FORMAT::BGR: 
		case PIXEL_FORMAT::RGB: {
			if (bitsPerPixel == 8)
				return GL_RGB8;
			else if (bitsPerPixel == 16)
				return GL_RGB16;
		} break;
		
		case PIXEL_FORMAT::BGRA: 
		case PIXEL_FORMAT::RGBA: {
			if (bitsPerPixel == 8)
				return GL_RGBA8;
			else if (bitsPerPixel == 16)
				return GL_RGBA16;
		} break;
	}

	return GL_RGBA8; // no format for unknown
}