#ifndef SOUND_COMMON_H_
#define SOUND_COMMON_H_

// DREAM OS
// DreamOS/Sound/SoundCommon.h

#define DEFAULT_SAMPLING_RATE 44100

namespace sound {

	enum class type {
		UNSIGNED_8_BIT,
		SIGNED_16_BIT,
		FLOATING_POINT_32_BIT,
		FLOATING_POINT_64_BIT,
		INVALID
	};

	enum class state {
		UNINITIALIZED,
		STOPPED,
		RUNNING,
		INVALID
	};
}

#endif // ! SOUND_COMMON_H_