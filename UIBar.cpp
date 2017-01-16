#include "UIBar.h"
#include "Primitives/FlatContext.h"
#include "Primitives/camera.h"

#include "DreamConsole\DreamConsole.h"

UIBar::UIBar(composite* c) :
	m_context(c)
{
	Initialize();
}

UIBar::~UIBar() {
	// empty
}

RESULT UIBar::Initialize() {

	RESULT r = R_PASS;

	//VARIABLES FOR DOUG
	m_numButtons = 5;			// number of buttons in the arc 

	float height = -0.5f;	// y position of buttons on the screen, 0.0f is the center
	m_depth = -1.5f;	// distance between the screen and a button (z position)

	m_angleY = 20.0f;	// angle between buttons on the arc
	float angleX = 60.0f;	// upward tilt of the buttons

	m_UIScale = vector(0.5f, 1.0f, 0.25f); // 
	m_enlargedScale = 1.25f;	// this value is multiplied to the scale when the menu item is selected

	std::vector<std::string> words = { "Watch", "Listen", "Play", "Whisper", "Present" }; //words display from left to right

	//other ones
	int shift = m_numButtons / 2;
	float odd = (m_numButtons % 2 == 0) ? 0.5f : 0.0f;

	for (int i = 0; i < m_numButtons; i++) {
		std::shared_ptr<composite> pButton = m_context->AddComposite();
		std::shared_ptr<quad> q = pButton->AddQuad(1.0f, 1.0f);

		std::shared_ptr<FlatContext> pContext = m_context->MakeFlatContext();
		const std::string str = (i < words.size()) ? words[words.size() - 1 - i] : "";
		std::shared_ptr<text> pText = pContext->AddText(L"ArialDistance.fnt", str, 0.1f, true);
		m_context->RenderToTexture(pContext);

		q->SetColorTexture(pContext->GetFramebuffer()->GetTexture());

		float rad = (m_angleY * M_PI / 180.0f) * (i - shift + odd);
		quaternion rot = quaternion::MakeQuaternionWithEuler(0.0f, rad, 0.0f);

		pButton->MoveTo(0.0f, height, 0.0f);
		pButton->SetOrientation(rot);

		q->MoveTo(0.0f, 0.0f, m_depth);
		q->RotateXByDeg(angleX);
		q->ScaleX(m_UIScale.x());
		q->ScaleZ(m_UIScale.z());
	}
	
	m_context->SetVisible(false);

	return r;
}

RESULT UIBar::ToggleVisible() {

	RESULT r = R_PASS;

	m_context->SetVisible(!m_context->IsVisible());

	// if the context has become visible, reset its position to match the camera
	if (m_context->IsVisible()) {
		quaternion q = m_context->GetCamera()->GetOrientation();
		m_rotationY = q.ProjectedYRotationDeg();
		quaternion q2 = quaternion::MakeQuaternionWithEuler(0.0f, m_rotationY * M_PI / 180.0f, 0.0f);

		m_context->SetPosition(m_context->GetCamera()->GetPosition());
		m_context->SetOrientation(q2);
	}

	return r;
}

RESULT UIBar::Notify(SenseControllerEvent *event) {

	SENSE_CONTROLLER_EVENT_TYPE type = event->type;
	int cType = event->state.type;

	if (type == SENSE_CONTROLLER_MENU_DOWN && cType == 1) {
		if (m_UIDirty) {
			ToggleVisible();
			m_UIDirty = false;
		}
	}
	else if (type == SENSE_CONTROLLER_MENU_UP && cType == 1) {
		m_UIDirty = true;
	}

	return R_PASS;
}

RESULT UIBar::Update(ray handRay) {
	
	// convert ray into context's space
	point ptContext = handRay.GetOrigin() - m_context->GetOrigin();
	ptContext = point(ptContext.x(), 0.0f, ptContext.z());
	ray contextRay = ray(ptContext, handRay.GetVector());
	point intersect = FurthestRaySphereIntersect(contextRay, point(0.0f, 0.0f, 0.0f));
	vector vIntersect = vector(intersect);
	vIntersect.Normalize();

	int shift = m_numButtons / 2;
	int numSections = m_numButtons != 0 ? 360 / int(m_angleY) : 1;
	float deg = float(-atan2(vIntersect.x(), vIntersect.z()) * 180.0f / M_PI);
	deg -= m_rotationY;
	int index = (numSections - int((deg) / m_angleY) + shift) % numSections;

	if (m_selectedIndex != index) {
		// swap enlarged menu item
		if (0 <= index && index < m_numButtons) {
			auto p = m_context->GetChildren()[m_numButtons - 1 - index];
			std::shared_ptr<composite> pButton = std::dynamic_pointer_cast<composite>(p);
			if (pButton != nullptr) {
				auto q = pButton->GetChildren()[0];
				q->ScaleX(m_UIScale.x() * m_enlargedScale);
				q->ScaleZ(m_UIScale.z() * m_enlargedScale);
			}
		}
		if (0 <= m_selectedIndex && m_selectedIndex < m_numButtons) {
			auto p = m_context->GetChildren()[m_numButtons - 1 - m_selectedIndex];
			std::shared_ptr<composite> pButton = std::dynamic_pointer_cast<composite>(p);
			if (pButton != nullptr) {
				auto q = pButton->GetChildren()[0];
				q->ScaleX(m_UIScale.x());
				q->ScaleZ(m_UIScale.z());
			}
		}
		m_selectedIndex = index;
	}
	return R_PASS;
}

point UIBar::FurthestRaySphereIntersect(const ray &r, point center) {

	vector vRayCircle = static_cast<ray>(r).ptOrigin() - center;

	float bValue = static_cast<ray>(r).vDirection().dot(vRayCircle);
	float cValue = vRayCircle.dot(vRayCircle) - pow(m_depth, 2.0f);
	float resultValue = pow(bValue, 2.0f) - cValue;

	OVERLAY_DEBUG_SET("res", resultValue);

	if (resultValue >= 0.0f) {
		float term = pow(resultValue, 0.5f);
		float t1 = (-1.0f) * bValue + term;
		//this result will always be smaller
		//float t2 = (-1.0f) * bValue - term;
		return r.GetOrigin() + point(t1 * r.GetVector());
	}
	return point(0.0f, 0.0f, 0.0f);
}