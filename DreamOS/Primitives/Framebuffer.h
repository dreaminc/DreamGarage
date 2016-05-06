#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/framebuffer.h
// Framebuffer Primitve
// The parent object for the Framebuffer 

#include "Primitives/Types/UID.h"

class framebuffer {
public:
	framebuffer(int width, int height, int channels) :
		m_width(width),
		m_height(height),
		m_channels(channels)
	{
		// empty
	}

	~framebuffer() {
		// empty
	}

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }
	int GetChannels() { return m_channels; }

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