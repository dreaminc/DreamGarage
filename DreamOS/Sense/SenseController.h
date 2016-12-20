#ifndef SENSE_CONTROLLER_H_
#define SENSE_CONTROLLER_H_

#include "Primitives/valid.h"

#include "Primitives/Publisher.h"
#include "Primitives/Subscriber.h"
#include "Primitives/point.h"
#include "SenseDevice.h"


class SenseController : public SenseDevice, public valid {
public:
	SenseController();
	~SenseController();

private:
	float m_trigger;
	point m_touchpad;
	bool m_grip;
	bool m_menu;
};

#endif // ! SENSE_CONTROLLER_H_