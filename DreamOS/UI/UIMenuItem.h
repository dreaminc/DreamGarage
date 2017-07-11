#ifndef UI_MENU_ITEM_H_
#define UI_MENU_ITEM_H_

#include "Primitives/valid.h"
#include "Primitives/FlatContext.h"
#include "Primitives/composite.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "UI/UIButton.h"

class DreamOS;

// IconFormat defaults
#define ICON_HEIGHT 1.5f
#define ICON_WIDTH 1.5f

struct IconFormat
{
	texture* pTexture;
	float height;
	float width;
	point ptPosition;

	IconFormat() :
		pTexture(nullptr),
		height(ICON_HEIGHT),
		width(ICON_WIDTH),
		ptPosition(0.0f, 0.35f, 0.0f)
	{}
};

#define LABEL_SIZE 0.8f

struct LabelFormat
{
	std::shared_ptr<font> pFont;
	std::string strLabel;
	float fontSize;
	point ptPosition;
	vector vScale;

	//TODO: a context is used to create the default texture off of the default font
	//	set manually for now
	LabelFormat() :
		//pFont(std::make_shared<Font>(L"Basis_Grotesque_Pro.fnt", true)),
		pFont(nullptr),
		strLabel(""),
		fontSize(LABEL_SIZE),
		ptPosition(0.0f, -0.25f, 0.0f),
		vScale(1.0f, 1.0f, 1.0f)
	{}
};

class UIMenuItem : public UIButton {
public:
	UIMenuItem(HALImp *pHALImp, DreamOS *pDreamOS);

	RESULT Initialize();

	std::shared_ptr<composite> GetContext();
	std::shared_ptr<quad> GetQuad();

	RESULT Update(IconFormat& iconFormat, LabelFormat& labelFormat);
	RESULT SetObjectParams(point ptQuad, quaternion qQuad, point ptContext, quaternion qContext);

	bool Contains(VirtualObj* pObj);

	std::string& GetName();

private:
	std::string m_strName;

private:
	std::shared_ptr<composite> m_pParentContext = nullptr;
};


#endif // ! UI_MENU_ITEM_H_