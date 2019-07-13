#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/primitives/framebuffer.h

// Framebuffer Primitive
// The parent object for the Framebuffer 

#include "core/types/DObject.h"

#include "core/primitives/texture.h"

#define DEFAULT_FRAMEBUFFER_CHANNELS 3

class framebuffer : public DObject {
public:
	framebuffer();
	framebuffer(int width, int height, int channels);
	virtual ~framebuffer() = 0;

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }
	int GetChannels() { return m_channels; }

	virtual texture *GetColorTexture() = 0;
	
	virtual RESULT ClearAttachments() = 0;

protected:
	int m_width;
	int m_height;
	int m_channels;
};

#endif // ! FRAMEBUFFER_H_