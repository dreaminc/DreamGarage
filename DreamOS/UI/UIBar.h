#ifndef UI_BAR_H_
#define UI_BAR_H_

#include "Primitives/composite.h"
#include "Primitives/ray.h"

#include "Sense/SenseController.h"
#include "Primitives/Publisher.h"

#include "UIModule.h"

#include "Primitives/dirty.h"

#include <stack>

typedef struct UIBarFormat {

	float menuPosZ;

	// item
	float itemAngleX;
	float itemAngleY; // angle between items
	float itemPosY;
	vector itemScale;
	float itemScaleSelected;

	// header 
	float headerAngleX;
	float headerPosY;

	UIBarFormat() :
		menuPosZ(-1.5f),
		itemAngleX(60.0f),
		itemAngleY(20.0f),
		itemPosY(-0.5f),
		itemScale(vector(1.0f, 1.0f, 1.0f)),
		itemScaleSelected(1.25f),
		headerAngleX(75.0f),
		headerPosY(0.0f)
	{}

} UI_BAR_INFO;

class UIBar : public UIModule {
public:
	UIBar(DreamOS *pDreamOS, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat);
	~UIBar();

	virtual RESULT HandleMenuUp(std::map<std::string, std::vector<std::string>>& menu, std::stack<std::string>& path) override;
	virtual RESULT HandleTriggerUp(std::map<std::string, std::vector<std::string>>& menu, std::stack<std::string>& path) override;

	virtual RESULT UpdateCurrentUILayer(UILayerInfo& info) override;

private:
	// Places MenuItem along a circular arc based on index
	RESULT UpdateWithRadialLayout(size_t index);

protected:
	IconFormat m_iconFormat;
	LabelFormat m_labelFormat;
	UIBarFormat m_barFormat;

//private:
protected:
	std::shared_ptr<texture> m_pIconTexture;

public:
	float GetLargeItemScale();
};


#endif // ! UI_BAR_H