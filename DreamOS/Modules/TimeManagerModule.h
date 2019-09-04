#ifndef TIME_MANAGER_MODULE_H_
#define TIME_MANAGER_MODULE_H_

// DREAM OS
// DreamOS/Modules/TimeManagerModule.h
// Time Manager 

#include "DreamModule.h"

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


class TimeManagerModule : 
	public DreamModule<TimeManagerModule>,
	public Publisher<TimeEventType, TimeEvent>
{
	friend class DreamModuleManager;

public:
	TimeManagerModule(DreamOS *pDreamOS, void *pContext, double processingTimeQuantum = 0.0167f);
	~TimeManagerModule();

	virtual RESULT InitializeModule(void *pContext = nullptr) override;
	virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static TimeManagerModule* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	RESULT Reset();		// Resets time

	RESULT ResetMinMaxFPS();
	RESULT PrintFPS();

	double GetRunTimeFrameRate();
	long long GetTotalNumberOfFrames();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock>	m_startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_currentTime;

	double m_processingTimeQuantum;
	double m_totalElapsedTime = 0.0f;
	double m_totalTimeToProcess;

	double m_runTimeFPS = 0.0f;
	
	double m_maxFPS = 0.0f;
	double m_minFPS = 0.0f;

	long long m_numFrames = 0;
};

#endif // !TIME_MANAGER_MODULE_H_
