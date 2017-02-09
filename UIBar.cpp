#include "UIBar.h"
#include "Primitives/FlatContext.h"
#include "Primitives/camera.h"

#include "DreamConsole\DreamConsole.h"

#include <algorithm>

UIBar::UIBar(composite* pComposite, UIMenuItem::IconFormat iconFormat, UIMenuItem::LabelFormat labelFormat) :
	UIModule(pComposite),
	m_iconFormat(iconFormat),
	m_labelFormat(labelFormat)
{
	// empty
}

UIBar::~UIBar() {
	// empty
}

RESULT UIBar::UpdateWithRadialLayout(std::shared_ptr<UIMenuItem> pItem, int index, int size, bool fHeader) {

	std::shared_ptr<composite> pButton = pItem->GetButton();
	std::shared_ptr<quad> q = pItem->GetQuad();

	// used to center the UIBar
	int shift = (size-1) / 2;
	float odd = ((size-1) % 2 == 0) ? 0.5f : 0.0f;

	// temp
	float itemAngleX = 60.0f;
	float itemAngleY = 20.0f;
	float headerAngleX = 75.0f;
	float headerYPos = 0.0f;
	float yPosition = -0.5f;
	float menuDepth = -1.5f;
	vector itemScale = vector(1.0f, 1.0f, 1.0f);
	float enlargedScale = 1.25f;

	// Radial layout
	int radIndex = fHeader ? 0 : index;
	float radY = (itemAngleY * M_PI / 180.0f) * -(radIndex - shift + odd);
	quaternion rotY = quaternion::MakeQuaternionWithEuler(0.0f, radY, 0.0f);
	float yPos = fHeader ? headerYPos : yPosition;
	pButton->MoveTo(0.0f, yPos, 0.0f);
	pButton->SetOrientation(rotY);

	float radX = (fHeader ? headerAngleX : itemAngleX) * M_PI / 180.0f;
	quaternion rotX = quaternion::MakeQuaternionWithEuler(radX, 0.0f, 0.0f);
	q->MoveTo(0.0f, 0.0f, menuDepth);
	q->SetOrientation(rotX);

	//could be wrong at first controller update
	float scaleFactor = index == m_selectedIndex ? enlargedScale : 1;
	q->ScaleX(itemScale.x() * scaleFactor);
	q->ScaleZ(itemScale.z() * scaleFactor);

	return R_PASS;
}

RESULT UIBar::ToggleVisible() {

	RESULT r = R_PASS;

	m_pContext->SetVisible(!m_pContext->IsVisible());

	// if the context has become visible, reset its position to match the camera
	if (m_pContext->IsVisible()) {
		quaternion q = m_pContext->GetCamera()->GetOrientation();
		m_headRotationYDeg = q.ProjectedYRotationDeg();
		quaternion q2 = quaternion::MakeQuaternionWithEuler(0.0f, m_headRotationYDeg * M_PI / 180.0f, 0.0f);

		m_pContext->SetPosition(m_pContext->GetCamera()->GetPosition());
		m_pContext->SetOrientation(q2);
	}

	return r;
}

RESULT UIBar::HandleMenuUp(UILayerInfo info) {
	RESULT r = R_PASS;

	if (info.icons.size() == 0 && info.labels.size() == 0) {
		ToggleVisible();
		return r;
	}
	UpdateCurrentUILayer(info);

	return r;
}

RESULT UIBar::HandleTriggerUp(UILayerInfo info) {
	RESULT r = R_PASS;

	if (info.icons.size() == 0 && info.labels.size() == 0) {
		return r;
	}
	UpdateCurrentUILayer(info);

	return r;
}

RESULT UIBar::UpdateSelectedItem(int index, int size) {
	//*
	vector itemScale = vector(1.0f, 1.0f, 1.0f);
	float enlargedScale = 1.25f;

	if (m_selectedIndex != index) {
		// swap enlarged menu item
		if (0 <= index && index < size) {
			auto p = m_currentUILayer->GetMenuItems()[index];
			auto pButton = p->GetButton();
			auto q = p->GetQuad();
			if (pButton != nullptr) {
				q->ScaleX(itemScale.x() * enlargedScale);
				q->ScaleZ(itemScale.z() * enlargedScale);
			}
		}
		if (0 <= m_selectedIndex && m_selectedIndex < size) {
			auto p = m_currentUILayer->GetMenuItems()[m_selectedIndex];
			auto pButton = p->GetButton();
			auto q = p->GetQuad();
			if (pButton != nullptr) {
				q->ScaleX(itemScale.x());
				q->ScaleZ(itemScale.z());
			}
		}
		m_selectedIndex = index;
	}
//*/
	return R_PASS;
}

RESULT UIBar::Update(ray handRay) {
	
	// convert ray into context's space
	//*
	
	point ptContext = handRay.GetOrigin() - m_pContext->GetOrigin();
	ptContext = point(ptContext.x(), 0.0f, ptContext.z());
	ray contextRay = ray(ptContext, handRay.GetVector());
	point intersect = FurthestRaySphereIntersect(contextRay, point(0.0f, 0.0f, 0.0f), 1.5f); // menuDepth
	vector vIntersect = vector(intersect);
	vIntersect.Normalize();

	//itemAngleY needed
	int size = (m_currentUILayer) ? int(m_currentUILayer->GetMenuItems().size())-1 : 0;
	int shift = (int)size / 2;
	float odd = (size % 2 != 0) ? 0.5f : 0.0f;
	int numSections = size != 0 ? 360 / int(20.0f) : 1; //itemAngle
	float deg = float(-atan2(vIntersect.x(), vIntersect.z()) * 180.0f / M_PI);
	deg -= m_headRotationYDeg;
	int index = (-int((deg) / 20.0f - odd) + shift) % numSections; //itemAngle
	OVERLAY_DEBUG_SET("index", index);
	UpdateSelectedItem(index, size);

	OVERLAY_DEBUG_SET("selected", m_selectedIndex);
	//*/
	return R_PASS;
}

point UIBar::FurthestRaySphereIntersect(const ray &r, point center, float radius) {
//*
	vector vRayCircle = static_cast<ray>(r).ptOrigin() - center;

	float bValue = static_cast<ray>(r).vDirection().dot(vRayCircle);
	float cValue = vRayCircle.dot(vRayCircle) - pow(radius, 2.0f); 
	float resultValue = pow(bValue, 2.0f) - cValue;

	OVERLAY_DEBUG_SET("res", resultValue);

	if (resultValue >= 0.0f) {
		float term = pow(resultValue, 0.5f);
		float t1 = (-1.0f) * bValue + term;
		//this result will always be smaller
		//float t2 = (-1.0f) * bValue - term;
		return r.GetOrigin() + point(t1 * r.GetVector());
	}
	//*/
	return point(0.0f, 0.0f, 0.0f);
}

RESULT UIBar::UpdateCurrentUILayer(UILayerInfo info) {

	m_pContext->ClearChildren(); //:O
	auto pLayer = CreateMenuLayer();

	int size = int(std::max(info.icons.size(), info.labels.size()));

	for (int i = 0; i < size; i++) {
		auto iconFormat = m_iconFormat;
		auto labelFormat = m_labelFormat;
		if (i < info.icons.size()) {
			iconFormat.pTexture = info.icons[i];
			iconFormat.position = point(0.0f, 0.5f, 0.5f);
		}
		if (i < info.labels.size())
			labelFormat.label = info.labels[i];
		auto pItem = pLayer->CreateMenuItem();
		pItem->Update(iconFormat, labelFormat);

		UpdateWithRadialLayout(pItem, i, size, i == size-1);
	}

	return R_PASS;
}

int UIBar::GetSelectedIndex() {
	return m_selectedIndex;
}