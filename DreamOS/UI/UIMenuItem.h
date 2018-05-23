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
#define LABEL_WIDTH_SCALE 0.9 
#define LABEL_HEIGHT_SCALE 0.28125
#define LABEL_HEIGHT_POSITION -0.46f
#define LABEL_LINE_HEIGHT_SCALE 0.1f

struct LabelFormat
{
	std::shared_ptr<font> pFont;
	std::string strLabel;
	float fontSize;
	point ptPosition;
	vector vScale;

	color bgColor;
	texture *pBgTexture;

	//TODO: a context is used to create the default texture off of the default font
	//	set manually for now
	LabelFormat() :
		//pFont(std::make_shared<Font>(L"Basis_Grotesque_Pro.fnt", true)),
		pFont(nullptr),
		strLabel(""),
		fontSize(LABEL_SIZE),
		ptPosition(0.0f, -0.115f, 0.0f),
		vScale(1.0f, 1.0f, 1.0f),
		bgColor(0.0f, 0.0f, 0.0f, 0.65f),
		pBgTexture(nullptr)
	{}
};

class UIMenuItem : public UIButton {
public:
	UIMenuItem(HALImp *pHALImp, DreamOS *pDreamOS);
	UIMenuItem(HALImp *pHALImp, DreamOS *pDreamOS, float width, float height);

	RESULT Initialize();

	std::shared_ptr<composite> GetContext();
	std::shared_ptr<quad> GetQuad();

	RESULT Update(IconFormat& iconFormat, LabelFormat& labelFormat);
	RESULT SetObjectParams(point ptQuad, quaternion qQuad, point ptContext, quaternion qContext);

	bool Contains(VirtualObj* pObj);

	std::string& GetName();


private:
	std::shared_ptr<text> m_pLabel = nullptr;
	std::string m_strName;

private:
	std::shared_ptr<composite> m_pParentContext = nullptr;
};


#endif // ! UI_MENU_ITEM_H_