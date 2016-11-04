#pragma once

class WindowController
{
public:
	virtual void Show(bool show) = 0;
};

namespace launcher
{
	void EntryPoint(WindowController* pSplashWindow);
	//void EntryPoint(int argc, char *argv[], WindowController* pSplashWindow);
}