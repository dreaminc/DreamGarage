#ifndef DIRTY_H_
#define DIRTY_H_

// DREAM OS
// DreamOS/Dimension/Primitives/dirty.h
// Dirty Object
// Simple dirty flag pattern

class dirty {
private:
	bool m_fDirty;

public:
	dirty() :
		m_fDirty(false)
	{
		/* empty stub */
	}

	~dirty() {
		/* empty stub */
	}

public:
	// Mark the object as dirty, data should be updated by the renderer
	RESULT SetDirty() {
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
