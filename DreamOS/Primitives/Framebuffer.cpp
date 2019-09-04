#include "Framebuffer.h"

framebuffer::framebuffer() :
	m_width(0),
	m_height(0),
	m_channels(DEFAULT_FRAMEBUFFER_CHANNELS)
{
	// empty
}

framebuffer::framebuffer(int width, int height, int channels) :
	m_width(width),
	m_height(height),
	m_channels(channels)
{
	// empty
}

framebuffer::~framebuffer() {
	// empty
	//ClearAttachments();

}
