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
	DreamAppMessage(long senderUserID, long receiverUserID, std::string strSenderDreamAppName, UID uidSenderDreamApp, long messageSize);
	~DreamAppMessage();

	virtual RESULT PrintMessage() override;

	std::string GetDreamAppName();
	UID GetDreamAppUID();
};

#endif	// ! PEER_STAY_ALIVE_MESSAGE_H_