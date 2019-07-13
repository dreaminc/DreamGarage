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

public:
	enum class flags : uint16_t {
		NONE = 0,
		SHARE_NETWORK = 1 << 0,
		SHARE_LOCAL = 1 << 1,
		LOOPBACK = 1 << 2, // TODO: limited by current architecture
		INVALID = 0xFFFF
	};

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

inline constexpr DreamAppMessage::flags operator | (const DreamAppMessage::flags &lhs, const DreamAppMessage::flags &rhs) {
	return static_cast<DreamAppMessage::flags>(
		static_cast<std::underlying_type<DreamAppMessage::flags>::type>(lhs) | static_cast<std::underlying_type<DreamAppMessage::flags>::type>(rhs)
		);
}

inline constexpr DreamAppMessage::flags operator & (const DreamAppMessage::flags &lhs, const DreamAppMessage::flags &rhs) {
	return static_cast<DreamAppMessage::flags>(
		static_cast<std::underlying_type<DreamAppMessage::flags>::type>(lhs) & static_cast<std::underlying_type<DreamAppMessage::flags>::type>(rhs)
		);
}

#endif	// ! PEER_STAY_ALIVE_MESSAGE_H_