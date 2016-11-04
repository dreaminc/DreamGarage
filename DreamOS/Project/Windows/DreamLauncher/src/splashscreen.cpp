#include "Logger.h"
#include "launcher.h"

#include <wx/wxprec.h>
#include <wx/animate.h>
#include <wx/mstream.h>

class SplashApp : public wxApp, public WindowController
{
public:
	virtual bool OnInit();

	// override main cmdln vars
	virtual bool Initialize(int& argc, wxChar **argv);
	// use for triggering the splash on and off
	virtual void Show(bool show) override;

private:
	wxFrame*	m_frame;
	std::thread	m_launcherThread;
};

// used to link wxApp with WinMain
wxIMPLEMENT_APP(SplashApp);

bool SplashApp::Initialize(int& argc, wxChar **argv)
{
	m_launcherThread = std::thread(launcher::EntryPoint, this);

	// we don't need the args for the wxApp
	argc = 1;

	return wxApp::Initialize(argc, argv);
}

bool LoadDataFromResource(char*& t_data, DWORD& t_dataSize, const wxString& t_name)
{
	bool     result = false;
	HGLOBAL  resHandle = 0;
	HRSRC    resource;

	resource = FindResource(0, t_name.wchar_str(), RT_RCDATA);

	if (resource != 0)
	{
		resHandle = LoadResource(NULL, resource);

		if (resHandle != 0)
		{
			t_data = (char*)LockResource(resHandle);
			t_dataSize = SizeofResource(NULL, resource);
			result = true;
		}
	}

	return result;
}

bool SplashApp::OnInit()
{
	m_frame = new wxFrame(NULL, wxID_ANY, "DreamSplash", wxDefaultPosition, wxSize(350, 350), wxSYSTEM_MENU);

	wxAnimationCtrl* animation = new wxAnimationCtrl(m_frame, wxID_ANY, wxNullAnimation, wxPoint(-225, -125));

	char* data = nullptr;
	DWORD	size = 0;

	if (!LoadDataFromResource(data, size, "SPLASH_LOADING_GIF"))
	{
		LOG(ERROR) << "resource not found";
		return false;
	}

	wxMemoryInputStream splash(data, size);

	animation->Load(splash);
	animation->Play();
	m_frame->Center();
	m_frame->Show(true);

	return true;
}

void SplashApp::Show(bool show)
{
	m_frame->Show(show);
}
