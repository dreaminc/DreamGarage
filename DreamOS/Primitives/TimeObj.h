#ifndef TIME_OBJ_H_
#define TIME_OBJ_H_

// DREAM OS
// DreamOS/Dimension/Primitives/TimeObj.h
// Time Object

#include "valid.h"
#include <vector>
#include <chrono>

class TimeObject {
public:
	virtual void onTimeUpdate(double currentTime, double deltaTime) = 0;
};

class TimeObj : TimeObject, valid {
public:
	TimeObj(double	processingTimeQuantum = 0.0167);

	// Resets time
	void reset();

	// Updates the time from previous call to update.
	void update();

	void addTimeObject(TimeObject* pObj);

private:
	void onTimeUpdate(double currentTime, double deltaTime);

	std::chrono::time_point<std::chrono::high_resolution_clock>	m_startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_currentTime;

	double m_processingTimeQuantum;
	double m_totalElapsedTime;
	double m_totalTimeToProcess;

	std::vector<TimeObject*> m_timeObjects;
};

#endif // !TIME_OBJ_H_
