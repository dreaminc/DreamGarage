#ifndef DIRTY_H_
#define DIRTY_H_

#include "core/ehm/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/dirty.h
// Dirty Object
// Simple dirty flag pattern

class dirty {
private:
	bool m_fDirty;

public:
	dirty(bool fStartState = false) :
		m_fDirty(fStartState)
	{
		/* empty stub */
	}

	~dirty() {
		/* empty stub */
	}

public:
	// Mark the object as dirty, data should be updated by the renderer
	virtual RESULT SetDirty() {
		m_fDirty = true;
		return R_PASS;
	}

	RESULT CleanDirty() {
		m_fDirty = false;
		return R_PASS;
	}

	// Check if dirty, and clean the dirty state
	bool CheckAndCleanDirty() {
		bool fDirty = m_fDirty;
		m_fDirty = false;

		return fDirty;
	}

	bool IsDirty() {
		return m_fDirty;
	}

};

#endif // ! DIRTY_H_
