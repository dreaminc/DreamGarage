#include "D3D11DesktopDuplicationThreadManager.h"
#include "RESULT/EHM.h"

DWORD WINAPI DDProc(_In_ void* Param);

D3D11DesktopDuplicationThreadManager::D3D11DesktopDuplicationThreadManager() {
	RtlZeroMemory(&m_PtrInfo, sizeof(m_PtrInfo));
}

D3D11DesktopDuplicationThreadManager::~D3D11DesktopDuplicationThreadManager() {
	Clean();
}

//
// Clean up resources
//
void D3D11DesktopDuplicationThreadManager::Clean() {
	if (m_PtrInfo.PtrShapeBuffer) {
		delete[] m_PtrInfo.PtrShapeBuffer;
		m_PtrInfo.PtrShapeBuffer = nullptr;
	}
	RtlZeroMemory(&m_PtrInfo, sizeof(m_PtrInfo));

	if (m_pThreadHandles) {
		for (UINT i = 0; i < m_ThreadCount; ++i) {
			if (m_pThreadHandles[i]) {
				CloseHandle(m_pThreadHandles[i]);
			}
		}
		delete[] m_pThreadHandles;
		m_pThreadHandles = nullptr;
	}

	if (m_pThreadData) {
		for (UINT i = 0; i < m_ThreadCount; ++i) {
			CleanDx(&m_pThreadData[i].DxRes);
		}
		delete[] m_pThreadData;
		m_pThreadData = nullptr;
	}

	m_ThreadCount = 0;
}

//
// Clean up DX_RESOURCES
//
void D3D11DesktopDuplicationThreadManager::CleanDx(_Inout_ DX_RESOURCES* pDXResourceData) {
	if (pDXResourceData->Device) {
		pDXResourceData->Device->Release();
		pDXResourceData->Device = nullptr;
	}

	if (pDXResourceData->Context) {
		pDXResourceData->Context->Release();
		pDXResourceData->Context = nullptr;
	}

	if (pDXResourceData->VertexShader) {
		pDXResourceData->VertexShader->Release();
		pDXResourceData->VertexShader = nullptr;
	}

	if (pDXResourceData->PixelShader) {
		pDXResourceData->PixelShader->Release();
		pDXResourceData->PixelShader = nullptr;
	}

	if (pDXResourceData->InputLayout) {
		pDXResourceData->InputLayout->Release();
		pDXResourceData->InputLayout = nullptr;
	}

	if (pDXResourceData->SamplerLinear) {
		pDXResourceData->SamplerLinear->Release();
		pDXResourceData->SamplerLinear = nullptr;
	}
}

//
// Start up threads for DDA
//
DUPL_RETURN D3D11DesktopDuplicationThreadManager::Initialize(INT outputToDuplicate, UINT OutputCount, HANDLE UnexpectedErrorEvent, HANDLE ExpectedErrorEvent, HANDLE TerminateThreadsEvent, HANDLE SharedHandle, _In_ RECT* pDesktopDim) {
	m_ThreadCount = OutputCount;
	m_pThreadHandles = new (std::nothrow) HANDLE[m_ThreadCount];
	m_pThreadData = new (std::nothrow) THREAD_DATA[m_ThreadCount];
	if (!m_pThreadHandles || !m_pThreadData) {
		return ProcessFailure(nullptr, L"Failed to allocate array for threads", L"Error", E_OUTOFMEMORY);
	}

	// Create appropriate # of threads for duplication
	DUPL_RETURN Ret = DUPL_RETURN_SUCCESS;
	for (UINT i = 0; i < m_ThreadCount; ++i) {
		m_pThreadData[i].UnexpectedErrorEvent = UnexpectedErrorEvent;
		m_pThreadData[i].ExpectedErrorEvent = ExpectedErrorEvent;
		m_pThreadData[i].TerminateThreadsEvent = TerminateThreadsEvent;
		m_pThreadData[i].Output = (outputToDuplicate < 0) ? i : outputToDuplicate;
		m_pThreadData[i].TexSharedHandle = SharedHandle;
		m_pThreadData[i].OffsetX = pDesktopDim->left;
		m_pThreadData[i].OffsetY = pDesktopDim->top;
		m_pThreadData[i].PtrInfo = &m_PtrInfo;

		RtlZeroMemory(&m_pThreadData[i].DxRes, sizeof(DX_RESOURCES));
		Ret = InitializeDx(&m_pThreadData[i].DxRes);
		if (Ret != DUPL_RETURN_SUCCESS) {
			return Ret;
		}

		DWORD ThreadId;
		m_pThreadHandles[i] = CreateThread(nullptr, 0, DDProc, &m_pThreadData[i], 0, &ThreadId);
		if (m_pThreadHandles[i] == nullptr) {
			return ProcessFailure(nullptr, L"Failed to create thread", L"Error", E_FAIL);
		}
	}

	return Ret;
}

//
// Get DX_RESOURCES
//
DUPL_RETURN D3D11DesktopDuplicationThreadManager::InitializeDx(_Out_ DX_RESOURCES* pDXResourcesData) {
	HRESULT r = S_OK;

	// Driver types supported
	D3D_DRIVER_TYPE DriverTypes[] =	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

	// Feature levels supported
	D3D_FEATURE_LEVEL FeatureLevels[] =	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

	D3D_FEATURE_LEVEL FeatureLevel;
	UINT geometryShaderSize = ARRAYSIZE(g_VS);
	D3D11_INPUT_ELEMENT_DESC Layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT NumElements = ARRAYSIZE(Layout);
	D3D11_SAMPLER_DESC SampDesc;

	// Create device
	for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)	{
		D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0, FeatureLevels, NumFeatureLevels,
			D3D11_SDK_VERSION, &pDXResourcesData->Device, &FeatureLevel, &pDXResourcesData->Context);
		// Device creation success, no need to loop anymore
		if (pDXResourcesData->Device != nullptr) {
			break;
		}
	}
	CNM(pDXResourcesData->Device, "Failed to create device in InitializeDx");

	// VERTEX shader	
	CRM(pDXResourcesData->Device->CreateVertexShader(g_VS, geometryShaderSize, nullptr, &pDXResourcesData->VertexShader), "Failed to create vertex shader in InitializeDx");

	// Input layout
	CRM(pDXResourcesData->Device->CreateInputLayout(Layout, NumElements, g_VS, geometryShaderSize, &pDXResourcesData->InputLayout), "Failed to create input layout in InitializeDx");
	
	pDXResourcesData->Context->IASetInputLayout(pDXResourcesData->InputLayout);

	// Pixel shader
	geometryShaderSize = ARRAYSIZE(g_PS);
	CRM(pDXResourcesData->Device->CreatePixelShader(g_PS, geometryShaderSize, nullptr, &pDXResourcesData->PixelShader), "Failed to create pixel shader in InitializeDx");

	// Set up sampler
	RtlZeroMemory(&SampDesc, sizeof(SampDesc));
	SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SampDesc.MinLOD = 0;
	SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	CRM(pDXResourcesData->Device->CreateSamplerState(&SampDesc, &pDXResourcesData->SamplerLinear), "Failed to create sampler state in InitializeDx");

Error:
	if (RFAILED()) {
		return ProcessFailure(pDXResourcesData->Device, L"Failed to InitializeDx in ThreadManager", L"Error", r, SystemTransitionsExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Getter for the PTR_INFO structure
//
PTR_INFO* D3D11DesktopDuplicationThreadManager::GetPointerInfo() {
	return &m_PtrInfo;
}

//
// Waits infinitely for all spawned threads to terminate
//
void D3D11DesktopDuplicationThreadManager::WaitForThreadTermination() {
	if (m_ThreadCount != 0) {
		WaitForMultipleObjectsEx(m_ThreadCount, m_pThreadHandles, TRUE, INFINITE, FALSE);
	}
}
