#ifndef ROTATION_MATRIX_H_
#define ROTATION_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/RotationMatrix.h
// Rotation Matrix 
// A general purpose rotation matrix

#include "matrix.h"
#include "Primitives/vector.h"
#include "Primitives/quaternion.h"

//#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4

#ifdef FLOAT_PRECISION
	typedef float rotation_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double rotation_precision;
#endif

#ifdef RIGHT_HANDED
	#define ROTATION_HAND_SIGN 1
#elif defined(LEFT_HANDED)
	#define ROTATION_HAND_SIGN -1
#endif

class ray;

class RotationMatrix : public matrix<rotation_precision, 4, 4> {
public:
	typedef enum {
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		XYZ_AXIS,
		ARBITRARY_AXIS,
		QUATERNION,
		VECTOR,
		INVALID
	} ROTATION_MATRIX_TYPE;

	RotationMatrix();
	RotationMatrix(vector rotationAxis, rotation_precision theta);
	RotationMatrix(rotation_precision thetaX, rotation_precision thetaY, rotation_precision thetaZ);
	RotationMatrix(vector vEuler);
	RotationMatrix(ROTATION_MATRIX_TYPE axis, rotation_precision theta);
	RotationMatrix(quaternion q);
	RotationMatrix(vector v, vector up);
	
	~RotationMatrix();

	RESULT SetQuaternionRotationMatrix(quaternion q);
	quaternion GetQuaternion();
	
	RESULT SetVectorRotationMatrix(vector v, vector up);
	RESULT SetArbitraryVectorRotationMatrix(vector u, rotation_precision theta);
	RESULT SetXYZRotationMatrix(rotation_precision phi, rotation_precision theta, rotation_precision psi);
	
	RESULT SetXRotationMatrix(rotation_precision theta);
	RESULT SetYRotationMatrix(rotation_precision theta);
	RESULT SetZRotationMatrix(rotation_precision theta);

	RESULT PrintMatrix();

private:
	ROTATION_MATRIX_TYPE m_type;
};

#endif // ! TRANSLATION_MATRIX_H_
#pragma once
