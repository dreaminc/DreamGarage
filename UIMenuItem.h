#ifndef UI_MENU_ITEM_H_
#define UI_MENU_ITEM_H_

#include "Primitives/FlatContext.h"
#include "Primitives/composite.h"

struct IconFormat
{
	std::shared_ptr<texture> pTexture;
	float height;
	float width;
	point position;

	IconFormat() :
		pTexture(nullptr),
		height(0.5f),
		width(0.5f),
		position(0.0f, 0.0f, 0.0f)
	{}
};

struct LabelFormat
{
	const std::wstring font;
	std::string label;
	float fontSize;
	point position;
	vector scale;

	LabelFormat() :
		font(L"ArialDistance.fnt"),
		label(""),
		fontSize(0.4f),
		position(0.0f, 0.0f, 0.0f),
		scale(1.0f, 1.0f, 1.0f)
	{}
};

class UIMenuItem {
public:
	UIMenuItem(composite* c);
	~UIMenuItem();

	RESULT Initialize();

	std::shared_ptr<composite> GetContext();
	std::shared_ptr<quad> GetQuad();


	RESULT Update(IconFormat iconFormat = IconFormat(), LabelFormat labelFormat = LabelFormat());

private:
	std::shared_ptr<composite> m_pContext;
	std::shared_ptr<quad> m_pQuad;
};


#endif // ! UI_MENU_ITEM_H_