#ifndef UI_MENU_ITEM_H_
#define UI_MENU_ITEM_H_

#include "Primitives/valid.h"
#include "Primitives/FlatContext.h"
#include "Primitives/composite.h"
#include "InteractionEngine/InteractionObjectEvent.h"

// IconFormat defaults
#define ICON_HEIGHT 1.5f
#define ICON_WIDTH 1.5f

struct IconFormat
{
	std::shared_ptr<texture> pTexture;
	float height;
	float width;
	point ptPosition;

	IconFormat() :
		pTexture(nullptr),
		height(ICON_HEIGHT),
		width(ICON_WIDTH),
		ptPosition(0.0f, 0.25f, 0.0f)
	{}
};

#define LABEL_SIZE 0.8f

struct LabelFormat
{
	std::shared_ptr<Font> pFont;
	std::string strLabel;
	float fontSize;
	point ptPosition;
	vector vScale;

	LabelFormat() :
		pFont(std::make_shared<Font>(L"Basis_Grotesque_Pro.fnt", true)),
		strLabel(""),
		fontSize(LABEL_SIZE),
		ptPosition(0.0f, -0.9f, 0.0f),
		vScale(1.0f, 1.0f, 1.0f)
	{}
};

class UIMenuItem : public valid {
public:
	UIMenuItem(std::shared_ptr<composite> pParentComposite);

	RESULT Initialize();

	std::shared_ptr<composite> GetContext();
	std::shared_ptr<quad> GetQuad();


	RESULT Update(IconFormat& iconFormat, LabelFormat& labelFormat);
	RESULT SetObjectParams(point ptQuad, quaternion qQuad, point ptContext, quaternion qContext);

	bool Contains(VirtualObj* pObj);

	std::string& GetName();

private:
	std::shared_ptr<composite> m_pContextComposite;
	std::shared_ptr<quad> m_pQuad;
	std::shared_ptr<quad> m_pQuadInvisible;
	std::string m_strName;

private:
	std::shared_ptr<composite> m_pParentContext = nullptr;
};


#endif // ! UI_MENU_ITEM_H_