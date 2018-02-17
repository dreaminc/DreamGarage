#include "D3D11DesktopDuplicationManager.h"
#include "RESULT/EHM.h"

//
// Constructor sets up references / variables
//
D3D11DesktopDuplicationManager::D3D11DesktopDuplicationManager() {
	RtlZeroMemory(&m_OutputDesc, sizeof(m_OutputDesc));
}

//
// Destructor simply calls CleanRefs to destroy everything
//
D3D11DesktopDuplicationManager::~D3D11DesktopDuplicationManager() {
	if (m_pDeskDupl) {
		m_pDeskDupl->Release();
		m_pDeskDupl = nullptr;
	}

	if (m_pAcquiredDesktopImage) {
		m_pAcquiredDesktopImage->Release();
		m_pAcquiredDesktopImage = nullptr;
	}

	if (m_pMetaDataBuffer) {
		delete[] m_pMetaDataBuffer;
		m_pMetaDataBuffer = nullptr;
	}

	if (m_pDevice) {
		m_pDevice->Release();
		m_pDevice = nullptr;
	}
}

//
// Initialize duplication interfaces
//
DUPL_RETURN D3D11DesktopDuplicationManager::InitDupl(_In_ ID3D11Device* pDevice, UINT Output) {
	HRESULT r = S_OK;
	m_OutputNumber = Output;

	// Take a reference on the device
	m_pDevice = pDevice;
	m_pDevice->AddRef();

	// Get DXGI device
	IDXGIDevice* DxgiDevice = nullptr;
	CRM(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice)), "Failed to QI for DXGI Device");

	// Get DXGI adapter
	IDXGIAdapter* DxgiAdapter = nullptr;
	CRM(DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter)), "Failed to get parent DXGI Adapter");
	DxgiDevice->Release();
	DxgiDevice = nullptr;

	// Get output
	IDXGIOutput* DxgiOutput = nullptr;
	CRM(DxgiAdapter->EnumOutputs(Output, &DxgiOutput), "Failed to get specified output in D3D11DesktopDuplicationManager");
	DxgiAdapter->Release();
	DxgiAdapter = nullptr;

	DxgiOutput->GetDesc(&m_OutputDesc);

	// QI for Output 1
	IDXGIOutput1* DxgiOutput1 = nullptr;
	CRM(DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1)), "Failed to QI for DxgiOutput1 in D3D11DesktopDuplicationManager");
	DxgiOutput->Release();
	DxgiOutput = nullptr;

	// Create desktop duplication
	CRM(DxgiOutput1->DuplicateOutput(m_pDevice, &m_pDeskDupl), "Failed to get duplicate output in D3D11DesktopDuplicationManager");
	DxgiOutput1->Release();
	DxgiOutput1 = nullptr;

Error:
	if (r == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) {
		MessageBoxW(nullptr, L"There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.", L"Error", MB_OK);
		return DUPL_RETURN_ERROR_UNEXPECTED;
	}
	if (RFAILED()) {
		return ProcessFailure(nullptr, L"Failed to Initialize DuplicationManager", L"Error", r);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Retrieves mouse info and write it into PtrInfo
//
DUPL_RETURN D3D11DesktopDuplicationManager::GetMouse(_Inout_ PTR_INFO* PtrInfo, _In_ DXGI_OUTDUPL_FRAME_INFO* FrameInfo, INT OffsetX, INT OffsetY) {
	// A non-zero mouse update timestamp indicates that there is a mouse position update and optionally a shape change
	if (FrameInfo->LastMouseUpdateTime.QuadPart == 0) {
		return DUPL_RETURN_SUCCESS;
	}

	bool UpdatePosition = true;

	// Make sure we don't update pointer position wrongly
	// If pointer is invisible, make sure we did not get an update from another output that the last time that said pointer
	// was visible, if so, don't set it to invisible or update.
	if (!FrameInfo->PointerPosition.Visible && (PtrInfo->WhoUpdatedPositionLast != m_OutputNumber)) {
		UpdatePosition = false;
	}

	// If two outputs both say they have a visible, only update if new update has newer timestamp
	if (FrameInfo->PointerPosition.Visible && PtrInfo->Visible && (PtrInfo->WhoUpdatedPositionLast != m_OutputNumber) && (PtrInfo->LastTimeStamp.QuadPart > FrameInfo->LastMouseUpdateTime.QuadPart)) {
		UpdatePosition = false;
	}

	// Update position
	if (UpdatePosition) {
		PtrInfo->Position.x = FrameInfo->PointerPosition.Position.x + m_OutputDesc.DesktopCoordinates.left - OffsetX;
		PtrInfo->Position.y = FrameInfo->PointerPosition.Position.y + m_OutputDesc.DesktopCoordinates.top - OffsetY;
		PtrInfo->WhoUpdatedPositionLast = m_OutputNumber;
		PtrInfo->LastTimeStamp = FrameInfo->LastMouseUpdateTime;
		PtrInfo->Visible = FrameInfo->PointerPosition.Visible != 0;
	}

	// No new shape
	if (FrameInfo->PointerShapeBufferSize == 0) {
		return DUPL_RETURN_SUCCESS;
	}

	// Old buffer too small
	if (FrameInfo->PointerShapeBufferSize > PtrInfo->BufferSize) {
		if (PtrInfo->PtrShapeBuffer) {
			delete[] PtrInfo->PtrShapeBuffer;
			PtrInfo->PtrShapeBuffer = nullptr;
		}
		PtrInfo->PtrShapeBuffer = new (std::nothrow) BYTE[FrameInfo->PointerShapeBufferSize];
		if (!PtrInfo->PtrShapeBuffer) {
			PtrInfo->BufferSize = 0;
			return ProcessFailure(nullptr, L"Failed to allocate memory for pointer shape in D3D11DesktopDuplicationManager", L"Error", E_OUTOFMEMORY);
		}

		// Update buffer size
		PtrInfo->BufferSize = FrameInfo->PointerShapeBufferSize;
	}

	// Get shape
	UINT BufferSizeRequired;
	
	HRESULT r = S_OK;
	CR(m_pDeskDupl->GetFramePointerShape(FrameInfo->PointerShapeBufferSize, reinterpret_cast<VOID*>(PtrInfo->PtrShapeBuffer), &BufferSizeRequired, &(PtrInfo->ShapeInfo)));

Error:
	if (RFAILED()) {
		delete[] PtrInfo->PtrShapeBuffer;
		PtrInfo->PtrShapeBuffer = nullptr;
		PtrInfo->BufferSize = 0;
		return ProcessFailure(m_pDevice, L"Failed to get frame pointer shape in D3D11DesktopDuplicationManager", L"Error", r, FrameInfoExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}


//
// Get next frame and write it into Data
//
_Success_(*Timeout == false && return == DUPL_RETURN_SUCCESS)
DUPL_RETURN D3D11DesktopDuplicationManager::GetFrame(_Out_ FRAME_DATA* Data, _Out_ bool* Timeout) {
	HRESULT r = S_OK;

	IDXGIResource* pDesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;

	// Get new frame
	CR(m_pDeskDupl->AcquireNextFrame(500, &FrameInfo, &pDesktopResource));
	
	*Timeout = false;

	// If still holding old frame, destroy it
	if (m_pAcquiredDesktopImage) {
		m_pAcquiredDesktopImage->Release();
		m_pAcquiredDesktopImage = nullptr;
	}

	// QI for IDXGIResource
	CRM(pDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_pAcquiredDesktopImage)), "Failed to QI for ID3D11Texture2D from acquired IDXGIResource in D3D11DesktopDuplicationManager");
	pDesktopResource->Release();
	pDesktopResource = nullptr;

	// Get metadata
	if (FrameInfo.TotalMetadataBufferSize) {
		// Old buffer too small
		if (FrameInfo.TotalMetadataBufferSize > m_MetaDataSize) {
			if (m_pMetaDataBuffer) {
				delete[] m_pMetaDataBuffer;
				m_pMetaDataBuffer = nullptr;
			}
			m_pMetaDataBuffer = new (std::nothrow) BYTE[FrameInfo.TotalMetadataBufferSize];
			if (!m_pMetaDataBuffer) {
				m_MetaDataSize = 0;
				Data->MoveCount = 0;
				Data->DirtyCount = 0;
				return ProcessFailure(nullptr, L"Failed to allocate memory for metadata in D3D11DesktopDuplicationManager", L"Error", E_OUTOFMEMORY);
			}
			m_MetaDataSize = FrameInfo.TotalMetadataBufferSize;
		}

		UINT BufSize = FrameInfo.TotalMetadataBufferSize;

		// Get move rectangles
		CRM(m_pDeskDupl->GetFrameMoveRects(BufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(m_pMetaDataBuffer), &BufSize), "Failed to get frame move rects in D3D11DesktopDuplicationManager");

		Data->MoveCount = BufSize / sizeof(DXGI_OUTDUPL_MOVE_RECT);

		BYTE* DirtyRects = m_pMetaDataBuffer + BufSize;
		BufSize = FrameInfo.TotalMetadataBufferSize - BufSize;

		// Get dirty rectangles
		CRM(m_pDeskDupl->GetFrameDirtyRects(BufSize, reinterpret_cast<RECT*>(DirtyRects), &BufSize), "Failed to get frame dirty rects in D3D11DesktopDuplicationManager");

		Data->DirtyCount = BufSize / sizeof(RECT);

		Data->MetaData = m_pMetaDataBuffer;
	}

	Data->Frame = m_pAcquiredDesktopImage;
	Data->FrameInfo = FrameInfo;

Error:
	if (r == DXGI_ERROR_WAIT_TIMEOUT) {
		*Timeout = true;
		return DUPL_RETURN_SUCCESS;
	}
	if (RFAILED()) {
		Data->MoveCount = 0;
		Data->DirtyCount = 0;
		return ProcessFailure(m_pDevice, L"Failed to acquire next frame in D3D11DesktopDuplicationManager", L"Error", r, FrameInfoExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Release frame
//
DUPL_RETURN D3D11DesktopDuplicationManager::DoneWithFrame() {
	HRESULT r = S_OK;

	CR(m_pDeskDupl->ReleaseFrame());

Error:
	if (RFAILED()) {
		return ProcessFailure(m_pDevice, L"Failed to release frame in D3D11DesktopDuplicationManager", L"Error", r, FrameInfoExpectedErrors);
	}
	if (m_pAcquiredDesktopImage) {
		m_pAcquiredDesktopImage->Release();
		m_pAcquiredDesktopImage = nullptr;
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Gets output desc into DescPtr
//
void D3D11DesktopDuplicationManager::GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* DescPtr) {
	*DescPtr = m_OutputDesc;
}
