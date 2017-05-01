#ifndef TEXT_ENTRY_STRING_H_
#define TEXT_ENTRY_STRING_H_

#include "RESULT/EHM.h"
#include "Primitives/dirty.h"
#include <string>

class TextEntryString : public dirty {
public:
	RESULT UpdateString(unsigned int keyCode);

	std::string m_string;
};

#endif // ! TEXT_ENTRY_STRING_H_