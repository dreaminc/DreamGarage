#ifndef VALID_H_
#define VALID_H_

// DREAM OS
// DreamOS/Dimension/Primitives/valid.h
// Valid Object
// Simple class function that plugs into the Valid EHM macros
// effectively a way to test whether a constructor of an object
// was successful or not

// Useful with the CV, CVM EHM

class valid {
private:
	bool m_fValidated;
	bool m_fValid;

public:
	valid() :
		m_fValid(false),
		m_fValidated(false)
	{
		/* empty stub */
	}

	~valid() {
		/* empty stub */
	}

public:
	void Validate() {
		if (m_fValidated)
			return;

		m_fValidated = true;
		m_fValid = true;
	}

	void Invalidate() {
		if (m_fValidated)
			return;

		m_fValidated = true;
		m_fValid = false;
	}

	void InvalidateOverride() {
		m_fValidated = true;
		m_fValid = false;
	}

	void ValidateOverride() {
		m_fValidated = true;
		m_fValid = true;
	}

public:
	bool IsValid() {
		return m_fValid;
	}

};

#endif // ! VALID_H_
