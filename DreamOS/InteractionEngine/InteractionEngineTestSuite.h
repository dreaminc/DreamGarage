#ifndef INTERACTION_ENGINE_TEST_SUITE_H_
#define INTERACTION_ENGINE_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/matrix/MatrixTestSuite.h

#include "Primitives/valid.h"
#include "Primitives/Subscriber.h"
#include "Test/DreamTestSuite.h"

#include <functional>
#include <memory>

class DreamOS;
class composite;
struct InteractionObjectEvent;
class DimRay;
class sphere;

struct RayCompositeTestContext : public Subscriber<InteractionObjectEvent> {
	composite *pComposite = nullptr;
	DimRay *pRay = nullptr;
	sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	DreamOS* m_pDreamOS = nullptr;

	virtual RESULT Notify(InteractionObjectEvent *mEvent) override;
};

// TODO: Consider moving valid up to TestSuite
class InteractionEngineTestSuite : public valid, public DreamTestSuite  {
public:
	InteractionEngineTestSuite(DreamOS *pDreamOS);
	~InteractionEngineTestSuite();

	virtual RESULT AddTests() override;

	virtual RESULT SetupPipeline(std::string strRenderProgramName = "standard") override;
	virtual RESULT SetupTestSuite() override;

	RESULT Initialize();

	RESULT AddTestFlatCollisions();

	RESULT AddTestCaptureObject();

	RESULT AddTestObjectBasedEvents();

	RESULT AddTestNestedCompositeOBB();

	RESULT AddTestMultiPrimitive();
	RESULT AddTestMultiPrimitiveComposite();

	RESULT AddTestMultiPrimitiveRemove();
	RESULT AddTestMultiPrimitiveCompositeRemove();

	RESULT AddTestCompositeRay();
	RESULT AddTestCompositeRayNested();
	RESULT AddTestCompositeRayController();

	RESULT InitializeRayCompositeTest(void *pContext);
	RESULT ResetTest(void *pContext);
	RESULT AddNestedCompositeQuads(int nestingLevel, float size, std::shared_ptr<composite> pCompositeParent);
	

private:
	DreamOS *m_pDreamOS;

};

#endif // ! INTERACTION_ENGINE_TEST_SUITE_H_
