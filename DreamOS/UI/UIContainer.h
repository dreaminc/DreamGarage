#ifndef UI_CONTAINER_H_
#define UI_CONTAINER_H_

#include "UIView.h"

class UIContainer : public UIView {
public:
	UIContainer(HALImp *pHALImp);
	~UIContainer();

private:
	std::vector<std::shared_ptr<UIView>> m_pViews;
};

#endif // ! UI_CONTAINER_H_ 