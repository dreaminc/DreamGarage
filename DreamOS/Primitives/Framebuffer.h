#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "RESULT/EHM.h"
#include "Primitives/texture.h"

// DREAM OS
// DreamOS/Dimension/Primitives/framebuffer.h
// Framebuffer Primitve
// The parent object for the Framebuffer 

#include "Primitives/Types/UID.h"

#define DEFAULT_FRAMEBUFFER_CHANNELS 3

class framebuffer {
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

public:
	UID getID() { return m_uid; }

private:
	UID m_uid;
};

#endif // ! FRAMEBUFFER_H_