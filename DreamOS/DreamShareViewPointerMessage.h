#ifndef DREAM_POINTER_MESSAGE_H_
#define DREAM_POINTER_MESSAGE_H_

#include "RESULT/EHM.h"

#include <string>
#include "DreamShareViewMessage.h"
#include "Primitives/Types/UID.h"

#include "Primitives/point.h"
#include "Primitives/color.h"

class DreamShareViewPointerMessage : public DreamShareViewMessage {

public:
	enum class flags : uint8_t {
		NONE = 0,
		LEFT = 1 << 0,
		ACTUATED = 1 << 1,
		INTERACTING = 1 << 2,

		INVALID = 0xFF
	};

	__declspec(align(8)) struct MessageBody {
		point ptPointer;
		int seatPosition;
		char szInitials[2]; // always 2 characters

		flags fFlags;

	} m_body;

public:
	DreamShareViewPointerMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, point ptPointer, int seatPosition, char strInitials[2], bool fActuated, bool fInteracting, bool fLeftHand);
	~DreamShareViewPointerMessage();

	RESULT PrintMessage() override;

	bool IsLeft();
	bool IsActuated();
	bool IsInteracting();
};

inline constexpr DreamShareViewPointerMessage::flags operator | (const DreamShareViewPointerMessage::flags &lhs, const DreamShareViewPointerMessage::flags &rhs) {
	return static_cast<DreamShareViewPointerMessage::flags>(
		static_cast<std::underlying_type<DreamShareViewPointerMessage::flags>::type>(lhs) | static_cast<std::underlying_type<DreamShareViewPointerMessage::flags>::type>(rhs)
		);
}

inline constexpr DreamShareViewPointerMessage::flags operator & (const DreamShareViewPointerMessage::flags &lhs, const DreamShareViewPointerMessage::flags &rhs) {
	return static_cast<DreamShareViewPointerMessage::flags>(
		static_cast<std::underlying_type<DreamShareViewPointerMessage::flags>::type>(lhs) & static_cast<std::underlying_type<DreamShareViewPointerMessage::flags>::type>(rhs)
		);
}

#endif // ! DREAM_POINTER_MESSAGE_H_