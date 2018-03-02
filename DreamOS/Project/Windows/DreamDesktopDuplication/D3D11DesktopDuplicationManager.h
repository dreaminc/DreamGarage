#ifndef _DUPLICATIONMANAGER_H_
#define _DUPLICATIONMANAGER_H_

#include "CommonTypes.h"
//#include "RESULT/EHM.h"
//
// Handles the task of duplicating an output.
//
class D3D11DesktopDuplicationManager //: public D3D11DesktopController
{

public:
	D3D11DesktopDuplicationManager();
	~D3D11DesktopDuplicationManager();
	_Success_(*Timeout == false && return == DUPL_RETURN_SUCCESS) DUPL_RETURN GetFrame(_Out_ FRAME_DATA* pFrameData, _Out_ bool* pTimeout);
	DUPL_RETURN DoneWithFrame();
	DUPL_RETURN InitDupl(_In_ ID3D11Device* pDevice, UINT Output);
	DUPL_RETURN GetMouse(_Inout_ PTR_INFO* pPtrInfo, _In_ DXGI_OUTDUPL_FRAME_INFO* pFrameInfo, INT OffsetX, INT OffsetY);
	void GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* pOutputDescription);

private:

	// vars
	IDXGIOutputDuplication* m_pDeskDupl = nullptr;
	ID3D11Texture2D* m_pAcquiredDesktopImage = nullptr;
	_Field_size_bytes_(m_MetaDataSize) BYTE* m_pMetaDataBuffer = nullptr;
	UINT m_MetaDataSize = 0;
	UINT m_OutputNumber = 0;
	DXGI_OUTPUT_DESC m_OutputDesc;
	ID3D11Device* m_pDevice = nullptr;
};

#endif
