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

// FPS Run Rate Filter
#define NUM_MICROSECONDS_IN_SECOND 1000000.0f
#define FPS_RRF_VALUE 1.0f
#define FPS_RRF_RESOLUTION 50.0f
#define FPS_RRF_XVAL (FPS_RRF_VALUE/FPS_RRF_RESOLUTION)
#define FPS_RRF_YVAL ((FPS_RRF_RESOLUTION - FPS_RRF_VALUE)/FPS_RRF_RESOLUTION)

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

	RESULT ResetMinMaxFPS();
	RESULT PrintFPS();

	double GetRunTimeFrameRate();
	long long GetTotalNumberOfFrames();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock>	m_startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_currentTime;

	double m_processingTimeQuantum;
	double m_totalElapsedTime;
	double m_totalTimeToProcess;

	double m_runTimeFPS = 0;
	
	double m_maxFPS = 0;
	double m_minFPS = 0;

	long long m_numFrames = 0;

public:
	UID getID() { return m_uid; }

private:
	UID m_uid;
};

#endif // !TIME_MANAGER_H_
