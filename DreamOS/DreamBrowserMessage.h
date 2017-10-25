#ifndef DREAM_BROWSER_MESSAGE_H_
#define DREAM_BROWSER_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamBrowserMessage.h
// A base message for dream app messages

#include <string>
#include "DreamAppMessage.h"
#include "Primitives/Types/UID.h"

#define DREAM_BROWSER_APP_NAME "DreamBrowser"

class DreamBrowserMessage : public DreamAppMessage {

public:
	enum class type : Message::DataType {
		PING,
		INVALID
	};

private:
	__declspec(align(8)) struct MessageBody {
		DreamBrowserMessage::type m_msgType;
	} m_body;

public:
	DreamBrowserMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp);

	~DreamBrowserMessage();

	virtual RESULT PrintMessage() override;
};

#endif	// ! DREAM_BROWSER_MESSAGE_H_