// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#ifndef _THREADMANAGER_H_
#define _THREADMANAGER_H_

#include "core/ehm/EHM.h"

// Dream Desktop
// dos/src/dreamdesktop/D3D11DesktopDuplicationThreadManager

#include "CommonTypes.h"

class D3D11DesktopDuplicationThreadManager {
public:
	D3D11DesktopDuplicationThreadManager();
	~D3D11DesktopDuplicationThreadManager();
	void Clean();
	DUPL_RETURN Initialize(INT SingleOutput, UINT OutputCount, HANDLE UnexpectedErrorEvent, HANDLE ExpectedErrorEvent, HANDLE TerminateThreadsEvent, HANDLE SharedHandle, _In_ RECT* DesktopDim);
	PTR_INFO* GetPointerInfo();
	void WaitForThreadTermination();

private:
	DUPL_RETURN InitializeDx(_Out_ DX_RESOURCES* Data);
	void CleanDx(_Inout_ DX_RESOURCES* Data);

	PTR_INFO m_PtrInfo;
	UINT m_ThreadCount = 0;
	_Field_size_(m_ThreadCount) HANDLE* m_pThreadHandles = nullptr;
	_Field_size_(m_ThreadCount) THREAD_DATA* m_pThreadData = nullptr;
};

#endif
