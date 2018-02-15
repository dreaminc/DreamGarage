#include "D3D11DesktopDuplicationOutputManager.h"
#include <vector>
using namespace DirectX;

//
// Constructor NULLs out all pointers & sets appropriate var vals
//
D3D11DesktopDuplicationOutputManager::D3D11DesktopDuplicationOutputManager() {
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
DUPL_RETURN D3D11DesktopDuplicationOutputManager::InitOutput(HWND Window, INT SingleOutput, _Out_ UINT* OutCount, _Out_ RECT* DeskBounds) {
	HRESULT hr;

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
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory))) {
		// Should handle this somehow   
	}

	for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
		DXGI_ADAPTER_DESC1 desc1;
		pAdapter->GetDesc1(&desc1);

		for (UINT i = 0; pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND; i++) {
			DXGI_OUTPUT_DESC DesktopDesc;
			hr = pOutput->GetDesc(&DesktopDesc);
			if (SUCCEEDED(hr)) {
				vOutputs.push_back(pOutput);
			}
		}

		if (desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;   // skip software adapters, can pass in /warp to command line if necessary
		}
		IDXGIAdapter *pAdapt = pAdapter;
		//hr = D3D11CreateDevice(pAdapter, DriverTypes[i], nullptr, creationFlags, FeatureLevels, NumFeatureLevels,
		//  D3D11_SDK_VERSION, &m_pDevice, &FeatureLevel, &m_pDeviceContext);
		hr = D3D11CreateDevice(pAdapt, D3D_DRIVER_TYPE_UNKNOWN, nullptr, creationFlags, FeatureLevels, NumFeatureLevels, D3D11_SDK_VERSION, &m_pDevice, &FeatureLevel, &m_pDeviceContext);
		if (SUCCEEDED(hr)) {
			// Device creation succeeded, no need to loop anymore
			break;
		}
	}

	if (pFactory) {
		pFactory->Release();
		pFactory = nullptr;
	}

	/* Create device
	for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
	{
	hr = D3D11CreateDevice(pAdapter, DriverTypes[DriverTypeIndex], nullptr, creationFlags, FeatureLevels, NumFeatureLevels,
	D3D11_SDK_VERSION, &m_pDevice, &FeatureLevel, &m_pDeviceContext);
	if (SUCCEEDED(hr))
	{
	// Device creation succeeded, no need to loop anymore
	break;
	}
	}
	if (FAILED(hr))
	{
	return ProcessFailure(m_pDevice, L"Device creation in D3D11DesktopDuplicationOutputManager failed", L"Error", hr, SystemTransitionsExpectedErrors);
	}
	//*/
	// Get DXGI factory
	IDXGIDevice* DxgiDevice = nullptr;
	hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr)) {
		return ProcessFailure(nullptr, L"Failed to QI for DXGI Device", L"Error", hr, nullptr);
	}

	IDXGIAdapter* DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	DxgiDevice->Release();
	DxgiDevice = nullptr;
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to get parent DXGI Adapter", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	hr = DxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&m_pFactory));
	DxgiAdapter->Release();
	DxgiAdapter = nullptr;
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to get parent DXGI Factory", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Register for occlusion status windows message
	hr = m_pFactory->RegisterOcclusionStatusWindow(Window, OCCLUSION_STATUS_MSG, &m_OcclusionCookie);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to register for occlusion message", L"Error", hr, SystemTransitionsExpectedErrors);
	}

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
	hr = m_pFactory->CreateSwapChainForHwnd(m_pDevice, Window, &SwapChainDesc, nullptr, nullptr, &m_pSwapChain);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create window swapchain", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Disable the ALT-ENTER shortcut for entering full-screen mode
	hr = m_pFactory->MakeWindowAssociation(Window, DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to make window association", L"Error", hr, SystemTransitionsExpectedErrors);
	}
	//*/
	// Create shared texture
	DUPL_RETURN Return = CreateSharedSurf(SingleOutput, OutCount, DeskBounds);
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
	hr = m_pDevice->CreateSamplerState(&SampDesc, &m_pSamplerLinear);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create sampler state in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

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
	hr = m_pDevice->CreateBlendState(&BlendStateDesc, &m_pBlendState);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create blend state in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Initialize shaders
	Return = InitShaders();
	if (Return != DUPL_RETURN_SUCCESS) {
		return Return;
	}

	GetWindowRect(m_pWindowHandle, &WindowRect);
	MoveWindow(m_pWindowHandle, WindowRect.left, WindowRect.top, (DeskBounds->right - DeskBounds->left) / 2, (DeskBounds->bottom - DeskBounds->top) / 2, TRUE);

	return Return;
}

//
// Recreate shared texture
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::CreateSharedSurf(INT SingleOutput, _Out_ UINT* OutCount, _Out_ RECT* DeskBounds) {
	HRESULT hr;

	// Get DXGI resources
	IDXGIDevice* DxgiDevice = nullptr;
	hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr)) {
		return ProcessFailure(nullptr, L"Failed to QI for DXGI Device", L"Error", hr);
	}

	IDXGIAdapter* DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	DxgiDevice->Release();
	DxgiDevice = nullptr;
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to get parent DXGI Adapter", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Debugging- list all outputs
	//*
	std::vector <IDXGIAdapter*> vAdapters;
	IDXGIAdapter * pAdapter;
	IDXGIFactory1* pFactory = NULL;
	IDXGIOutput *pOutput;
	std::vector <IDXGIOutput*> vOutputs;
	// Create a DXGIFactory object.
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&pFactory))) {
		return ProcessFailure(m_pDevice, L"Failed to get parent DXGI Adapter", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		vAdapters.push_back(pAdapter);
		DXGI_ADAPTER_DESC pDesc;
		hr = vAdapters[i]->GetDesc(&pDesc);

		for (UINT i = 0; pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND; ++i) {
			vOutputs.push_back(pOutput);
			DXGI_OUTPUT_DESC DesktopDesc;
			pOutput->GetDesc(&DesktopDesc);
		}

	}
	//*/

	// Set initial values so that we always catch the right coordinates
	DeskBounds->left = INT_MAX;
	DeskBounds->right = INT_MIN;
	DeskBounds->top = INT_MAX;
	DeskBounds->bottom = INT_MIN;

	IDXGIOutput* DxgiOutput = nullptr;

	// Figure out right dimensions for full size desktop texture and # of outputs to duplicate
	UINT OutputCount;
	if (SingleOutput < 0) {
		hr = S_OK;
		for (OutputCount = 0; SUCCEEDED(hr); ++OutputCount) {
			if (DxgiOutput) {
				DxgiOutput->Release();
				DxgiOutput = nullptr;
			}
			hr = DxgiAdapter->EnumOutputs(OutputCount, &DxgiOutput);
			if (DxgiOutput && (hr != DXGI_ERROR_NOT_FOUND)) {
				DXGI_OUTPUT_DESC DesktopDesc;
				DxgiOutput->GetDesc(&DesktopDesc);

				DeskBounds->left = XMMin(DesktopDesc.DesktopCoordinates.left, DeskBounds->left);
				DeskBounds->top = XMMin(DesktopDesc.DesktopCoordinates.top, DeskBounds->top);
				DeskBounds->right = XMMax(DesktopDesc.DesktopCoordinates.right, DeskBounds->right);
				DeskBounds->bottom = XMMax(DesktopDesc.DesktopCoordinates.bottom, DeskBounds->bottom);
			}
		}

		--OutputCount;
	}
	else {
		DXGI_ADAPTER_DESC pDesc;
		hr = DxgiAdapter->GetDesc(&pDesc);

		//hr = DxgiAdapter->EnumOutputs(SingleOutput, &DxgiOutput);
		hr = DxgiAdapter->EnumOutputs(1, &DxgiOutput);
		if (FAILED(hr)) {
			DxgiAdapter->Release();
			DxgiAdapter = nullptr;
			return ProcessFailure(m_pDevice, L"Output specified to be duplicated does not exist", L"Error", hr);
		}
		DXGI_OUTPUT_DESC DesktopDesc;
		DxgiOutput->GetDesc(&DesktopDesc);
		*DeskBounds = DesktopDesc.DesktopCoordinates;

		DxgiOutput->Release();
		DxgiOutput = nullptr;

		OutputCount = 1;
	}

	DxgiAdapter->Release();
	DxgiAdapter = nullptr;

	// Set passed in output count variable
	*OutCount = OutputCount;

	if (OutputCount == 0) {
		// We could not find any outputs, the system must be in a transition so return expected error
		// so we will attempt to recreate
		return DUPL_RETURN_ERROR_EXPECTED;
	}

	// Create shared texture for all duplication threads to draw into
	D3D11_TEXTURE2D_DESC DeskTexD;
	RtlZeroMemory(&DeskTexD, sizeof(D3D11_TEXTURE2D_DESC));
	DeskTexD.Width = DeskBounds->right - DeskBounds->left;
	DeskTexD.Height = DeskBounds->bottom - DeskBounds->top;
	DeskTexD.MipLevels = 1;
	DeskTexD.ArraySize = 1;
	DeskTexD.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	DeskTexD.SampleDesc.Count = 1;
	DeskTexD.Usage = D3D11_USAGE_DEFAULT;
	DeskTexD.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	DeskTexD.CPUAccessFlags = 0;
	DeskTexD.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = m_pDevice->CreateTexture2D(&DeskTexD, nullptr, &m_pSharedSurf);
	if (FAILED(hr)) {
		if (OutputCount != 1) {
			// If we are duplicating the complete desktop we try to create a single texture to hold the
			// complete desktop image and blitz updates from the per output DDA interface.  The GPU can
			// always support a texture size of the maximum resolution of any single output but there is no
			// guarantee that it can support a texture size of the desktop.
			// The sample only use this large texture to display the desktop image in a single window using DX
			// we could revert back to using GDI to update the window in this failure case.
			return ProcessFailure(m_pDevice, L"Failed to create DirectX shared texture - we are attempting to create a texture the size of the complete desktop and this may be larger than the maximum texture size of your GPU.  Please try again using the -output command line parameter to duplicate only 1 monitor or configure your computer to a single monitor configuration", L"Error", hr, SystemTransitionsExpectedErrors);
		}
		else {
			return ProcessFailure(m_pDevice, L"Failed to create shared texture", L"Error", hr, SystemTransitionsExpectedErrors);
		}
	}

	// Get keyed mutex
	hr = m_pSharedSurf->QueryInterface(__uuidof(IDXGIKeyedMutex), reinterpret_cast<void**>(&m_pKeyMutex));
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to query for keyed mutex in D3D11DesktopDuplicationOutputManager", L"Error", hr);
	}

	return DUPL_RETURN_SUCCESS;
}

DUPL_RETURN D3D11DesktopDuplicationOutputManager::CopyToSendToDream(BYTE** pBuffer) {
	//*
	ID3D11Texture2D *pTempTexture = nullptr;
	D3D11_TEXTURE2D_DESC descTemp;

	HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTempTexture));		// 0 is back buffer
	pTempTexture->GetDesc(&descTemp);

	// Make copy that we can access Data from
	UINT pxWidth = descTemp.Width;
	UINT pxHeight = descTemp.Height;

	D3D11_TEXTURE2D_DESC descDream;
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

	ID3D11Texture2D* pTextureForDream = nullptr;
	hr = m_pDevice->CreateTexture2D(&descDream, nullptr, &pTextureForDream);
	if (SUCCEEDED(hr) && pxWidth != 800) {
		m_pDeviceContext->CopyResource(pTextureForDream, pTempTexture);

		IDXGISurface *DreamSurface = nullptr;
		pTextureForDream->QueryInterface(__uuidof(IDXGISurface), (void **)&DreamSurface);
		pTextureForDream->Release();
		pTextureForDream = nullptr;

		DXGI_MAPPED_RECT rectSurfaceForDream;
		hr = DreamSurface->Map(&rectSurfaceForDream, DXGI_MAP_READ);
		if (FAILED(hr)) {
			DreamSurface->Release();
			DreamSurface = nullptr;
		}

		// Get Data into unsigned char* buffer
		*pBuffer = (BYTE*)malloc(pxWidth * pxHeight * 4);
		if (!(*pBuffer)) {
			return ProcessFailure(nullptr, L"Failed to allocate memory for new buffer for Dream.", L"Error", E_OUTOFMEMORY);
		}
		// memcpy(&pBuffer, rectSurfaceForDream.pBits, rectSurfaceForDream.Pitch);

		UINT* pBuffer32 = reinterpret_cast<UINT*>(*pBuffer);
		UINT* Desktop32 = reinterpret_cast<UINT*>(rectSurfaceForDream.pBits);
		UINT  DesktopPitchInPixels = rectSurfaceForDream.Pitch / sizeof(UINT);

		for (INT Row = 0; Row < pxHeight; ++Row) {
			for (INT Col = 0; Col < pxWidth; ++Col) {
				UINT temp = (Desktop32[(Row * DesktopPitchInPixels) + Col]);
				pBuffer32[(Row * pxWidth) + Col] = temp;
			}
		}

		// Done with resource
		hr = DreamSurface->Unmap();
		DreamSurface->Release();
		DreamSurface = nullptr;

		pTempTexture->Release();
		pTempTexture = nullptr;

	}

	if (pTempTexture != nullptr) {
		pTempTexture->Release();
		pTempTexture = nullptr;
	}
}
//*/

//
// Present to the application window
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::UpdateApplicationWindow(_In_ PTR_INFO* PointerInfo, _Inout_ bool* Occluded, BYTE **pBuffer) {
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

	CopyToSendToDream(pBuffer);

	// Got mutex, so draw
	DUPL_RETURN Ret = DrawFrame();
	if (Ret == DUPL_RETURN_SUCCESS) {
		// We have keyed mutex so we can access the mouse info
		/*	This Draws the Mouse, disabling for Now
		if (PointerInfo->Visible) {
			// Draw mouse into texture
			Ret = DrawMouse(PointerInfo);
		}
		//*/
	}	

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
	HANDLE Hnd = nullptr;

	// QI IDXGIResource interface to synchronized shared surface.
	IDXGIResource* DXGIResource = nullptr;
	HRESULT hr = m_pSharedSurf->QueryInterface(__uuidof(IDXGIResource), reinterpret_cast<void**>(&DXGIResource));
	if (SUCCEEDED(hr)) {
		// Obtain handle to IDXGIResource object.
		DXGIResource->GetSharedHandle(&Hnd);
		DXGIResource->Release();
		DXGIResource = nullptr;
	}

	return Hnd;
}

//
// Draw frame into backbuffer
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::DrawFrame() {
	HRESULT hr;

	// If window was resized, resize swapchain
	if (m_fNeedsResize) {
		DUPL_RETURN Ret = ResizeSwapChain();
		if (Ret != DUPL_RETURN_SUCCESS) {
			return Ret;
		}
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

	D3D11_TEXTURE2D_DESC FrameDesc;
	m_pSharedSurf->GetDesc(&FrameDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
	ShaderDesc.Format = FrameDesc.Format;
	ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderDesc.Texture2D.MostDetailedMip = FrameDesc.MipLevels - 1;
	ShaderDesc.Texture2D.MipLevels = FrameDesc.MipLevels;

	// Create new shader resource view
	ID3D11ShaderResourceView* ShaderResource = nullptr;
	hr = m_pDevice->CreateShaderResourceView(m_pSharedSurf, &ShaderDesc, &ShaderResource);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create shader resource when drawing a frame", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Set resources
	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;
	FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_pDeviceContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, nullptr);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetShaderResources(0, 1, &ShaderResource);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_BUFFER_DESC BufferDesc;
	RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(VERTEX) * NUMVERTICES;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	RtlZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Vertices;

	ID3D11Buffer* VertexBuffer = nullptr;

	// Create vertex buffer
	hr = m_pDevice->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
	if (FAILED(hr)) {
		ShaderResource->Release();
		ShaderResource = nullptr;
		return ProcessFailure(m_pDevice, L"Failed to create vertex buffer when drawing a frame", L"Error", hr, SystemTransitionsExpectedErrors);
	}
	m_pDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
	
	// Draw textured quad onto render target
	m_pDeviceContext->Draw(NUMVERTICES, 0);

	VertexBuffer->Release();
	VertexBuffer = nullptr;

	// Release shader resource
	ShaderResource->Release();
	ShaderResource = nullptr;

	return DUPL_RETURN_SUCCESS;
}

//
// Process both masked and monochrome pointers
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::ProcessMonoMask(bool IsMono, _Inout_ PTR_INFO* PtrInfo, _Out_ INT* PtrWidth, _Out_ INT* PtrHeight, _Out_ INT* PtrLeft, _Out_ INT* PtrTop, _Outptr_result_bytebuffer_(*PtrHeight * *PtrWidth * BPP) BYTE** InitBuffer, _Out_ D3D11_BOX* Box) {
	// Desktop dimensions
	D3D11_TEXTURE2D_DESC FullDesc;
	m_pSharedSurf->GetDesc(&FullDesc);
	INT DesktopWidth = FullDesc.Width;
	INT DesktopHeight = FullDesc.Height;

	// Pointer position
	INT GivenLeft = PtrInfo->Position.x;
	INT GivenTop = PtrInfo->Position.y;

	// Figure out if any adjustment is needed for out of bound positions
	if (GivenLeft < 0) {
		*PtrWidth = GivenLeft + static_cast<INT>(PtrInfo->ShapeInfo.Width);
	}
	else if ((GivenLeft + static_cast<INT>(PtrInfo->ShapeInfo.Width)) > DesktopWidth) {
		*PtrWidth = DesktopWidth - GivenLeft;
	}
	else {
		*PtrWidth = static_cast<INT>(PtrInfo->ShapeInfo.Width);
	}

	if (IsMono) {
		PtrInfo->ShapeInfo.Height = PtrInfo->ShapeInfo.Height / 2;
	}

	if (GivenTop < 0) {
		*PtrHeight = GivenTop + static_cast<INT>(PtrInfo->ShapeInfo.Height);
	}
	else if ((GivenTop + static_cast<INT>(PtrInfo->ShapeInfo.Height)) > DesktopHeight) {
		*PtrHeight = DesktopHeight - GivenTop;
	}
	else {
		*PtrHeight = static_cast<INT>(PtrInfo->ShapeInfo.Height);
	}

	if (IsMono) {
		PtrInfo->ShapeInfo.Height = PtrInfo->ShapeInfo.Height * 2;
	}

	*PtrLeft = (GivenLeft < 0) ? 0 : GivenLeft;
	*PtrTop = (GivenTop < 0) ? 0 : GivenTop;

	// Staging buffer/texture
	D3D11_TEXTURE2D_DESC CopyBufferDesc;
	CopyBufferDesc.Width = *PtrWidth;
	CopyBufferDesc.Height = *PtrHeight;
	CopyBufferDesc.MipLevels = 1;
	CopyBufferDesc.ArraySize = 1;
	CopyBufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	CopyBufferDesc.SampleDesc.Count = 1;
	CopyBufferDesc.SampleDesc.Quality = 0;
	CopyBufferDesc.Usage = D3D11_USAGE_STAGING;
	CopyBufferDesc.BindFlags = 0;
	CopyBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	CopyBufferDesc.MiscFlags = 0;

	ID3D11Texture2D* CopyBuffer = nullptr;
	HRESULT hr = m_pDevice->CreateTexture2D(&CopyBufferDesc, nullptr, &CopyBuffer);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed creating staging texture for pointer", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Copy needed part of desktop image
	Box->left = *PtrLeft;
	Box->top = *PtrTop;
	Box->right = *PtrLeft + *PtrWidth;
	Box->bottom = *PtrTop + *PtrHeight;
	m_pDeviceContext->CopySubresourceRegion(CopyBuffer, 0, 0, 0, 0, m_pSharedSurf, 0, Box);

	// QI for IDXGISurface
	IDXGISurface* CopySurface = nullptr;
	hr = CopyBuffer->QueryInterface(__uuidof(IDXGISurface), (void **)&CopySurface);
	CopyBuffer->Release();
	CopyBuffer = nullptr;
	if (FAILED(hr)) {
		return ProcessFailure(nullptr, L"Failed to QI staging texture into IDXGISurface for pointer", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Map pixels
	DXGI_MAPPED_RECT MappedSurface;
	hr = CopySurface->Map(&MappedSurface, DXGI_MAP_READ);
	if (FAILED(hr)) {
		CopySurface->Release();
		CopySurface = nullptr;
		return ProcessFailure(m_pDevice, L"Failed to map surface for pointer", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// New mouseshape buffer
	*InitBuffer = new (std::nothrow) BYTE[*PtrWidth * *PtrHeight * BPP];
	if (!(*InitBuffer)) {
		return ProcessFailure(nullptr, L"Failed to allocate memory for new mouse shape buffer.", L"Error", E_OUTOFMEMORY);
	}

	UINT* InitBuffer32 = reinterpret_cast<UINT*>(*InitBuffer);
	UINT* Desktop32 = reinterpret_cast<UINT*>(MappedSurface.pBits);
	UINT  DesktopPitchInPixels = MappedSurface.Pitch / sizeof(UINT);

	// What to skip (pixel offset)
	UINT SkipX = (GivenLeft < 0) ? (-1 * GivenLeft) : (0);
	UINT SkipY = (GivenTop < 0) ? (-1 * GivenTop) : (0);

	if (IsMono) {
		for (int Row = 0; Row < *PtrHeight; ++Row) {
			// Set mask
			BYTE Mask = 0x80;
			Mask = Mask >> (SkipX % 8);
			for (int Col = 0; Col < *PtrWidth; ++Col) {
				// Get masks using appropriate offsets
				BYTE AndMask = PtrInfo->PtrShapeBuffer[((Col + SkipX) / 8) + ((Row + SkipY) * (PtrInfo->ShapeInfo.Pitch))] & Mask;
				BYTE XorMask = PtrInfo->PtrShapeBuffer[((Col + SkipX) / 8) + ((Row + SkipY + (PtrInfo->ShapeInfo.Height / 2)) * (PtrInfo->ShapeInfo.Pitch))] & Mask;
				UINT AndMask32 = (AndMask) ? 0xFFFFFFFF : 0xFF000000;
				UINT XorMask32 = (XorMask) ? 0x00FFFFFF : 0x00000000;

				// Set new pixel
				InitBuffer32[(Row * *PtrWidth) + Col] = (Desktop32[(Row * DesktopPitchInPixels) + Col] & AndMask32) ^ XorMask32;
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
		UINT* Buffer32 = reinterpret_cast<UINT*>(PtrInfo->PtrShapeBuffer);

		// Iterate through pixels
		for (INT Row = 0; Row < *PtrHeight; ++Row) {
			for (INT Col = 0; Col < *PtrWidth; ++Col) {
				// Set up mask
				UINT MaskVal = 0xFF000000 & Buffer32[(Col + SkipX) + ((Row + SkipY) * (PtrInfo->ShapeInfo.Pitch / sizeof(UINT)))];
				if (MaskVal) {
					// Mask was 0xFF
					InitBuffer32[(Row * *PtrWidth) + Col] = (Desktop32[(Row * DesktopPitchInPixels) + Col] ^ Buffer32[(Col + SkipX) + ((Row + SkipY) * (PtrInfo->ShapeInfo.Pitch / sizeof(UINT)))]) | 0xFF000000;
				}
				else {
					// Mask was 0x00
					InitBuffer32[(Row * *PtrWidth) + Col] = Buffer32[(Col + SkipX) + ((Row + SkipY) * (PtrInfo->ShapeInfo.Pitch / sizeof(UINT)))] | 0xFF000000;
				}
			}
		}
	}

	// Done with resource
	hr = CopySurface->Unmap();
	CopySurface->Release();
	CopySurface = nullptr;
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to unmap surface for pointer", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	return DUPL_RETURN_SUCCESS;
}

//
// Draw mouse provided in buffer to backbuffer
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::DrawMouse(_In_ PTR_INFO* PtrInfo) {
	// Vars to be used
	ID3D11Texture2D* MouseTex = nullptr;
	ID3D11ShaderResourceView* ShaderRes = nullptr;
	ID3D11Buffer* VertexBufferMouse = nullptr;
	D3D11_SUBRESOURCE_DATA InitData;
	D3D11_TEXTURE2D_DESC Desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC SDesc;

	// Position will be changed based on mouse position
	VERTEX Vertices[NUMVERTICES] = {
		{ XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_TEXTURE2D_DESC FullDesc;
	m_pSharedSurf->GetDesc(&FullDesc);
	INT DesktopWidth = FullDesc.Width;
	INT DesktopHeight = FullDesc.Height;

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

	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	// Set shader resource properties
	SDesc.Format = Desc.Format;
	SDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SDesc.Texture2D.MostDetailedMip = Desc.MipLevels - 1;
	SDesc.Texture2D.MipLevels = Desc.MipLevels;

	switch (PtrInfo->ShapeInfo.Type) {
	case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR: {
		PtrLeft = PtrInfo->Position.x;
		PtrTop = PtrInfo->Position.y;

		PtrWidth = static_cast<INT>(PtrInfo->ShapeInfo.Width);
		PtrHeight = static_cast<INT>(PtrInfo->ShapeInfo.Height);

		break;
	}

	case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME: {
		ProcessMonoMask(true, PtrInfo, &PtrWidth, &PtrHeight, &PtrLeft, &PtrTop, &InitBuffer, &Box);
		break;
	}

	case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR: {
		ProcessMonoMask(false, PtrInfo, &PtrWidth, &PtrHeight, &PtrLeft, &PtrTop, &InitBuffer, &Box);
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
	Desc.Width = PtrWidth;
	Desc.Height = PtrHeight;

	// Set up init data
	InitData.pSysMem = (PtrInfo->ShapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR) ? PtrInfo->PtrShapeBuffer : InitBuffer;
	InitData.SysMemPitch = (PtrInfo->ShapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR) ? PtrInfo->ShapeInfo.Pitch : PtrWidth * BPP;
	InitData.SysMemSlicePitch = 0;

	// Create mouseshape as texture
	HRESULT hr = m_pDevice->CreateTexture2D(&Desc, &InitData, &MouseTex);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create mouse pointer texture", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Create shader resource from texture
	hr = m_pDevice->CreateShaderResourceView(MouseTex, &SDesc, &ShaderRes);
	if (FAILED(hr)) {
		MouseTex->Release();
		MouseTex = nullptr;
		return ProcessFailure(m_pDevice, L"Failed to create shader resource from mouse pointer texture", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	D3D11_BUFFER_DESC BDesc;
	ZeroMemory(&BDesc, sizeof(D3D11_BUFFER_DESC));
	BDesc.Usage = D3D11_USAGE_DEFAULT;
	BDesc.ByteWidth = sizeof(VERTEX) * NUMVERTICES;
	BDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BDesc.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = Vertices;

	// Create vertex buffer
	hr = m_pDevice->CreateBuffer(&BDesc, &InitData, &VertexBufferMouse);
	if (FAILED(hr)) {
		ShaderRes->Release();
		ShaderRes = nullptr;
		MouseTex->Release();
		MouseTex = nullptr;
		return ProcessFailure(m_pDevice, L"Failed to create mouse pointer vertex buffer in OutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Set resources
	FLOAT BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &VertexBufferMouse, &Stride, &Offset);
	m_pDeviceContext->OMSetBlendState(m_pBlendState, BlendFactor, 0xFFFFFFFF);
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, nullptr);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetShaderResources(0, 1, &ShaderRes);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

	// Draw
	m_pDeviceContext->Draw(NUMVERTICES, 0);

	// Clean
	if (VertexBufferMouse) {
		VertexBufferMouse->Release();
		VertexBufferMouse = nullptr;
	}
	if (ShaderRes) {
		ShaderRes->Release();
		ShaderRes = nullptr;
	}
	if (MouseTex) {
		MouseTex->Release();
		MouseTex = nullptr;
	}
	if (InitBuffer)	{
		delete[] InitBuffer;
		InitBuffer = nullptr;
	}

	return DUPL_RETURN_SUCCESS;
}

//
// Initialize shaders for drawing to screen
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::InitShaders() {
	HRESULT hr;

	UINT Size = ARRAYSIZE(g_VS);
	hr = m_pDevice->CreateVertexShader(g_VS, Size, nullptr, &m_pVertexShader);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create vertex shader in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	D3D11_INPUT_ELEMENT_DESC Layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT NumElements = ARRAYSIZE(Layout);
	hr = m_pDevice->CreateInputLayout(Layout, NumElements, g_VS, Size, &m_pInputLayout);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create input layout in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	Size = ARRAYSIZE(g_PS);
	hr = m_pDevice->CreatePixelShader(g_PS, Size, nullptr, &m_pPixelShader);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create pixel shader in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	return DUPL_RETURN_SUCCESS;
}

//
// Reset render target view
//
DUPL_RETURN D3D11DesktopDuplicationOutputManager::MakeRTV() {
	// Get backbuffer
	ID3D11Texture2D* BackBuffer = nullptr;
	HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer));
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to get backbuffer for making render target view in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Create a render target view
	hr = m_pDevice->CreateRenderTargetView(BackBuffer, nullptr, &m_pRTV);
	BackBuffer->Release();
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to create render target view in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}
	//*/
	// Set new render target
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, nullptr);

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
	HRESULT hr = m_pSwapChain->ResizeBuffers(SwapChainDesc.BufferCount, Width, Height, SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags);
	if (FAILED(hr)) {
		return ProcessFailure(m_pDevice, L"Failed to resize swapchain buffers in D3D11DesktopDuplicationOutputManager", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Make new render target view
	DUPL_RETURN Ret = MakeRTV();
	if (Ret != DUPL_RETURN_SUCCESS) {
		return Ret;
	}

	// Set new viewport
	SetViewPort(Width, Height);

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
