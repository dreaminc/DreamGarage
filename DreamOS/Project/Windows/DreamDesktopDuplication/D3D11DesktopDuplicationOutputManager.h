#ifndef _OUTPUTMANAGER_H_
#define _OUTPUTMANAGER_H_

#include <stdio.h>

#include "CommonTypes.h"
#include "warning.h"

//
// Handles the task of drawing into a window.
// Has the functionality to draw the mouse given a mouse shape buffer and position
//
class D3D11DesktopDuplicationOutputManager {
public:
	D3D11DesktopDuplicationOutputManager();
	~D3D11DesktopDuplicationOutputManager();
	DUPL_RETURN InitOutput(HWND Window, INT outputToDuplicate, _Out_ UINT* OutCount, _Out_ RECT* pDeskBounds);
	DUPL_RETURN UpdateApplicationWindow(_In_ PTR_INFO* pPointerInfo, _Inout_ bool* Occluded, BYTE **pBuffer, UINT &pxWidth, UINT &pxHeight);
	HRESULT CopyToSendToDream(BYTE** pBuffer, UINT &pxWidth, UINT &pxHeight);
	void CleanRefs();
	HANDLE GetSharedHandle();
	void WindowResize();

private:
	// Methods
	DUPL_RETURN ProcessMonoMask(bool IsMono, _Inout_ PTR_INFO* pPtrInfo, _Out_ INT* pPtrWidth, _Out_ INT* pPtrHeight, _Out_ INT* pPtrLeft, _Out_ INT* pPtrTop, _Outptr_result_bytebuffer_(*pPtrHeight * *pPtrWidth * BPP) BYTE** pInitBuffer, _Out_ D3D11_BOX* pBox);
	DUPL_RETURN MakeRTV();
	void SetViewPort(UINT Width, UINT Height);
	DUPL_RETURN InitShaders();
	DUPL_RETURN InitGeometry();
	DUPL_RETURN CreateSharedSurf(INT outputToMonitor, _Out_ UINT* pOutCount, _Out_ RECT* pDeskBounds);
	DUPL_RETURN DrawFrame();
	DUPL_RETURN DrawMouse(_In_ PTR_INFO* pPtrInfo);
	DUPL_RETURN ResizeSwapChain();

	// Vars
	unsigned char* m_pTextureByteBuffer;

	IDXGISwapChain1* m_pSwapChain = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	IDXGIFactory2* m_pFactory = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	ID3D11RenderTargetView* m_pRTV = nullptr;
	ID3D11SamplerState* m_pSamplerLinear = nullptr;
	ID3D11BlendState* m_pBlendState = nullptr;
	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
	ID3D11Texture2D* m_pSharedSurf = nullptr;
	IDXGIKeyedMutex* m_pKeyMutex = nullptr;
	HWND m_pWindowHandle = nullptr;
	bool m_fNeedsResize = false;
	DWORD m_OcclusionCookie = 0;
};

#endif