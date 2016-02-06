#ifndef DIM_OBJ_H_
#define DIM_OBJ_H_

// DREAM OS
// DreamOS/Dimension/Primitives/DimObj.h
// Dimension Base Object
//   All objects in Dimension should dervice from this base class

class DimObj {
private:
    point m_ptOrigin;   // origin
    //AABV m_aabv;        // Axis Aligned Bounding Volume

public:
    DimObj() :
        m_ptOrigin()
        //m_aabv()
    {
        /* stub */
    }

    ~DimObj() {
        /* stub */
    }

private:
	UID m_uid;
};

#endif // !DIM_OBJ_H_
