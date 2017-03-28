#ifndef UI_BAR_H_
#define UI_BAR_H_

#include "Primitives/composite.h"
#include "Primitives/ray.h"

#include "Sense/SenseController.h"
#include "Primitives/Publisher.h"

#include "UIModule.h"

#include "Primitives/dirty.h"

#include <stack>

typedef struct RadialLayerFormat {

	float menuPosZ;

	// item
	float itemAngleX;
	float itemAngleY; // angle between items
	float itemStartAngleY;
	float itemPosY;
	vector itemScale;
	float itemScaleSelected;

	// header 
	float headerAngleX;
	float headerPosY;

	RadialLayerFormat() :
		menuPosZ(-1.0f),
		itemAngleX(60.0f),
		itemAngleY(20.0f),
		itemStartAngleY(-30.0f),
		itemPosY(-0.5f),
		itemScale(vector(1.0f, 1.0f, 1.0f)),
		itemScaleSelected(1.25f),
		headerAngleX(75.0f),
		headerPosY(-0.25f)
	{}

} UI_BAR_INFO;

class UIBar : public UIModule {
public:
	UIBar(DreamOS *pDreamOS, 
			IconFormat& iconFormat, 
			LabelFormat& labelFormat, 
			RadialLayerFormat& menuFormat,
			RadialLayerFormat& titleFormat);
	~UIBar();

	RESULT UpdateCurrentUILayer(UILayerInfo& info, RadialLayerFormat& layerFormat);
	RESULT UpdateUILayers(UILayerInfo& currentInfo, UILayerInfo& titleInfo);

private:
	// Places MenuItem along a circular arc based on index
	RESULT UpdateWithRadialLayout(size_t index, RadialLayerFormat& layerFormat);

protected:
	IconFormat m_iconFormat;
	LabelFormat m_labelFormat;
	RadialLayerFormat m_menuFormat;
	RadialLayerFormat m_titleFormat;

public:
	float GetLargeItemScale();
};


#endif // ! UI_BAR_H