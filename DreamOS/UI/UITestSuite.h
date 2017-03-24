#ifndef UI_TEST_SUITE_H_
#define UI_TEST_SUITE_H_

#include "RESULT/EHM.h"

#include "Primitives/valid.h"
#include "Primitives/Subscriber.h"
#include "Test/TestSuite.h"

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>

class DreamOS;
class DreamUIBar;
class VirtualObj;
class sphere;

struct SenseControllerEvent;
struct SenseKeyboardEvent;
struct SenseMouseEvent;
struct InteractionObjectEvent;

class UITestSuite : public valid, public TestSuite, public Subscriber<SenseControllerEvent>, public Subscriber<SenseKeyboardEvent>, public Subscriber<SenseMouseEvent> {
public:
	UITestSuite(DreamOS *pDreamOS);
	~UITestSuite();

	RESULT Initialize();
	RESULT InitializeUI();

	RESULT AddTestUI();
	RESULT AddTestInteractionUI();
	RESULT AddTestInteractionFauxUI();
	RESULT AddTestSharedContentView();

	virtual RESULT AddTests() override;

public:
	virtual RESULT Notify(SenseControllerEvent *event) override;
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseMouseEvent *mEvent) override;

private:
	DreamOS *m_pDreamOS;
	std::shared_ptr<DreamUIBar> m_pDreamUIBar;

	VirtualObj* m_pPrevSelected;

	std::map<std::string, std::vector<std::string>> m_menu;
	std::stack<std::string> m_path;

	sphere *m_pSphere1;
	sphere *m_pSphere2;
};



#endif UI_TEST_SUITE_H_