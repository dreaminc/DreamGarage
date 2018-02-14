#include "D3D11DesktopDuplicationManager.h"


//
// Constructor sets up references / variables
//
D3D11DesktopDuplicationManager::D3D11DesktopDuplicationManager() : m_DeskDupl(nullptr),
m_AcquiredDesktopImage(nullptr),
m_MetaDataBuffer(nullptr),
m_MetaDataSize(0),
m_OutputNumber(0),
m_Device(nullptr)
{
	RtlZeroMemory(&m_OutputDesc, sizeof(m_OutputDesc));
}

//
// Destructor simply calls CleanRefs to destroy everything
//
D3D11DesktopDuplicationManager::~D3D11DesktopDuplicationManager()
{
	if (m_DeskDupl)
	{
		m_DeskDupl->Release();
		m_DeskDupl = nullptr;
	}

	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}

	if (m_MetaDataBuffer)
	{
		delete[] m_MetaDataBuffer;
		m_MetaDataBuffer = nullptr;
	}

	if (m_Device)
	{
		m_Device->Release();
		m_Device = nullptr;
	}
}

//
// Initialize duplication interfaces
//
DUPL_RETURN D3D11DesktopDuplicationManager::InitDupl(_In_ ID3D11Device* Device, UINT Output)
{
	m_OutputNumber = Output;

	// Take a reference on the device
	m_Device = Device;
	m_Device->AddRef();

	// Get DXGI device
	IDXGIDevice* DxgiDevice = nullptr;
	HRESULT hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to QI for DXGI Device", L"Error", hr);
	}

	// Get DXGI adapter
	IDXGIAdapter* DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	DxgiDevice->Release();
	DxgiDevice = nullptr;
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to get parent DXGI Adapter", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Get output
	IDXGIOutput* DxgiOutput = nullptr;
	hr = DxgiAdapter->EnumOutputs(Output, &DxgiOutput);
	DxgiAdapter->Release();
	DxgiAdapter = nullptr;
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to get specified output in D3D11DesktopDuplicationManager", L"Error", hr, EnumOutputsExpectedErrors);
	}

	DxgiOutput->GetDesc(&m_OutputDesc);

	// QI for Output 1
	IDXGIOutput1* DxgiOutput1 = nullptr;
	hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
	DxgiOutput->Release();
	DxgiOutput = nullptr;
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to QI for DxgiOutput1 in D3D11DesktopDuplicationManager", L"Error", hr);
	}

	// Create desktop duplication
	hr = DxgiOutput1->DuplicateOutput(m_Device, &m_DeskDupl);
	DxgiOutput1->Release();
	DxgiOutput1 = nullptr;
	if (FAILED(hr))
	{
		if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		{
			MessageBoxW(nullptr, L"There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.", L"Error", MB_OK);
			return DUPL_RETURN_ERROR_UNEXPECTED;
		}
		return ProcessFailure(m_Device, L"Failed to get duplicate output in D3D11DesktopDuplicationManager", L"Error", hr, CreateDuplicationExpectedErrors);
	}

	return DUPL_RETURN_SUCCESS;
}

/*
RESULT D3D11DesktopDuplicationManager::RegisterDesktopAppObserver(D3D11DesktopDuplicationManager* pDesktopControllerObserver) {
	RESULT r = R_PASS;

	CBM((m_pWebBrowserControllerObserver == nullptr), "CEFBrowserControllerObserver already registered");
	CN(pCEFBrowserControllerObserver);

	m_pWebBrowserControllerObserver = pCEFBrowserControllerObserver;

Error:
	return r;
}
*/

//
// Retrieves mouse info and write it into PtrInfo
//
DUPL_RETURN D3D11DesktopDuplicationManager::GetMouse(_Inout_ PTR_INFO* PtrInfo, _In_ DXGI_OUTDUPL_FRAME_INFO* FrameInfo, INT OffsetX, INT OffsetY)
{
	// A non-zero mouse update timestamp indicates that there is a mouse position update and optionally a shape change
	if (FrameInfo->LastMouseUpdateTime.QuadPart == 0)
	{
		return DUPL_RETURN_SUCCESS;
	}

	bool UpdatePosition = true;

	// Make sure we don't update pointer position wrongly
	// If pointer is invisible, make sure we did not get an update from another output that the last time that said pointer
	// was visible, if so, don't set it to invisible or update.
	if (!FrameInfo->PointerPosition.Visible && (PtrInfo->WhoUpdatedPositionLast != m_OutputNumber))
	{
		UpdatePosition = false;
	}

	// If two outputs both say they have a visible, only update if new update has newer timestamp
	if (FrameInfo->PointerPosition.Visible && PtrInfo->Visible && (PtrInfo->WhoUpdatedPositionLast != m_OutputNumber) && (PtrInfo->LastTimeStamp.QuadPart > FrameInfo->LastMouseUpdateTime.QuadPart))
	{
		UpdatePosition = false;
	}

	// Update position
	if (UpdatePosition)
	{
		PtrInfo->Position.x = FrameInfo->PointerPosition.Position.x + m_OutputDesc.DesktopCoordinates.left - OffsetX;
		PtrInfo->Position.y = FrameInfo->PointerPosition.Position.y + m_OutputDesc.DesktopCoordinates.top - OffsetY;
		PtrInfo->WhoUpdatedPositionLast = m_OutputNumber;
		PtrInfo->LastTimeStamp = FrameInfo->LastMouseUpdateTime;
		PtrInfo->Visible = FrameInfo->PointerPosition.Visible != 0;
	}

	// No new shape
	if (FrameInfo->PointerShapeBufferSize == 0)
	{
		return DUPL_RETURN_SUCCESS;
	}

	// Old buffer too small
	if (FrameInfo->PointerShapeBufferSize > PtrInfo->BufferSize)
	{
		if (PtrInfo->PtrShapeBuffer)
		{
			delete[] PtrInfo->PtrShapeBuffer;
			PtrInfo->PtrShapeBuffer = nullptr;
		}
		PtrInfo->PtrShapeBuffer = new (std::nothrow) BYTE[FrameInfo->PointerShapeBufferSize];
		if (!PtrInfo->PtrShapeBuffer)
		{
			PtrInfo->BufferSize = 0;
			return ProcessFailure(nullptr, L"Failed to allocate memory for pointer shape in D3D11DesktopDuplicationManager", L"Error", E_OUTOFMEMORY);
		}

		// Update buffer size
		PtrInfo->BufferSize = FrameInfo->PointerShapeBufferSize;
	}

	// Get shape
	UINT BufferSizeRequired;
	HRESULT hr = m_DeskDupl->GetFramePointerShape(FrameInfo->PointerShapeBufferSize, reinterpret_cast<VOID*>(PtrInfo->PtrShapeBuffer), &BufferSizeRequired, &(PtrInfo->ShapeInfo));
	if (FAILED(hr))
	{
		delete[] PtrInfo->PtrShapeBuffer;
		PtrInfo->PtrShapeBuffer = nullptr;
		PtrInfo->BufferSize = 0;
		return ProcessFailure(m_Device, L"Failed to get frame pointer shape in D3D11DesktopDuplicationManager", L"Error", hr, FrameInfoExpectedErrors);
	}

	return DUPL_RETURN_SUCCESS;
}


//
// Get next frame and write it into Data
//
_Success_(*Timeout == false && return == DUPL_RETURN_SUCCESS)
DUPL_RETURN D3D11DesktopDuplicationManager::GetFrame(_Out_ FRAME_DATA* Data, _Out_ bool* Timeout)
{
	IDXGIResource* DesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;

	// Get new frame
	HRESULT hr = m_DeskDupl->AcquireNextFrame(500, &FrameInfo, &DesktopResource);
	if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	{
		*Timeout = true;
		return DUPL_RETURN_SUCCESS;
	}
	*Timeout = false;

	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to acquire next frame in D3D11DesktopDuplicationManager", L"Error", hr, FrameInfoExpectedErrors);
	}

	// If still holding old frame, destroy it
	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}

	// QI for IDXGIResource
	hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_AcquiredDesktopImage));
	DesktopResource->Release();
	DesktopResource = nullptr;
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in D3D11DesktopDuplicationManager", L"Error", hr);
	}

	/*
	unsigned char* pTextureBuffer;
	ID3D11Texture2D *pTextureForDream = nullptr;
	ID3D11DeviceContext *pDeviceContextForDream;
	DXGI_OUTDUPL_DESC descDreamOutput;
	m_DeskDupl->GetDesc(&descDreamOutput);

	pDeviceContextForDream->CopyResource(pTextureForDream, m_AcquiredDesktopImage);

	D3D11_TEXTURE2D_DESC descDream;
	pTextureForDream->GetDesc(&descDream);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT subresource = D3D11CalcSubresource(0, 0, 0);	// What is this

	pDeviceContextForDream->Map(pTextureForDream, subresource, D3D11_MAP_READ, 0, &mappedResource);

	BYTE *pBuffer(new BYTE[mappedResource.RowPitch * descDream.Height]);		// hmm
	UINT bmpRowPitch = descDreamOutput.ModeDesc.Width * 4;
	BYTE* sptr = reinterpret_cast<BYTE*>(mappedResource.pData);
	BYTE* dptr = pBuffer + mappedResource.RowPitch * descDream.Height - bmpRowPitch;
	
	UINT rowPitch;
	if (bmpRowPitch < mappedResource.RowPitch) {
		rowPitch = bmpRowPitch;
	}
	else {
		rowPitch = mappedResource.RowPitch;
	}

	for (size_t h = 0; h < descDreamOutput.ModeDesc.Height; ++h)
	{
		memcpy_s(dptr, bmpRowPitch, sptr, rowPitch);
		sptr += mappedResource.RowPitch;
		dptr -= bmpRowPitch;
	}

	pDeviceContextForDream->Unmap(pTextureForDream, subresource);
	long captureSize = rowPitch * descDream.Height;
	pTextureBuffer = new UCHAR[captureSize];
	pTextureBuffer = (UCHAR*)malloc(captureSize);

	//Copying to UCHAR buffer 
	memcpy(pTextureBuffer, pBuffer, captureSize);
	//*/

	// Get metadata
	// Get buffer from here, pass to onpaint
	if (FrameInfo.TotalMetadataBufferSize)
	{
		// Old buffer too small
		if (FrameInfo.TotalMetadataBufferSize > m_MetaDataSize)
		{
			if (m_MetaDataBuffer)
			{
				delete[] m_MetaDataBuffer;
				m_MetaDataBuffer = nullptr;
			}
			m_MetaDataBuffer = new (std::nothrow) BYTE[FrameInfo.TotalMetadataBufferSize];
			if (!m_MetaDataBuffer)
			{
				m_MetaDataSize = 0;
				Data->MoveCount = 0;
				Data->DirtyCount = 0;
				return ProcessFailure(nullptr, L"Failed to allocate memory for metadata in D3D11DesktopDuplicationManager", L"Error", E_OUTOFMEMORY);
			}
			m_MetaDataSize = FrameInfo.TotalMetadataBufferSize;
		}

		UINT BufSize = FrameInfo.TotalMetadataBufferSize;

		// Get move rectangles
		hr = m_DeskDupl->GetFrameMoveRects(BufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(m_MetaDataBuffer), &BufSize);
		if (FAILED(hr))
		{
			Data->MoveCount = 0;
			Data->DirtyCount = 0;
			return ProcessFailure(nullptr, L"Failed to get frame move rects in D3D11DesktopDuplicationManager", L"Error", hr, FrameInfoExpectedErrors);
		}
		Data->MoveCount = BufSize / sizeof(DXGI_OUTDUPL_MOVE_RECT);

		BYTE* DirtyRects = m_MetaDataBuffer + BufSize;
		BufSize = FrameInfo.TotalMetadataBufferSize - BufSize;

		// Get dirty rectangles
		hr = m_DeskDupl->GetFrameDirtyRects(BufSize, reinterpret_cast<RECT*>(DirtyRects), &BufSize);
		if (FAILED(hr))
		{
			Data->MoveCount = 0;
			Data->DirtyCount = 0;
			return ProcessFailure(nullptr, L"Failed to get frame dirty rects in D3D11DesktopDuplicationManager", L"Error", hr, FrameInfoExpectedErrors);
		}
		Data->DirtyCount = BufSize / sizeof(RECT);

		Data->MetaData = m_MetaDataBuffer;
//		m_pDesktopControllerObserver->OnDataBuffer(m_MetaDataBuffer);
	}

	Data->Frame = m_AcquiredDesktopImage;
	Data->FrameInfo = FrameInfo;

	return DUPL_RETURN_SUCCESS;
}

//
// Release frame
//
DUPL_RETURN D3D11DesktopDuplicationManager::DoneWithFrame()
{
	HRESULT hr = m_DeskDupl->ReleaseFrame();
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to release frame in D3D11DesktopDuplicationManager", L"Error", hr, FrameInfoExpectedErrors);
	}

	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}

	return DUPL_RETURN_SUCCESS;
}

//
// Gets output desc into DescPtr
//
void D3D11DesktopDuplicationManager::GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* DescPtr)
{
	*DescPtr = m_OutputDesc;
}
