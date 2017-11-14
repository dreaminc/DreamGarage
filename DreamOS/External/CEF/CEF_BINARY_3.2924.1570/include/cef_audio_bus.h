// Copyright (c) 2012 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the CEF translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef CEF_INCLUDE_CEF_AUDIO_BUS_H_
#define CEF_INCLUDE_CEF_AUDIO_BUS_H_
#pragma once

#include <vector>
#include "include/cef_base.h"
#include "include/cef_drag_data.h"
#include "include/cef_frame.h"
#include "include/cef_image.h"
#include "include/cef_navigation_entry.h"
#include "include/cef_process_message.h"
#include "include/cef_request_context.h"

class CefBrowserHost;
class CefClient;
///
// Class used to represent a browser window. When used in the browser process
// the methods of this class may be called on any thread unless otherwise
// indicated in the comments. When used in the render process the methods of
// this class may only be called on the main thread.
///
/*--cef(source=library)--*/
class CefAudioBus : public virtual CefBaseRefCounted {
public:
	///
	// Returns number of frames
	///
	/*--cef()--*/
	virtual int num_frames() = 0;

	///
	// Gets interleaved float buffer
	///
	/*--cef()--*/
	virtual void ToInterleavedFloatBuffer(int num_frames_to_read,
		float* dest_buffer) = 0;

	///
	// Gets interleaved unsigned int8 buffer
	///
	/*--cef()--*/
	virtual void ToInterleavedUint8Buffer(int num_frames_to_read,
		char* dest_buffer) = 0;
};

#endif  // CEF_INCLUDE_CEF_AUDIO_BUS_H_