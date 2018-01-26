#ifndef DESKTOP_DUPLICATION_CONTROLLER_H_
#define DESKTOP_DUPLICATION_CONTROLLER_H_

#include "RESULT/EHM.h"

class D3D11DesktopController {
public:
	class observer {
	public:
		virtual RESULT OnPaint(const void *pBuffer, int width, int height) = 0;
	};

	RESULT RegisterDesktopControllerObserver(D3D11DesktopController::observer* pDesktopControllerObserver);

protected:
	D3D11DesktopController::observer* m_pDesktopControllerObserver = nullptr;

};

#endif
