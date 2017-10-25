#ifndef DREAM_APP_MESSAGE_H_
#define DREAM_APP_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamAppMessage.h
// A base message for dream app messages

#include <string>
#include "DreamMessage.h"
#include "Primitives/Types/UID.h"

class DreamAppMessage : public DreamMessage {

private:
	__declspec(align(8)) struct DreamAppMessageHeader {
		std::string strSenderDreamAppName;
		UID uidSenderDreamApp;
	} m_dreamAppMessageHeader;

public:
	DreamAppMessage(long senderUserID, long receiverUserID,  std::string strSenderDreamAppName, UID uidSenderDreamApp) :
		DreamMessage(senderUserID, receiverUserID, DreamMessage::type::APP, sizeof(DreamAppMessage))
	{
		m_dreamAppMessageHeader.strSenderDreamAppName = strSenderDreamAppName;
		m_dreamAppMessageHeader.uidSenderDreamApp;
	}

	~DreamAppMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		RESULT r = R_PASS;

		DEBUG_LINEOUT("DreamAppMessage sender:%s UID: %I64u", 
			m_dreamAppMessageHeader.strSenderDreamAppName.c_str(), m_dreamAppMessageHeader.uidSenderDreamApp.GetID());
		
		CR(DreamMessage::PrintMessage());

	Error:
		return R_PASS;
	}	
};

#endif	// ! PEER_STAY_ALIVE_MESSAGE_H_