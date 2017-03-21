#include "Win32Helper.h"

RESULT Win32Helper::ThreadBlockingMessageLoop() {
	MSG msg;

	BOOL ret = 0;

	while (ret = GetMessage(&msg, NULL, 0, 0))
	{
		if (ret > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (ret < 0) {
			// window message error
		}
		else {
			break;
		}
	}

	return R_PASS;
}

RESULT Win32Helper::PostQuitMessage(std::thread& thread) {
	BOOL res = PostThreadMessage(GetThreadId(thread.native_handle()), WM_QUIT, 0, 0);

	return ((res) ? R_PASS : R_ERROR);
}