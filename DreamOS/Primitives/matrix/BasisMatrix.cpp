#include "BasisMatrix.h"

BasisMatrix::BasisMatrix(vector vXAxis, vector vYAxis, vector vZAxis) {
	identity(1.0f);

	//this->element(0, 0) = vXAxis.x();
	//this->element(1, 0) = vXAxis.y();
	//this->element(2, 0) = vXAxis.z();
	//
	//this->element(0, 1) = vYAxis.x();
	//this->element(1, 1) = vYAxis.y();
	//this->element(2, 1) = vYAxis.z();
	//
	//this->element(0, 2) = vZAxis.x();
	//this->element(1, 2) = vZAxis.y();
	//this->element(2, 2) = vZAxis.z();

	SetBasisMatrixVectors(vXAxis, vYAxis, vZAxis);
}

BasisMatrix::~BasisMatrix() {
	// empty stub
}

BasisMatrix::BasisMatrix(BasisMatrix::type matType) {
	identity(1.0f);

	SetBasisMatrixType(matType);
}

RESULT BasisMatrix::SetBasisMatrixType(BasisMatrix::type matType) {
	RESULT r = R_PASS;

	vector vLook, vUp, vLeft;

	// TODO: WTF does the up vector have to be negative!!

	switch (matType) {
		case POS_X: {
			vLook = vector(1.0f, 0.0f, 0.0f);
			vUp = vector(0.0f, -1.0f, 0.0f);
			vLeft = vector(0.0f, 0.0f, -1.0f);
		} break;

		case NEG_X: {
			vLook = vector(1.0f, 0.0f, 0.0f);
			vUp = vector(0.0f, -1.0f, 0.0f);
			vLeft = vector(0.0f, 0.0f, 1.0f);
		} break;

		case POS_Y: {
			vLook = vector(0.0f, 1.0f, 0.0f);
			vUp = vector(0.0f, 0.0f, -1.0f);
			vLeft = vector(1.0f, 0.0f, 0.0f);
		} break;

		case NEG_Y: {
			vLook = vector(0.0f, -1.0f, 0.0f);
			vUp = vector(0.0f, 0.0f, 1.0f);
			vLeft = vector(1.0f, 0.0f, 0.0f);
		} break;

		case POS_Z: {
			vLook = vector(0.0f, 0.0f, 1.0f);
			vUp = vector(0.0f, -1.0f, 0.0f);
			vLeft = vector(-1.0f, 0.0f, 0.0f);
		} break;

		case NEG_Z: {
			vLook = vector(0.0f, 0.0f, -1.0f);
			vUp = vector(0.0f, -1.0f, 0.0f);
			vLeft = vector(1.0f, 0.0f, 0.0f);
		} break;
	}

	SetBasisMatrixVectors(vLeft, vUp, vLook);

Error:
	return r;
}

RESULT BasisMatrix::SetBasisMatrixVectors(vector vA, vector vB, vector vC) {

	this->element(0, 0) = vA.x();
	this->element(1, 0) = vA.y();
	this->element(2, 0) = vA.z();

	this->element(0, 1) = vB.x();
	this->element(1, 1) = vB.y();
	this->element(2, 1) = vB.z();

	this->element(0, 2) = vC.x();
	this->element(1, 2) = vC.y();
	this->element(2, 2) = vC.z();

	return R_PASS;
}