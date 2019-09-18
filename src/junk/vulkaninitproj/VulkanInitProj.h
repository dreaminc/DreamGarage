#ifndef VULKAN_INIT_PROJ_
#define VULKAN_INIT_PROJ_

#include "core/ehm/EHM.h"

#include <stdlib.h>

// DREAM OS
// dos/src/junk/vulkinitproj/VulkanInitProj.h
// A junk project to initialize vulkan to explore the API for porting

// TODO: This is a bit of a hack around the WIN32_LEAN_AND_MEAN OpenGL compilation issue
#if defined(_WIN32)
	#include <windows.h>
	#include <shellapi.h>
#endif


#endif // ! VULKAN_INIT_PROJ_