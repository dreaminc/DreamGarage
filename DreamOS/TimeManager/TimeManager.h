#ifndef TIME_MANAGER_H_
#define TIME_MANAGER_H_

// DREAM OS
// DreamOS/Dimension/Primitives/TimeManager.h
// Time Manager 

#include "Primitives/valid.h"
#include "Primitives/Types/UID.h"
#include "Primitives/Publisher.h"

#include <vector>
#include <chrono>

// TODO: This should be made into a singleton

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
		// empty
	}
} TIME_EVENT;


class TimeManager : public Publisher<TimeEventType, TimeEvent>, public valid {
public:
	TimeManager(double	processingTimeQuantum = 0.0167);
	~TimeManager();

	RESULT Reset();		// Resets time
	RESULT Update();	// Updates the time from previous call to update.

private:
	std::chrono::time_point<std::chrono::high_resolution_clock>	m_startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_currentTime;

	double m_processingTimeQuantum;
	double m_totalElapsedTime;
	double m_totalTimeToProcess;

public:
	UID getID() { return m_uid; }

private:
	UID m_uid;
};

#endif // !TIME_MANAGER_H_
