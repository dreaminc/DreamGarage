#ifndef _DISPLAYMANAGER_H_
#define _DISPLAYMANAGER_H_

#include "RESULT/EHM.h"

// Dream Desktop Duplication

#include "CommonTypes.h"

// Handles the task of processing frames

class D3D11DesktopDuplicationDisplayManager {
public:
	D3D11DesktopDuplicationDisplayManager();
	~D3D11DesktopDuplicationDisplayManager();

	void InitD3D(DX_RESOURCES* pDXResourcesData);

	ID3D11Device* GetDevice();
	DUPL_RETURN ProcessFrame(_In_ FRAME_DATA* pFrameData, _Inout_ ID3D11Texture2D* pSharedSurfaceTexture, INT OffsetX, INT OffsetY, _In_ DXGI_OUTPUT_DESC* pDesktopOutputDescription);
	void CleanRefs();

private:
	// methods
	DUPL_RETURN CopyDirty(_In_ ID3D11Texture2D* pSrcSurface, _Inout_ ID3D11Texture2D* pSharedSurfaceTexture, _In_reads_(DirtyCount) RECT* pDirtyBuffer, UINT DirtyCount, INT OffsetX, INT OffsetY, _In_ DXGI_OUTPUT_DESC* pDesktopOuputDescription);
	DUPL_RETURN CopyMove(_Inout_ ID3D11Texture2D* pSharedSurfaceTexture, _In_reads_(MoveCount) DXGI_OUTDUPL_MOVE_RECT* pMoveBuffer, UINT MoveCount, INT OffsetX, INT OffsetY, _In_ DXGI_OUTPUT_DESC* pDesktopOutputDescription, INT textureWidth, INT textureHeight);
	void SetDirtyVert(_Out_writes_(NUMVERTICES) VERTEX* pVertices, _In_ RECT* pDirtyRect, INT OffsetX, INT OffsetY, _In_ DXGI_OUTPUT_DESC* pDesktopOuputDescription, _In_ D3D11_TEXTURE2D_DESC* pSharedSurfaceTextureDescription, _In_ D3D11_TEXTURE2D_DESC* pSourceSurfaceTextureDescription);
	void SetMoveRect(_Out_ RECT* SrcRect, _Out_ RECT* DestRect, _In_ DXGI_OUTPUT_DESC* DeskDesc, _In_ DXGI_OUTDUPL_MOVE_RECT* MoveRect, INT TexWidth, INT TexHeight);

	// variables
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	ID3D11Texture2D* m_pMoveSurf = nullptr;
	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
	ID3D11RenderTargetView* m_pRTV = nullptr;
	ID3D11SamplerState* m_pSamplerLinear = nullptr;
	BYTE* m_pDirtyVertexBufferAlloc = nullptr;
	UINT m_DirtyVertexBufferAllocSize = 0;
};

#endif
