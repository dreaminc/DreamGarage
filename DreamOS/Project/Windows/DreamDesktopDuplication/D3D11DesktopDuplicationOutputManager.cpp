#include "D3D11DesktopDuplicationOutputManager.h"
#include <vector>
#include "RESULT/EHM.h"
using namespace DirectX;

//
// Constructor NULLs out all pointers & sets appropriate var vals
//
D3D11DesktopDuplicationOutputManager::D3D11DesktopDuplicationOutputManager() 
{
	// empty
}

//
// Destructor which calls CleanRefs to release all references and memory.
//
D3D11DesktopDuplicationOutputManager::~D3D11DesktopDuplicationOutputManager() {
	CleanRefs();
}

//
// Indicates that window has been resized.
//
void D3D11DesktopDuplicationOutputManager::WindowResize() {
	m_fNeedsResize = true;
}

//
// Initialize all state
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::InitOutput(HWND Window, INT outputToDuplicate, _Out_ UINT* OutCount, _Out_ RECT* DeskBounds) {
	HRESULT r = S_OK;

	// Store window handle
	m_pWindowHandle = Window;

	// Driver types supported
	D3D_DRIVER_TYPE DriverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
/*
#ifdef _USE_TEST_APP
	// If the project is in a debug build, enable the debug layer.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
*/

	// Feature levels supported
	D3D_FEATURE_LEVEL FeatureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);
	D3D_FEATURE_LEVEL FeatureLevel;

	// Get the Right adapter
	std::vector <IDXGIAdapter1*> vAdapters;
	IDXGIAdapter1 *pAdapter;
	IDXGIFactory1 *pFactory = NULL;
	IDXGIOutput *pOutput;
	std::vector <IDXGIOutput*> vOutputs;

	// Create a DXGIFactory object.
	CR(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory));


	for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
		DXGI_ADAPTER_DESC1 desc1;
		pAdapter->GetDesc1(&desc1);

		for (UINT i = 0; pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND; i++) {
			DXGI_OUTPUT_DESC DesktopDesc;
			CR(pOutput->GetDesc(&DesktopDesc));
			vOutputs.push_back(pOutput);
		}

		if (desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;   // skip software adapters, can pass in /warp to command line if necessary
		}
		IDXGIAdapter *pAdapt = pAdapter;
		//D3D11CreateDevice(pAdapter, DriverTypes[i], nullptr, creationFlags, FeatureLevels, NumFeatureLevels,
		//D3D11_SDK_VERSION, &m_pDevice, &FeatureLevel, &m_pDeviceContext);
		D3D11CreateDevice(pAdapt, D3D_DRIVER_TYPE_UNKNOWN, nullptr, creationFlags, FeatureLevels, NumFeatureLevels, D3D11_SDK_VERSION, &m_pDevice, &FeatureLevel, &m_pDeviceContext);
		CNR(m_pDevice, R_SKIPPED);
		// Device was created so break
		break;
	}

	if (pFactory) {
		pFactory->Release();
		pFactory = nullptr;
	}

	IDXGIDevice* pDxgiDevice = nullptr;
	CRM(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice)), "Failed to QI for DXGI Device");

	IDXGIAdapter* pDxgiAdapter = nullptr;
	CRM(pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter)), "Failed to get parent DXGI Adapter");
	pDxgiDevice->Release();
	pDxgiDevice = nullptr;

	CRM(pDxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&m_pFactory)), "Failed to get parent DXGI Factory");
	pDxgiAdapter->Release();
	pDxgiAdapter = nullptr;

	// Register for occlusion status windows message
	CRM(m_pFactory->RegisterOcclusionStatusWindow(Window, OCCLUSION_STATUS_MSG, &m_OcclusionCookie), "Failed to register for occlusion message");

	// Get window size
	RECT WindowRect;
	GetClientRect(m_pWindowHandle, &WindowRect);
	UINT Width = WindowRect.right - WindowRect.left;
	UINT Height = WindowRect.bottom - WindowRect.top;

	// Create swapchain for window
	//*
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
	RtlZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.Width = Width;
	SwapChainDesc.Height = Height;
	SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	CRM(m_pFactory->CreateSwapChainForHwnd(m_pDevice, Window, &SwapChainDesc, nullptr, nullptr, &m_pSwapChain), "Failed to create window swapchain");

	// Disable the ALT-ENTER shortcut for entering full-screen mode
	CRM(m_pFactory->MakeWindowAssociation(Window, DXGI_MWA_NO_ALT_ENTER), "Failed to make window association");

	// Create shared texture
	DUPL_RETURN Return = CreateSharedSurf(outputToDuplicate, OutCount, DeskBounds);
	if (Return != DUPL_RETURN_SUCCESS) {
		return Return;
	}

	// Make new render target view
	Return = MakeRTV();
	if (Return != DUPL_RETURN_SUCCESS) {
		return Return;
	}

	// Set view port
	SetViewPort(Width, Height);

	// Create the sample state
	D3D11_SAMPLER_DESC SampDesc;
	RtlZeroMemory(&SampDesc, sizeof(SampDesc));
	SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SampDesc.MinLOD = 0;
	SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	CRM(m_pDevice->CreateSamplerState(&SampDesc, &m_pSamplerLinear), "Failed to create sampler state in D3D11DesktopDuplicationOutputManager");

	// Create the blend state
	D3D11_BLEND_DESC BlendStateDesc;
	BlendStateDesc.AlphaToCoverageEnable = FALSE;
	BlendStateDesc.IndependentBlendEnable = FALSE;
	BlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	CRM(m_pDevice->CreateBlendState(&BlendStateDesc, &m_pBlendState), "Failed to create blend state in D3D11DesktopDuplicationOutputManager");

	// Initialize shaders
	Return = InitShaders();
	if (Return != DUPL_RETURN_SUCCESS) {
		return Return;
	}

	GetWindowRect(m_pWindowHandle, &WindowRect);
	MoveWindow(m_pWindowHandle, WindowRect.left, WindowRect.top, (DeskBounds->right - DeskBounds->left) / 2, (DeskBounds->bottom - DeskBounds->top) / 2, TRUE);

Error:
	if (RFAILED()) {
		return ProcessFailure(nullptr, L"Failed to Init OutputManager", L"Error", r, nullptr);
	}
	return Return;
}

//
// Recreate shared texture
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::CreateSharedSurf(INT outputToDuplicate, _Out_ UINT* pOutCount, _Out_ RECT* pDeskBounds) {
	HRESULT r = S_OK;

	UINT OutputCount;
	IDXGIOutput* pDxgiOutput = nullptr;
	D3D11_TEXTURE2D_DESC DeskTexD;
	
	// Get DXGI resources
	IDXGIDevice* pDxgiDevice = nullptr;
	IDXGIAdapter* pDxgiAdapter = nullptr;
	CRM(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice)), "Failed to QI for DXGI Device");
	CRM(pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter)), "Failed to get parent DXGI Adapter");


	// Debugging- list all outputs
	/*
	std::vector <IDXGIAdapter*> vAdapters;
	IDXGIAdapter * pAdapter;
	IDXGIFactory1* pFactory = NULL;
	IDXGIOutput *pOutput;
	std::vector <IDXGIOutput*> vOutputs;
	// Create a DXGIFactory object.
	CRM(CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&pFactory), "Failed to get parent DXGI Adapter")

	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		vAdapters.push_back(pAdapter);
		DXGI_ADAPTER_DESC pDesc;
		CNR(vAdapters[i]->GetDesc(&pDesc), R_SKIPPED);

		for (UINT i = 0; pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
			vOutputs.push_back(pOutput);
			DXGI_OUTPUT_DESC DesktopDesc;
			pOutput->GetDesc(&DesktopDesc);
		}

	}
	//*/

	// Set initial values so that we always catch the right coordinates
	pDeskBounds->left = INT_MAX;
	pDeskBounds->right = INT_MIN;
	pDeskBounds->top = INT_MAX;
	pDeskBounds->bottom = INT_MIN;

	// Figure out right dimensions for full size desktop texture and # of outputs to duplicate
	if (outputToDuplicate < 0) {
		r = R_PASS;		// This could be better?
		for (OutputCount = 0; RSUCCESS(); ++OutputCount) {
			if (pDxgiOutput) {
				pDxgiOutput->Release();
				pDxgiOutput = nullptr;
			}
			r = (pDxgiAdapter->EnumOutputs(OutputCount, &pDxgiOutput));
			if (pDxgiOutput && (r != DXGI_ERROR_NOT_FOUND)) {
				DXGI_OUTPUT_DESC DesktopDesc;
				pDxgiOutput->GetDesc(&DesktopDesc);

				pDeskBounds->left = XMMin(DesktopDesc.DesktopCoordinates.left, pDeskBounds->left);
				pDeskBounds->top = XMMin(DesktopDesc.DesktopCoordinates.top, pDeskBounds->top);
				pDeskBounds->right = XMMax(DesktopDesc.DesktopCoordinates.right, pDeskBounds->right);
				pDeskBounds->bottom = XMMax(DesktopDesc.DesktopCoordinates.bottom, pDeskBounds->bottom);
			}
		}

		--OutputCount;
	}
	else {
		DXGI_ADAPTER_DESC pDesc;
		CR(pDxgiAdapter->GetDesc(&pDesc));
		CRM(pDxgiAdapter->EnumOutputs(outputToDuplicate, &pDxgiOutput), "Output specified to be duplicated does not exist");

		DXGI_OUTPUT_DESC DesktopDesc;
		pDxgiOutput->GetDesc(&DesktopDesc);
		*pDeskBounds = DesktopDesc.DesktopCoordinates;

		OutputCount = 1;
	}

	// Set passed in output count variable
	*pOutCount = OutputCount;

	if (OutputCount == 0) {
		// We could not find any outputs, the system must be in a transition so return expected error
		// so we will attempt to recreate
		return DUPL_RETURN_ERROR_EXPECTED;
	}

	// Create shared texture for all duplication threads to draw into	
	RtlZeroMemory(&DeskTexD, sizeof(D3D11_TEXTURE2D_DESC));
	DeskTexD.Width = pDeskBounds->right - pDeskBounds->left;
	DeskTexD.Height = pDeskBounds->bottom - pDeskBounds->top;
	DeskTexD.MipLevels = 1;
	DeskTexD.ArraySize = 1;
	DeskTexD.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	DeskTexD.SampleDesc.Count = 1;
	DeskTexD.Usage = D3D11_USAGE_DEFAULT;
	DeskTexD.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	DeskTexD.CPUAccessFlags = 0;
	DeskTexD.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	CR(m_pDevice->CreateTexture2D(&DeskTexD, nullptr, &m_pSharedSurf));

	// Get keyed mutex
	CRM(m_pSharedSurf->QueryInterface(__uuidof(IDXGIKeyedMutex), reinterpret_cast<void**>(&m_pKeyMutex)), "Failed to query for keyed mutex in D3D11DesktopDuplicationOutputManager");

Error:
	if (pDxgiOutput) {
		pDxgiOutput->Release();
		pDxgiOutput = nullptr;
	}
	if (pDxgiDevice) {
		pDxgiDevice->Release();
		pDxgiDevice = nullptr;
	}
	if (pDxgiAdapter) {
		pDxgiAdapter->Release();
		pDxgiAdapter = nullptr;
	}
	if (RFAILED()) {
		if (OutputCount != 1) {
			// If we are duplicating the complete desktop we try to create a single texture to hold the
			// complete desktop image and blitz updates from the per output DDA interface.  The GPU can
			// always support a texture size of the maximum resolution of any single output but there is no
			// guarantee that it can support a texture size of the desktop.
			// The sample only use this large texture to display the desktop image in a single window using DX
			// we could revert back to using GDI to update the window in this failure case.
			return ProcessFailure(m_pDevice, L"Failed to create DirectX shared texture - we are attempting to create a texture the size of the complete desktop and this may be larger than the maximum texture size of your GPU.  Please try again using the -output command line parameter to duplicate only 1 monitor or configure your computer to a single monitor configuration", L"Error", r, SystemTransitionsExpectedErrors);
		}
		else {
			return ProcessFailure(m_pDevice, L"Failed to create shared texture", L"Error", r, SystemTransitionsExpectedErrors);
		}
		return ProcessFailure(nullptr, L"Failed to CreateSharedSurf in OutputManager", L"Error", r);
	}
	return DUPL_RETURN_SUCCESS;
}

HRESULT D3D11DesktopDuplicationOutputManager::CopyToSendToDream(BYTE** pBuffer, UINT &pxWidth, UINT &pxHeight) {
	HRESULT r = S_OK;

	ID3D11Texture2D *pTempTexture = nullptr;
	ID3D11Texture2D* pTextureForDream = nullptr;
	D3D11_TEXTURE2D_DESC descTemp;
	D3D11_TEXTURE2D_DESC descDream;
	IDXGISurface *DreamSurface = nullptr;

	CR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTempTexture)));		// 0 is back buffer
	pTempTexture->GetDesc(&descTemp);

	// Make copy that we can access Data from
	pxWidth = descTemp.Width;
	pxHeight = descTemp.Height;

	descDream.Width = pxWidth;
	descDream.Height = pxHeight;
	descDream.MipLevels = 1;
	descDream.ArraySize = 1;
	descDream.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	descDream.SampleDesc.Count = 1;
	descDream.SampleDesc.Quality = 0;
	descDream.Usage = D3D11_USAGE_STAGING;
	descDream.BindFlags = 0;
	descDream.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	descDream.MiscFlags = 0;

	CR(m_pDevice->CreateTexture2D(&descDream, nullptr, &pTextureForDream));
	if (pxWidth > 0) {
		m_pDeviceContext->CopyResource(pTextureForDream, pTempTexture);

		pTextureForDream->QueryInterface(__uuidof(IDXGISurface), (void **)&DreamSurface);
		pTextureForDream->Release();
		pTextureForDream = nullptr;

		DXGI_MAPPED_RECT rectSurfaceForDream;
		CR(DreamSurface->Map(&rectSurfaceForDream, DXGI_MAP_READ));

		// Get Data into unsigned char* buffer
		*pBuffer = (BYTE*)malloc(pxWidth * pxHeight * 4);
		if (!(*pBuffer)) {
			return ProcessFailure(nullptr, L"Failed to allocate memory for new buffer for Dream.", L"Error", E_OUTOFMEMORY);
		}
		// memcpy(&pBuffer, rectSurfaceForDream.pBits, rectSurfaceForDream.Pitch);
		{
			UINT* pBuffer32 = reinterpret_cast<UINT*>(*pBuffer);
			UINT* Desktop32 = reinterpret_cast<UINT*>(rectSurfaceForDream.pBits);
			UINT  DesktopPitchInPixels = rectSurfaceForDream.Pitch / sizeof(UINT);

			for (INT Row = 0; Row < pxHeight; ++Row) {
				for (INT Col = 0; Col < pxWidth; ++Col) {
					UINT temp = (Desktop32[(Row * DesktopPitchInPixels) + Col]);
					pBuffer32[(Row * pxWidth) + Col] = temp;
				}
			}
		}

		// Done with resource
		CR(DreamSurface->Unmap());
		DreamSurface->Release();
		DreamSurface = nullptr;

		pTempTexture->Release();
		pTempTexture = nullptr;
	}

Error:
	if (DreamSurface != nullptr) {
		DreamSurface->Release();
		DreamSurface = nullptr;
	}

	if (pTempTexture != nullptr) {
		pTempTexture->Release();
		pTempTexture = nullptr;
	}
	return r;
}

//
// Present to the application window
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::UpdateApplicationWindow(_In_ PTR_INFO* PointerInfo, _Inout_ bool* Occluded, BYTE **pBuffer, UINT &pxWidth, UINT &pxHeight) {
	// In a typical desktop duplication application there would be an application running on one system collecting the desktop images
	// and another application running on a different system that receives the desktop images via a network and display the image. This
	// sample contains both these aspects into a single application.
	// This routine is the part of the sample that displays the desktop image onto the display

	// Try and acquire sync on common display buffer
	HRESULT hr = m_pKeyMutex->AcquireSync(1, 100);
	if (hr == static_cast<HRESULT>(WAIT_TIMEOUT)) {
		// Another thread has the keyed mutex so try again later
		return DUPL_RETURN_SUCCESS;
	}
	else if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to acquire Keyed mutex in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	CopyToSendToDream(pBuffer, pxWidth, pxHeight);

	// Got mutex, so draw
	DUPL_RETURN Ret = DrawFrame();
	/*	This Draws the Mouse, disabling for Now
	if (Ret == DUPL_RETURN_SUCCESS) {
		// We have keyed mutex so we can access the mouse info
		
		if (PointerInfo->Visible) {
		// Draw mouse into texture
		Ret = DrawMouse(PointerInfo);
		}
	}
	//*/

	// Release keyed mutex
	hr = m_pKeyMutex->ReleaseSync(0);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to Release Keyed mutex in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Present to window if all worked
	if (Ret == DUPL_RETURN_SUCCESS) {
		// Present to window
		hr = m_pSwapChain->Present(1, 0);
		if (FAILED(hr)) {
			return ProcessFailure(m_pDevice, L"Failed to present", L"Error", hr, SystemTransitionsExpectedErrors);
		}
		else if (hr == DXGI_STATUS_OCCLUDED) {
			*Occluded = true;
		}
	}

	return Ret;
}

//
// Returns shared handle
//
HANDLE D3D11DesktopDuplicationOutputManager::GetSharedHandle() {
	HRESULT r = S_OK;

	HANDLE Hnd = nullptr;
	// QI IDXGIResource interface to synchronized shared surface.
	IDXGIResource* pDXGIResource = nullptr;
	CR(m_pSharedSurf->QueryInterface(__uuidof(IDXGIResource), reinterpret_cast<void**>(&pDXGIResource)));
	
	// Obtain handle to IDXGIResource object.
	pDXGIResource->GetSharedHandle(&Hnd);
	pDXGIResource->Release();
	pDXGIResource = nullptr;
	
Error:
	return Hnd;
}

//
// Draw frame into backbuffer
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::DrawFrame() {
	HRESULT r = S_OK;

	// Create new shader resource view
	ID3D11ShaderResourceView* pShaderResourceView = nullptr;
	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;
	FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	D3D11_BUFFER_DESC initDataBufferDescription;
	D3D11_SUBRESOURCE_DATA subresourceInitData;
	ID3D11Buffer* pVertexBuffer = nullptr;

	// If window was resized, resize swapchain
	if (m_fNeedsResize) {
		DUPL_RETURN Ret = ResizeSwapChain();
		CB(Ret == DUPL_RETURN_SUCCESS);
		m_fNeedsResize = false;
	}

	// Vertices for drawing whole texture
	VERTEX Vertices[NUMVERTICES] = {
		{ XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_TEXTURE2D_DESC textureFrameFromSharedSurfaceDescription;
	m_pSharedSurf->GetDesc(&textureFrameFromSharedSurfaceDescription);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;
	shaderResourceViewDescription.Format = textureFrameFromSharedSurfaceDescription.Format;
	shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescription.Texture2D.MostDetailedMip = textureFrameFromSharedSurfaceDescription.MipLevels - 1;
	shaderResourceViewDescription.Texture2D.MipLevels = textureFrameFromSharedSurfaceDescription.MipLevels;	
	
	CRM(m_pDevice->CreateShaderResourceView(m_pSharedSurf, &shaderResourceViewDescription, &pShaderResourceView), "Failed to create shader resource when drawing a frame");
	
	// Set resources	
	m_pDeviceContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, nullptr);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetShaderResources(0, 1, &pShaderResourceView);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	RtlZeroMemory(&initDataBufferDescription, sizeof(initDataBufferDescription));
	initDataBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	initDataBufferDescription.ByteWidth = sizeof(VERTEX) * NUMVERTICES;
	initDataBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	initDataBufferDescription.CPUAccessFlags = 0;
	
	RtlZeroMemory(&subresourceInitData, sizeof(subresourceInitData));
	subresourceInitData.pSysMem = Vertices;

	// Create vertex buffer
	CRM(m_pDevice->CreateBuffer(&initDataBufferDescription, &subresourceInitData, &pVertexBuffer), "Failed to create vertex buffer when drawing a frame");

	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &Offset);
	
	// Draw textured quad onto render target
	m_pDeviceContext->Draw(NUMVERTICES, 0);	

Error:
	if (pShaderResourceView) {
		pShaderResourceView->Release();
		pShaderResourceView = nullptr;
	}
	if (pVertexBuffer) {
		pVertexBuffer->Release();
		pVertexBuffer = nullptr;
	}
	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed to DrawFrame in OutputManager", L"Error", r, SystemTransitionsExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Process both masked and monochrome pointers
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::ProcessMonoMask(bool IsMono, _Inout_ PTR_INFO* pPtrInfo, _Out_ INT* pPtrWidth, _Out_ INT* pPtrHeight, _Out_ INT* pPtrLeft, _Out_ INT* pPtrTop, _Outptr_result_bytebuffer_(*pPtrHeight * *pPtrWidth * BPP) BYTE** pInitBuffer, _Out_ D3D11_BOX* Box) {
	HRESULT r = S_OK;
	// Desktop dimensions
	D3D11_TEXTURE2D_DESC sharedSurfaceTextureDescription;
	m_pSharedSurf->GetDesc(&sharedSurfaceTextureDescription);
	INT DesktopWidth = sharedSurfaceTextureDescription.Width;
	INT DesktopHeight = sharedSurfaceTextureDescription.Height;

	// Pointer position
	INT GivenLeft = pPtrInfo->Position.x;
	INT GivenTop = pPtrInfo->Position.y;

	// Vars
	UINT* InitBuffer32;
	UINT* Desktop32;
	UINT  DesktopPitchInPixels;
	IDXGISurface* pCopySurface = nullptr;
	ID3D11Texture2D* pCopyBuffer = nullptr;
	DXGI_MAPPED_RECT MappedSurface;

	// Figure out if any adjustment is needed for out of bound positions
	if (GivenLeft < 0) {
		*pPtrWidth = GivenLeft + static_cast<INT>(pPtrInfo->ShapeInfo.Width);
	}
	else if ((GivenLeft + static_cast<INT>(pPtrInfo->ShapeInfo.Width)) > DesktopWidth) {
		*pPtrWidth = DesktopWidth - GivenLeft;
	}
	else {
		*pPtrWidth = static_cast<INT>(pPtrInfo->ShapeInfo.Width);
	}

	if (IsMono) {
		pPtrInfo->ShapeInfo.Height = pPtrInfo->ShapeInfo.Height / 2;
	}

	if (GivenTop < 0) {
		*pPtrHeight = GivenTop + static_cast<INT>(pPtrInfo->ShapeInfo.Height);
	}
	else if ((GivenTop + static_cast<INT>(pPtrInfo->ShapeInfo.Height)) > DesktopHeight) {
		*pPtrHeight = DesktopHeight - GivenTop;
	}
	else {
		*pPtrHeight = static_cast<INT>(pPtrInfo->ShapeInfo.Height);
	}

	if (IsMono) {
		pPtrInfo->ShapeInfo.Height = pPtrInfo->ShapeInfo.Height * 2;
	}

	*pPtrLeft = (GivenLeft < 0) ? 0 : GivenLeft;
	*pPtrTop = (GivenTop < 0) ? 0 : GivenTop;

	// What to skip (pixel offset)
	UINT SkipX = (GivenLeft < 0) ? (-1 * GivenLeft) : (0);
	UINT SkipY = (GivenTop < 0) ? (-1 * GivenTop) : (0);

	// Staging buffer/texture
	D3D11_TEXTURE2D_DESC CopyBufferDesc;
	CopyBufferDesc.Width = *pPtrWidth;
	CopyBufferDesc.Height = *pPtrHeight;
	CopyBufferDesc.MipLevels = 1;
	CopyBufferDesc.ArraySize = 1;
	CopyBufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	CopyBufferDesc.SampleDesc.Count = 1;
	CopyBufferDesc.SampleDesc.Quality = 0;
	CopyBufferDesc.Usage = D3D11_USAGE_STAGING;
	CopyBufferDesc.BindFlags = 0;
	CopyBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	CopyBufferDesc.MiscFlags = 0;

	CRM(m_pDevice->CreateTexture2D(&CopyBufferDesc, nullptr, &pCopyBuffer), "Failed creating staging texture for pointer");

	// Copy needed part of desktop image
	Box->left = *pPtrLeft;
	Box->top = *pPtrTop;
	Box->right = *pPtrLeft + *pPtrWidth;
	Box->bottom = *pPtrTop + *pPtrHeight;
	m_pDeviceContext->CopySubresourceRegion(pCopyBuffer, 0, 0, 0, 0, m_pSharedSurf, 0, Box);

	// QI for IDXGISurface
	CRM(pCopyBuffer->QueryInterface(__uuidof(IDXGISurface), (void **)&pCopySurface), "Failed to QI staging texture into IDXGISurface for pointer");

	// Map pixels	
	CRM(pCopySurface->Map(&MappedSurface, DXGI_MAP_READ), "Failed to map surface for pointer");

	// New mouseshape buffer
	*pInitBuffer = new (std::nothrow) BYTE[*pPtrWidth * *pPtrHeight * BPP];
	if (!(*pInitBuffer)) {
		return ProcessFailure(nullptr, L"Failed to allocate memory for new mouse shape buffer.", L"Error", E_OUTOFMEMORY);
	}

	InitBuffer32 = reinterpret_cast<UINT*>(*pInitBuffer);
	Desktop32 = reinterpret_cast<UINT*>(MappedSurface.pBits);
	DesktopPitchInPixels = MappedSurface.Pitch / sizeof(UINT);

	if (IsMono) {
		for (int Row = 0; Row < *pPtrHeight; ++Row) {
			// Set mask
			BYTE Mask = 0x80;
			Mask = Mask >> (SkipX % 8);
			for (int Col = 0; Col < *pPtrWidth; ++Col) {
				// Get masks using appropriate offsets
				BYTE AndMask = pPtrInfo->PtrShapeBuffer[((Col + SkipX) / 8) + ((Row + SkipY) * (pPtrInfo->ShapeInfo.Pitch))] & Mask;
				BYTE XorMask = pPtrInfo->PtrShapeBuffer[((Col + SkipX) / 8) + ((Row + SkipY + (pPtrInfo->ShapeInfo.Height / 2)) * (pPtrInfo->ShapeInfo.Pitch))] & Mask;
				UINT AndMask32 = (AndMask) ? 0xFFFFFFFF : 0xFF000000;
				UINT XorMask32 = (XorMask) ? 0x00FFFFFF : 0x00000000;

				// Set new pixel
				InitBuffer32[(Row * *pPtrWidth) + Col] = (Desktop32[(Row * DesktopPitchInPixels) + Col] & AndMask32) ^ XorMask32;
				// Adjust mask
				if (Mask == 0x01) {
					Mask = 0x80;
				}
				else {
					Mask = Mask >> 1;
				}
			}
		}
	}
	
	else {
		UINT* Buffer32 = reinterpret_cast<UINT*>(pPtrInfo->PtrShapeBuffer);

		// Iterate through pixels
		for (INT Row = 0; Row < *pPtrHeight; ++Row) {
			for (INT Col = 0; Col < *pPtrWidth; ++Col) {
				// Set up mask
				UINT MaskVal = 0xFF000000 & Buffer32[(Col + SkipX) + ((Row + SkipY) * (pPtrInfo->ShapeInfo.Pitch / sizeof(UINT)))];
				if (MaskVal) {
					// Mask was 0xFF
					InitBuffer32[(Row * *pPtrWidth) + Col] = (Desktop32[(Row * DesktopPitchInPixels) + Col] ^ Buffer32[(Col + SkipX) + ((Row + SkipY) * (pPtrInfo->ShapeInfo.Pitch / sizeof(UINT)))]) | 0xFF000000;
				}
				else {
					// Mask was 0x00
					InitBuffer32[(Row * *pPtrWidth) + Col] = Buffer32[(Col + SkipX) + ((Row + SkipY) * (pPtrInfo->ShapeInfo.Pitch / sizeof(UINT)))] | 0xFF000000;
				}
			}
		}
	}

	// Done with resource
	CRM(pCopySurface->Unmap(), "Failed to unmap surface for pointer");

Error:
	if (pCopyBuffer) {
		pCopyBuffer->Release();
		pCopyBuffer = nullptr;
	}
	if (pCopySurface) {
		pCopySurface->Release();
		pCopySurface = nullptr;
	}
	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed ProcessingMonoMask in OutputManager", L"Error", r, SystemTransitionsExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Draw mouse provided in buffer to backbuffer
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::DrawMouse(_In_ PTR_INFO* pPtrInfo) {
	HRESULT r = S_OK;
	
	// Vars to be used
	ID3D11Texture2D* pMouseTexture = nullptr;
	ID3D11ShaderResourceView* pShaderResourceView = nullptr;
	ID3D11Buffer* pVertexBufferMouse = nullptr;
	D3D11_SUBRESOURCE_DATA subresourceInitData;
	D3D11_TEXTURE2D_DESC copyTextureDescription;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;
	D3D11_BUFFER_DESC bufferDescription;
	FLOAT BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;

	// Position will be changed based on mouse position
	VERTEX Vertices[NUMVERTICES] = {
		{ XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_TEXTURE2D_DESC sharedSurfaceTextureDescription;
	m_pSharedSurf->GetDesc(&sharedSurfaceTextureDescription);
	INT DesktopWidth = sharedSurfaceTextureDescription.Width;
	INT DesktopHeight = sharedSurfaceTextureDescription.Height;

	// Center of desktop dimensions
	INT CenterX = (DesktopWidth / 2);
	INT CenterY = (DesktopHeight / 2);

	// Clipping adjusted coordinates / dimensions
	INT PtrWidth = 0;
	INT PtrHeight = 0;
	INT PtrLeft = 0;
	INT PtrTop = 0;

	// Buffer used if necessary (in case of monochrome or masked pointer)
	BYTE* InitBuffer = nullptr;

	// Used for copying pixels
	D3D11_BOX Box;
	Box.front = 0;
	Box.back = 1;

	copyTextureDescription.MipLevels = 1;
	copyTextureDescription.ArraySize = 1;
	copyTextureDescription.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	copyTextureDescription.SampleDesc.Count = 1;
	copyTextureDescription.SampleDesc.Quality = 0;
	copyTextureDescription.Usage = D3D11_USAGE_DEFAULT;
	copyTextureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	copyTextureDescription.CPUAccessFlags = 0;
	copyTextureDescription.MiscFlags = 0;

	// Set shader resource properties
	shaderResourceViewDescription.Format = copyTextureDescription.Format;
	shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescription.Texture2D.MostDetailedMip = copyTextureDescription.MipLevels - 1;
	shaderResourceViewDescription.Texture2D.MipLevels = copyTextureDescription.MipLevels;

	switch (pPtrInfo->ShapeInfo.Type) {
	case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR: {
		PtrLeft = pPtrInfo->Position.x;
		PtrTop = pPtrInfo->Position.y;

		PtrWidth = static_cast<INT>(pPtrInfo->ShapeInfo.Width);
		PtrHeight = static_cast<INT>(pPtrInfo->ShapeInfo.Height);

		break;
	}

	case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME: {
		ProcessMonoMask(true, pPtrInfo, &PtrWidth, &PtrHeight, &PtrLeft, &PtrTop, &InitBuffer, &Box);
		break;
	}

	case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR: {
		ProcessMonoMask(false, pPtrInfo, &PtrWidth, &PtrHeight, &PtrLeft, &PtrTop, &InitBuffer, &Box);
		break;
	}

	default:
		break;
	}

	// VERTEX creation
	Vertices[0].Pos.x = (PtrLeft - CenterX) / (FLOAT)CenterX;
	Vertices[0].Pos.y = -1 * ((PtrTop + PtrHeight) - CenterY) / (FLOAT)CenterY;
	Vertices[1].Pos.x = (PtrLeft - CenterX) / (FLOAT)CenterX;
	Vertices[1].Pos.y = -1 * (PtrTop - CenterY) / (FLOAT)CenterY;
	Vertices[2].Pos.x = ((PtrLeft + PtrWidth) - CenterX) / (FLOAT)CenterX;
	Vertices[2].Pos.y = -1 * ((PtrTop + PtrHeight) - CenterY) / (FLOAT)CenterY;
	Vertices[3].Pos.x = Vertices[2].Pos.x;
	Vertices[3].Pos.y = Vertices[2].Pos.y;
	Vertices[4].Pos.x = Vertices[1].Pos.x;
	Vertices[4].Pos.y = Vertices[1].Pos.y;
	Vertices[5].Pos.x = ((PtrLeft + PtrWidth) - CenterX) / (FLOAT)CenterX;
	Vertices[5].Pos.y = -1 * (PtrTop - CenterY) / (FLOAT)CenterY;

	// Set texture properties
	copyTextureDescription.Width = PtrWidth;
	copyTextureDescription.Height = PtrHeight;

	// Set up init data
	subresourceInitData.pSysMem = (pPtrInfo->ShapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR) ? pPtrInfo->PtrShapeBuffer : InitBuffer;
	subresourceInitData.SysMemPitch = (pPtrInfo->ShapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR) ? pPtrInfo->ShapeInfo.Pitch : PtrWidth * BPP;
	subresourceInitData.SysMemSlicePitch = 0;

	// Create mouseshape as texture
	CRM(m_pDevice->CreateTexture2D(&copyTextureDescription, &subresourceInitData, &pMouseTexture), "Failed to create mouse pointer texture");

	// Create shader resource from texture
	CRM(m_pDevice->CreateShaderResourceView(pMouseTexture, &shaderResourceViewDescription, &pShaderResourceView), "Failed to create shader resource from mouse pointer texture");

	ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth = sizeof(VERTEX) * NUMVERTICES;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;

	ZeroMemory(&subresourceInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	subresourceInitData.pSysMem = Vertices;

	// Create vertex buffer
	CRM(m_pDevice->CreateBuffer(&bufferDescription, &subresourceInitData, &pVertexBufferMouse), "Failed to create mouse pointer vertex buffer in OutputManager");

	// Set resources	
	m_pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBufferMouse, &Stride, &Offset);
	m_pDeviceContext->OMSetBlendState(m_pBlendState, BlendFactor, 0xFFFFFFFF);
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, nullptr);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetShaderResources(0, 1, &pShaderResourceView);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

	// Draw
	m_pDeviceContext->Draw(NUMVERTICES, 0);

Error:
	// Clean
	if (pVertexBufferMouse) {
		pVertexBufferMouse->Release();
		pVertexBufferMouse = nullptr;
	}
	if (pShaderResourceView) {
		pShaderResourceView->Release();
		pShaderResourceView = nullptr;
	}
	if (pMouseTexture) {
		pMouseTexture->Release();
		pMouseTexture = nullptr;
	}
	if (InitBuffer)	{
		delete[] InitBuffer;
		InitBuffer = nullptr;
	}

	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed to create DrawMouse in OutputManager", L"Error", r, SystemTransitionsExpectedErrors);
	}

	return DUPL_RETURN_SUCCESS;
}

//
// Initialize shaders for drawing to screen
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::InitShaders() {
	HRESULT r = S_OK;

	UINT Size = ARRAYSIZE(g_VS);
	D3D11_INPUT_ELEMENT_DESC Layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT NumElements = ARRAYSIZE(Layout);

	CRM(m_pDevice->CreateVertexShader(g_VS, Size, nullptr, &m_pVertexShader), "Failed to create vertex shader in D3D11DesktopDuplicationOutputManager");
	CRM(m_pDevice->CreateInputLayout(Layout, NumElements, g_VS, Size, &m_pInputLayout), "Failed to create input layout in D3D11DesktopDuplicationOutputManager");

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	Size = ARRAYSIZE(g_PS);
	CRM(m_pDevice->CreatePixelShader(g_PS, Size, nullptr, &m_pPixelShader), "Failed to create pixel shader in D3D11DesktopDuplicationOutputManager");

Error:
	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed to Init shader in D3D11DesktopDuplicationOutputManager", L"Error", r, SystemTransitionsExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Reset render target view
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::MakeRTV() {
	HRESULT r = S_OK;
	
	// Get backbuffer
	ID3D11Texture2D* BackBuffer = nullptr;
	CRM(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer)), "Failed to get backbuffer for making render target view in D3D11DesktopDuplicationOutputManager");

	// Create a render target view
	CRM(m_pDevice->CreateRenderTargetView(BackBuffer, nullptr, &m_pRTV), "Failed to create render target view in D3D11DesktopDuplicationOutputManager");

	// Set new render target
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, nullptr);

Error:
	BackBuffer->Release();
	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed MakeRTV in D3D11DesktopDuplicationOutputManager", L"Error", r, SystemTransitionsExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Set new viewport
//
void D3D11DesktopDuplicationOutputManager::SetViewPort(UINT Width, UINT Height) {
	D3D11_VIEWPORT VP;
	VP.Width = static_cast<FLOAT>(Width);
	VP.Height = static_cast<FLOAT>(Height);
	VP.MinDepth = 0.0f;
	VP.MaxDepth = 1.0f;
	VP.TopLeftX = 0;
	VP.TopLeftY = 0;
	m_pDeviceContext->RSSetViewports(1, &VP);
}

//
// Resize swapchain
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::ResizeSwapChain() {
	HRESULT r = S_OK;
	DUPL_RETURN Ret;

	if (m_pRTV) {
		m_pRTV->Release();
		m_pRTV = nullptr;
	}

	RECT WindowRect;
	GetClientRect(m_pWindowHandle, &WindowRect);
	UINT Width = WindowRect.right - WindowRect.left;
	UINT Height = WindowRect.bottom - WindowRect.top;

	// Resize swapchain
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	m_pSwapChain->GetDesc(&SwapChainDesc);
	CRM(m_pSwapChain->ResizeBuffers(SwapChainDesc.BufferCount, Width, Height, SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags), "Failed to resize swapchain buffers in D3D11DesktopDuplicationOutputManager");

	// Make new render target view
	Ret = MakeRTV();
	if (Ret != DUPL_RETURN_SUCCESS) {
		return Ret;
	}

	// Set new viewport
	SetViewPort(Width, Height);

Error:
	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed to resize swapchain buffers in D3D11DesktopDuplicationOutputManager", L"Error", r, SystemTransitionsExpectedErrors);
	}
	return Ret;
}

//
// Releases all references
//
void D3D11DesktopDuplicationOutputManager::CleanRefs() {
	if (m_pVertexShader) {
		m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}

	if (m_pPixelShader) {
		m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}

	if (m_pInputLayout) {
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}

	if (m_pRTV) {
		m_pRTV->Release();
		m_pRTV = nullptr;
	}

	if (m_pSamplerLinear) {
		m_pSamplerLinear->Release();
		m_pSamplerLinear = nullptr;
	}

	if (m_pBlendState) {
		m_pBlendState->Release();
		m_pBlendState = nullptr;
	}

	if (m_pDeviceContext) {
		m_pDeviceContext->Release();
		m_pDeviceContext = nullptr;
	}

	if (m_pDevice) {
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	if (m_pSwapChain) {
		m_pSwapChain->Release();
		m_pSwapChain = nullptr;
	}

	if (m_pSharedSurf) {
		m_pSharedSurf->Release();
		m_pSharedSurf = nullptr;
	}

	if (m_pKeyMutex) {
		m_pKeyMutex->Release();
		m_pKeyMutex = nullptr;
	}

	if (m_pFactory)	{
		if (m_OcclusionCookie) {
			m_pFactory->UnregisterOcclusionStatus(m_OcclusionCookie);
			m_OcclusionCookie = 0;
		}
		m_pFactory->Release();
		m_pFactory = nullptr;
	}
}
