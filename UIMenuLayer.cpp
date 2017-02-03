#include "UIMenuLayer.h"

UIMenuLayer::UIMenuLayer(composite* c) :
	m_context(c)
{
	// empty
}

UIMenuLayer::~UIMenuLayer() 
{
	// empty
}

RESULT UIMenuLayer::Initialize()
{
	return R_PASS;
}

std::shared_ptr<UIMenuItem> UIMenuLayer::CreateMenuItem()
{
	return nullptr;
}

RESULT UIMenuLayer::Clear()
{
	return R_PASS;
}