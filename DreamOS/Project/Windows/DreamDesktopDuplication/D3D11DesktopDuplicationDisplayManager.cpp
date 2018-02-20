#include "D3D11DesktopDuplicationDisplayManager.h"
#include "RESULT/EHM.h"
using namespace DirectX;

//
// Constructor
//
D3D11DesktopDuplicationDisplayManager::D3D11DesktopDuplicationDisplayManager() {
	// empty
}

//
// Destructor calls CleanRefs to destroy everything
//
D3D11DesktopDuplicationDisplayManager::~D3D11DesktopDuplicationDisplayManager() {
	CleanRefs();

	if (m_pDirtyVertexBufferAlloc) {
		delete[] m_pDirtyVertexBufferAlloc;
		m_pDirtyVertexBufferAlloc = nullptr;
	}
}

//
// Initialize D3D variables
//
void D3D11DesktopDuplicationDisplayManager::InitD3D(DX_RESOURCES* pDXResourceData) {
	m_pDevice = pDXResourceData->Device;
	m_pDeviceContext = pDXResourceData->Context;
	m_pVertexShader = pDXResourceData->VertexShader;
	m_pPixelShader = pDXResourceData->PixelShader;
	m_pInputLayout = pDXResourceData->InputLayout;
	m_pSamplerLinear = pDXResourceData->SamplerLinear;

	m_pDevice->AddRef();
	m_pDeviceContext->AddRef();
	m_pVertexShader->AddRef();
	m_pPixelShader->AddRef();
	m_pInputLayout->AddRef();
	m_pSamplerLinear->AddRef();
}

//
// Process a given frame and its metadata
//
DUPL_RETURN D3D11DesktopDuplicationDisplayManager::ProcessFrame(_In_ FRAME_DATA* pFrameData, _Inout_ ID3D11Texture2D* pSharedSurfaceTexture, INT OffsetX, INT OffsetY, _In_ DXGI_OUTPUT_DESC* pDesktopOutputDescription) {
	HRESULT r = S_OK;
	DUPL_RETURN Ret = DUPL_RETURN_SUCCESS;

	// Process dirties and moves
	if (pFrameData->FrameInfo.TotalMetadataBufferSize) {
		D3D11_TEXTURE2D_DESC textureDescription;
		pFrameData->Frame->GetDesc(&textureDescription);

		if (pFrameData->MoveCount > 0) {
			Ret = CopyMove(pSharedSurfaceTexture, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(pFrameData->MetaData), pFrameData->MoveCount, OffsetX, OffsetY, pDesktopOutputDescription, textureDescription.Width, textureDescription.Height);
			CB(Ret == DUPL_RETURN_SUCCESS);
		}

		if (pFrameData->DirtyCount > 0) {
			Ret = CopyDirty(pFrameData->Frame, pSharedSurfaceTexture, reinterpret_cast<RECT*>(pFrameData->MetaData + (pFrameData->MoveCount * sizeof(DXGI_OUTDUPL_MOVE_RECT))), pFrameData->DirtyCount, OffsetX, OffsetY, pDesktopOutputDescription);
		}
	}

Error:
	return Ret;
}

//
// Returns D3D device being used
//
ID3D11Device* D3D11DesktopDuplicationDisplayManager::GetDevice() {
	return m_pDevice;
}

//
// Set appropriate source and destination rects for move rects
//
void D3D11DesktopDuplicationDisplayManager::SetMoveRect(_Out_ RECT* pSrcRect, _Out_ RECT* pDestRect, _In_ DXGI_OUTPUT_DESC* pDesktopOutputDescription, _In_ DXGI_OUTDUPL_MOVE_RECT* pMoveRect, INT textureWidth, INT textureHeight) {
	switch (pDesktopOutputDescription->Rotation) {
	case DXGI_MODE_ROTATION_UNSPECIFIED:
	case DXGI_MODE_ROTATION_IDENTITY: {
		pSrcRect->left = pMoveRect->SourcePoint.x;
		pSrcRect->top = pMoveRect->SourcePoint.y;
		pSrcRect->right = pMoveRect->SourcePoint.x + pMoveRect->DestinationRect.right - pMoveRect->DestinationRect.left;
		pSrcRect->bottom = pMoveRect->SourcePoint.y + pMoveRect->DestinationRect.bottom - pMoveRect->DestinationRect.top;

		*pDestRect = pMoveRect->DestinationRect;
		break;
	}
	case DXGI_MODE_ROTATION_ROTATE90: {
		pSrcRect->left = textureHeight - (pMoveRect->SourcePoint.y + pMoveRect->DestinationRect.bottom - pMoveRect->DestinationRect.top);
		pSrcRect->top = pMoveRect->SourcePoint.x;
		pSrcRect->right = textureHeight - pMoveRect->SourcePoint.y;
		pSrcRect->bottom = pMoveRect->SourcePoint.x + pMoveRect->DestinationRect.right - pMoveRect->DestinationRect.left;

		pDestRect->left = textureHeight - pMoveRect->DestinationRect.bottom;
		pDestRect->top = pMoveRect->DestinationRect.left;
		pDestRect->right = textureHeight - pMoveRect->DestinationRect.top;
		pDestRect->bottom = pMoveRect->DestinationRect.right;
		break;
	}
	case DXGI_MODE_ROTATION_ROTATE180: {
		pSrcRect->left = textureWidth - (pMoveRect->SourcePoint.x + pMoveRect->DestinationRect.right - pMoveRect->DestinationRect.left);
		pSrcRect->top = textureHeight - (pMoveRect->SourcePoint.y + pMoveRect->DestinationRect.bottom - pMoveRect->DestinationRect.top);
		pSrcRect->right = textureWidth - pMoveRect->SourcePoint.x;
		pSrcRect->bottom = textureHeight - pMoveRect->SourcePoint.y;

		pDestRect->left = textureWidth - pMoveRect->DestinationRect.right;
		pDestRect->top = textureHeight - pMoveRect->DestinationRect.bottom;
		pDestRect->right = textureWidth - pMoveRect->DestinationRect.left;
		pDestRect->bottom = textureHeight - pMoveRect->DestinationRect.top;
		break;
	}
	case DXGI_MODE_ROTATION_ROTATE270: {
		pSrcRect->left = pMoveRect->SourcePoint.x;
		pSrcRect->top = textureWidth - (pMoveRect->SourcePoint.x + pMoveRect->DestinationRect.right - pMoveRect->DestinationRect.left);
		pSrcRect->right = pMoveRect->SourcePoint.y + pMoveRect->DestinationRect.bottom - pMoveRect->DestinationRect.top;
		pSrcRect->bottom = textureWidth - pMoveRect->SourcePoint.x;

		pDestRect->left = pMoveRect->DestinationRect.top;
		pDestRect->top = textureWidth - pMoveRect->DestinationRect.right;
		pDestRect->right = pMoveRect->DestinationRect.bottom;
		pDestRect->bottom = textureWidth - pMoveRect->DestinationRect.left;
		break;
	}
	default: {
		RtlZeroMemory(pDestRect, sizeof(RECT));
		RtlZeroMemory(pSrcRect, sizeof(RECT));
		break;
	}
	}
}

//
// Copy move rectangles
//
DUPL_RETURN D3D11DesktopDuplicationDisplayManager::CopyMove(_Inout_ ID3D11Texture2D* pSharedSurfaceTexture, _In_reads_(MoveCount) DXGI_OUTDUPL_MOVE_RECT* pMoveBuffer, UINT MoveCount, INT OffsetX, INT OffsetY, _In_ DXGI_OUTPUT_DESC* pDesktopOutputDescription, INT TexWidth, INT TexHeight) {
	HRESULT r = S_OK;
	D3D11_TEXTURE2D_DESC SharedTextureDescription;
	pSharedSurfaceTexture->GetDesc(&SharedTextureDescription);

	// Make new intermediate surface to copy into for moving
	if (!m_pMoveSurf) {
		D3D11_TEXTURE2D_DESC MoveDesc;
		MoveDesc = SharedTextureDescription;
		MoveDesc.Width = pDesktopOutputDescription->DesktopCoordinates.right - pDesktopOutputDescription->DesktopCoordinates.left;
		MoveDesc.Height = pDesktopOutputDescription->DesktopCoordinates.bottom - pDesktopOutputDescription->DesktopCoordinates.top;
		MoveDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		MoveDesc.MiscFlags = 0;
		CH(m_pDevice->CreateTexture2D(&MoveDesc, nullptr, &m_pMoveSurf));	
	}

	for (UINT i = 0; i < MoveCount; ++i) {
		RECT SrcRect;
		RECT DestRect;

		SetMoveRect(&SrcRect, &DestRect, pDesktopOutputDescription, &(pMoveBuffer[i]), TexWidth, TexHeight);

		// Copy rect out of shared surface
		D3D11_BOX Box;
		Box.left = SrcRect.left + pDesktopOutputDescription->DesktopCoordinates.left - OffsetX;
		Box.top = SrcRect.top + pDesktopOutputDescription->DesktopCoordinates.top - OffsetY;
		Box.front = 0;
		Box.right = SrcRect.right + pDesktopOutputDescription->DesktopCoordinates.left - OffsetX;
		Box.bottom = SrcRect.bottom + pDesktopOutputDescription->DesktopCoordinates.top - OffsetY;
		Box.back = 1;
		m_pDeviceContext->CopySubresourceRegion(m_pMoveSurf, 0, SrcRect.left, SrcRect.top, 0, pSharedSurfaceTexture, 0, &Box);

		// Copy back to shared surface
		Box.left = SrcRect.left;
		Box.top = SrcRect.top;
		Box.front = 0;
		Box.right = SrcRect.right;
		Box.bottom = SrcRect.bottom;
		Box.back = 1;
		m_pDeviceContext->CopySubresourceRegion(pSharedSurfaceTexture, 0, DestRect.left + pDesktopOutputDescription->DesktopCoordinates.left - OffsetX, DestRect.top + pDesktopOutputDescription->DesktopCoordinates.top - OffsetY, 0, m_pMoveSurf, 0, &Box);
	}

Error:
	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed to create staging texture for move rects", L"Error", r, SystemTransitionsExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Sets up vertices for dirty rects for rotated desktops
//
#pragma warning(push)
#pragma warning(disable:__WARNING_USING_UNINIT_VAR) // false positives in SetDirtyVert due to tool bug

void D3D11DesktopDuplicationDisplayManager::SetDirtyVert(_Out_writes_(NUMVERTICES) VERTEX* pVertices, _In_ RECT* pDirtyRect, INT OffsetX, INT OffsetY, _In_ DXGI_OUTPUT_DESC* pDesktopOutputDescription, _In_ D3D11_TEXTURE2D_DESC* pSharedSurfaceTextureDescription, _In_ D3D11_TEXTURE2D_DESC* pSourceSurfaceTextureDescription) {
	INT CenterX = pSharedSurfaceTextureDescription->Width / 2;
	INT CenterY = pSharedSurfaceTextureDescription->Height / 2;

	INT Width = pDesktopOutputDescription->DesktopCoordinates.right - pDesktopOutputDescription->DesktopCoordinates.left;
	INT Height = pDesktopOutputDescription->DesktopCoordinates.bottom - pDesktopOutputDescription->DesktopCoordinates.top;

	// Rotation compensated destination rect
	RECT DestDirty = *pDirtyRect;

	// Set appropriate coordinates compensated for rotation
	switch (pDesktopOutputDescription->Rotation) {
	case DXGI_MODE_ROTATION_ROTATE90: {
		DestDirty.left = Width - pDirtyRect->bottom;
		DestDirty.top = pDirtyRect->left;
		DestDirty.right = Width - pDirtyRect->top;
		DestDirty.bottom = pDirtyRect->right;

		pVertices[0].TexCoord = XMFLOAT2(pDirtyRect->right / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->bottom / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[1].TexCoord = XMFLOAT2(pDirtyRect->left / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->bottom / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[2].TexCoord = XMFLOAT2(pDirtyRect->right / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->top / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[5].TexCoord = XMFLOAT2(pDirtyRect->left / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->top / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		break;
	}
	case DXGI_MODE_ROTATION_ROTATE180: {
		DestDirty.left = Width - pDirtyRect->right;
		DestDirty.top = Height - pDirtyRect->bottom;
		DestDirty.right = Width - pDirtyRect->left;
		DestDirty.bottom = Height - pDirtyRect->top;

		pVertices[0].TexCoord = XMFLOAT2(pDirtyRect->right / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->top / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[1].TexCoord = XMFLOAT2(pDirtyRect->right / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->bottom / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[2].TexCoord = XMFLOAT2(pDirtyRect->left / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->top / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[5].TexCoord = XMFLOAT2(pDirtyRect->left / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->bottom / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		break;
	}
	case DXGI_MODE_ROTATION_ROTATE270: {
		DestDirty.left = pDirtyRect->top;
		DestDirty.top = Height - pDirtyRect->right;
		DestDirty.right = pDirtyRect->bottom;
		DestDirty.bottom = Height - pDirtyRect->left;

		pVertices[0].TexCoord = XMFLOAT2(pDirtyRect->left / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->top / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[1].TexCoord = XMFLOAT2(pDirtyRect->right / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->top / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[2].TexCoord = XMFLOAT2(pDirtyRect->left / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->bottom / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[5].TexCoord = XMFLOAT2(pDirtyRect->right / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->bottom / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		break;
	}
	default:
		assert(false); // drop through
	case DXGI_MODE_ROTATION_UNSPECIFIED:
	case DXGI_MODE_ROTATION_IDENTITY: {
		pVertices[0].TexCoord = XMFLOAT2(pDirtyRect->left / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->bottom / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[1].TexCoord = XMFLOAT2(pDirtyRect->left / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->top / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[2].TexCoord = XMFLOAT2(pDirtyRect->right / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->bottom / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		pVertices[5].TexCoord = XMFLOAT2(pDirtyRect->right / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Width), pDirtyRect->top / static_cast<FLOAT>(pSourceSurfaceTextureDescription->Height));
		break;
	}
	}

	// Set positions
	pVertices[0].Pos = XMFLOAT3((DestDirty.left + pDesktopOutputDescription->DesktopCoordinates.left - OffsetX - CenterX) / static_cast<FLOAT>(CenterX),
		-1 * (DestDirty.bottom + pDesktopOutputDescription->DesktopCoordinates.top - OffsetY - CenterY) / static_cast<FLOAT>(CenterY),
		0.0f);
	pVertices[1].Pos = XMFLOAT3((DestDirty.left + pDesktopOutputDescription->DesktopCoordinates.left - OffsetX - CenterX) / static_cast<FLOAT>(CenterX),
		-1 * (DestDirty.top + pDesktopOutputDescription->DesktopCoordinates.top - OffsetY - CenterY) / static_cast<FLOAT>(CenterY),
		0.0f);
	pVertices[2].Pos = XMFLOAT3((DestDirty.right + pDesktopOutputDescription->DesktopCoordinates.left - OffsetX - CenterX) / static_cast<FLOAT>(CenterX),
		-1 * (DestDirty.bottom + pDesktopOutputDescription->DesktopCoordinates.top - OffsetY - CenterY) / static_cast<FLOAT>(CenterY),
		0.0f);
	pVertices[3].Pos = pVertices[2].Pos;
	pVertices[4].Pos = pVertices[1].Pos;
	pVertices[5].Pos = XMFLOAT3((DestDirty.right + pDesktopOutputDescription->DesktopCoordinates.left - OffsetX - CenterX) / static_cast<FLOAT>(CenterX),
		-1 * (DestDirty.top + pDesktopOutputDescription->DesktopCoordinates.top - OffsetY - CenterY) / static_cast<FLOAT>(CenterY),
		0.0f);

	pVertices[3].TexCoord = pVertices[2].TexCoord;
	pVertices[4].TexCoord = pVertices[1].TexCoord;
}

#pragma warning(pop) // re-enable __WARNING_USING_UNINIT_VAR

//
// Copies dirty rectangles
//
DUPL_RETURN D3D11DesktopDuplicationDisplayManager::CopyDirty(_In_ ID3D11Texture2D* pSrcSurfaceTexture, _Inout_ ID3D11Texture2D* pSharedSurfTexture, _In_reads_(DirtyCount) RECT* pDirtyBuffer, UINT DirtyCount, INT OffsetX, INT OffsetY, _In_ DXGI_OUTPUT_DESC* pDesktopOutputDescription) {
	HRESULT r = S_OK;

	D3D11_TEXTURE2D_DESC sharedSurfaceDescription;
	pSharedSurfTexture->GetDesc(&sharedSurfaceDescription);

	D3D11_TEXTURE2D_DESC sourceSurfaceTextureDescription;
	pSrcSurfaceTexture->GetDesc(&sourceSurfaceTextureDescription);

	if (!m_pRTV) {
		CRM(m_pDevice->CreateRenderTargetView(pSharedSurfTexture, nullptr, &m_pRTV), "Failed to create render target view for dirty rects");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderDescription;
	shaderDescription.Format = sourceSurfaceTextureDescription.Format;
	shaderDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderDescription.Texture2D.MostDetailedMip = sourceSurfaceTextureDescription.MipLevels - 1;
	shaderDescription.Texture2D.MipLevels = sourceSurfaceTextureDescription.MipLevels;

	// Create new shader resource view
	ID3D11ShaderResourceView* pShaderResource = nullptr;
	CRM(m_pDevice->CreateShaderResourceView(pSrcSurfaceTexture, &shaderDescription, &pShaderResource), "Failed to create shader resource view for dirty rects");

	FLOAT BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_pDeviceContext->OMSetBlendState(nullptr, BlendFactor, 0xFFFFFFFF);
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, nullptr);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetShaderResources(0, 1, &pShaderResource);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create space for vertices for the dirty rects if the current space isn't large enough
	UINT BytesNeeded = sizeof(VERTEX) * NUMVERTICES * DirtyCount;
	if (BytesNeeded > m_DirtyVertexBufferAllocSize) {
		if (m_pDirtyVertexBufferAlloc) {
			delete[] m_pDirtyVertexBufferAlloc;
		}

		m_pDirtyVertexBufferAlloc = new (std::nothrow) BYTE[BytesNeeded];
		if (!m_pDirtyVertexBufferAlloc)	{
			m_DirtyVertexBufferAllocSize = 0;
			return ProcessFailure(nullptr, L"Failed to allocate memory for dirty vertex buffer.", L"Error", E_OUTOFMEMORY);
		}

		m_DirtyVertexBufferAllocSize = BytesNeeded;
	}

	// Fill them in
	VERTEX* pDirtyVertex = reinterpret_cast<VERTEX*>(m_pDirtyVertexBufferAlloc);
	for (UINT i = 0; i < DirtyCount; ++i, pDirtyVertex += NUMVERTICES) {
		SetDirtyVert(pDirtyVertex, &(pDirtyBuffer[i]), OffsetX, OffsetY, pDesktopOutputDescription, &sharedSurfaceDescription, &sourceSurfaceTextureDescription);
	}

	// Create vertex buffer
	D3D11_BUFFER_DESC pVertexBufferDescription;
	RtlZeroMemory(&pVertexBufferDescription, sizeof(pVertexBufferDescription));
	pVertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	pVertexBufferDescription.ByteWidth = BytesNeeded;
	pVertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	pVertexBufferDescription.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	RtlZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = m_pDirtyVertexBufferAlloc;

	ID3D11Buffer* pVertexBuffer = nullptr;
	CRM(m_pDevice->CreateBuffer(&pVertexBufferDescription, &InitData, &pVertexBuffer),"Failed to create vertex buffer in dirty rect processing");

	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &Offset);

	D3D11_VIEWPORT VP;
	VP.Width = static_cast<FLOAT>(sharedSurfaceDescription.Width);
	VP.Height = static_cast<FLOAT>(sharedSurfaceDescription.Height);
	VP.MinDepth = 0.0f;
	VP.MaxDepth = 1.0f;
	VP.TopLeftX = 0.0f;
	VP.TopLeftY = 0.0f;
	m_pDeviceContext->RSSetViewports(1, &VP);

	m_pDeviceContext->Draw(NUMVERTICES * DirtyCount, 0);

	pVertexBuffer->Release();
	pVertexBuffer = nullptr;

	pShaderResource->Release();
	pShaderResource = nullptr;

Error:
	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed in CopyDirty of DisplayManager", L"Error", r, SystemTransitionsExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Clean all references
//
void D3D11DesktopDuplicationDisplayManager::CleanRefs() {
	if (m_pDeviceContext) {
		m_pDeviceContext->Release();
		m_pDeviceContext = nullptr;
	}

	if (m_pDevice) {
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	if (m_pMoveSurf) {
		m_pMoveSurf->Release();
		m_pMoveSurf = nullptr;
	}

	if (m_pVertexShader) {
		m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}

	if (m_pPixelShader) {
		m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}

	if (m_pInputLayout)	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}

	if (m_pSamplerLinear) {
		m_pSamplerLinear->Release();
		m_pSamplerLinear = nullptr;
	}

	if (m_pRTV)	{
		m_pRTV->Release();
		m_pRTV = nullptr;
	}
}
