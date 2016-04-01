#ifndef TIME_OBJ_H_
#define TIME_OBJ_H_

// DREAM OS
// DreamOS/Dimension/Primitives/TimeObj.h
// Time Object

#include "valid.h"
#include "Publisher.h"
#include <vector>
#include <chrono>

typedef enum TimeEventType {
	TIME_ELAPSED,
} TIME_EVENT_TYPE;

typedef struct TimeEvent {
	TimeEventType EventType;

	double m_currentTime;
	double m_deltaTime;

	TimeEvent(TimeEventType eventType, double currentTime, double deltaTime) :
		EventType(eventType), m_currentTime(currentTime), m_deltaTime(deltaTime)
	{

	}
} TIME_EVENT;


class TimeObj : public Publisher<TimeEventType, TimeEvent>, valid {
public:
	TimeObj(double	processingTimeQuantum = 0.0167);

	// Resets time
	void reset();

	// Updates the time from previous call to update.
	void update();

private:
	void onTimeUpdate(double currentTime, double deltaTime);

	std::chrono::time_point<std::chrono::high_resolution_clock>	m_startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_currentTime;

	double m_processingTimeQuantum;
	double m_totalElapsedTime;
	double m_totalTimeToProcess;
};

#endif // !TIME_OBJ_H_
