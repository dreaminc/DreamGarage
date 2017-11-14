// Copyright (c) 2015 Marshall A. Greenblatt. All rights reserved.
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

// NOTE: This has been borrowed from the pre-existing CEF PR
// https://bitbucket.org/chromiumembedded/cef/pull-requests/18

#ifndef CEF_INCLUDE_CEF_AUDIO_HANDLER_H_
#define CEF_INCLUDE_CEF_AUDIO_HANDLER_H_
#pragma once

#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_audio_bus.h"

///
// Implement this interface to handle audio events
///
/*--cef(source=client)--*/
class CefAudioHandler : public virtual CefBaseRefCounted {
public:
	///
	// Event handler for every new audio stream, |audio_stream_id|
	// will identify this stream in future events, and is unique across 
	// all other audio stream. OnAudioStreamStarted can be called also 
	// after OnAudioSteamStopped was called.
	//
	// Will be called on the UI thread 
	///
	/*--cef()--*/
	virtual void OnAudioStreamStarted(CefRefPtr<CefBrowser> browser,
		int audio_stream_id,
		int sample_rate,
		int channels,
		int bits_per_sample,
		int64 buffer_duration) { }

	///
	// Event handler for new audio packet from |audio_stream_id|,
	// |data| is a uint8 array representing a raw PCM packet
	// |data_length| is the size of the array.
	//
	// Will be called on the UI thread 
	///
	/*--cef()--*/
	virtual void OnAudioStreamPacket(CefRefPtr<CefBrowser> browser,
		int audio_stream_id,
		const void* data,
		int data_length) { }

	///
	// Event handler for stream |audio_stream_id| has been stopped,
	// for every OnAudioStreamStarted, OnAudioSteamStopped will be 
	// called.
	//
	// Will be called on the UI thread
	///
	/*--cef()--*/
	virtual void OnAudioSteamStopped(CefRefPtr<CefBrowser> browser,
		int audio_stream_id) { }

	///
	// Event handler for stream mirroring
	//
	// Will be called on the UI thread
	///
	/*--cef()--*/
	virtual void OnAudioData(CefRefPtr<CefBrowser> browser, 
                             int frames, int channels, int bits_per_sample,
							 const void* data_buffer) { }

	///
	// Will be called before every OnAudioStreamPacket call,
	// this getter should return the amount of time in milliseconds
	// the audio should be delayed.
	//
	// Returning any number greater then 0 will also delay any
	// video attached to the stream identified by |audio_stream_id|.
	//
	// The return value should be a non negative number.
	//
	// Will be called on the UI thread
	///
	/*--cef()--*/
	virtual int GetLatencyInMilliseconds(CefRefPtr<CefBrowser> browser,
		int audio_stream_id) {
		return 0;
	}
};

#endif  // CEF_INCLUDE_CEF_AUDIO_HANDLER_H_