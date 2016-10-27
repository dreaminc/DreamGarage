#ifndef WEBRTC_ICE_CANDIDATE_H_
#define WEBRTC_ICE_CANDIDATE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCICECandidate.h
// The WebRTC ICE Candidate object

class WebRTCICECandidate {
public:
	WebRTCICECandidate(std::string strSDPCandidate = "", std::string strSDPMediaID = "", int sdpMediateLineIndex = -1) :
		m_strSDPCandidate(strSDPCandidate),
		m_strSDPMediaID(strSDPMediaID),
		m_SDPMediateLineIndex(sdpMediateLineIndex)
	{
		// empty
	}

public:
	std::string m_strSDPCandidate;
	std::string m_strSDPMediaID;
	int m_SDPMediateLineIndex;

	RESULT Print() {
		DEBUG_LINEOUT("candidate: %s", m_strSDPCandidate.c_str());
		DEBUG_LINEOUT("Media ID: %s", m_strSDPMediaID.c_str());
		DEBUG_LINEOUT("Media Line Index: %d", m_SDPMediateLineIndex);
		return R_PASS;
	}
};


#endif // ! WEBRTC_ICE_CANDIDATE_H_