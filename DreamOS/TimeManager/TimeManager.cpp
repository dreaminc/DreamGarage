#include "TimeManager.h"

TimeManager::TimeManager(double	processingTimeQuantum) : 
	m_processingTimeQuantum(processingTimeQuantum),
	m_totalElapsedTime(0.0f),
	m_totalTimeToProcess(0.0f),
	m_startTime(std::chrono::high_resolution_clock::now()),
	m_currentTime(std::chrono::high_resolution_clock::now())
{
	RESULT r = R_PASS;

	RegisterEvent(TIME_ELAPSED);
	Reset();

	Validate();
	return;
/*TODO: Reference this error case
Error:
	Invalidate();
	return;*/
}

TimeManager::~TimeManager() {
	// empty
}

RESULT TimeManager::Reset() {
	m_startTime = std::chrono::high_resolution_clock::now();

	m_currentTime = m_startTime;
	m_totalElapsedTime = 0;
	m_totalTimeToProcess = 0;

	return R_PASS;
}

RESULT TimeManager::Update() {

	auto now = std::chrono::high_resolution_clock::now();
	auto deltaTime = std::chrono::duration<double>(now - m_currentTime).count();
	m_currentTime = now;

	m_totalTimeToProcess += deltaTime;

	while (m_totalTimeToProcess >= m_processingTimeQuantum) {
		TimeEvent event(TIME_ELAPSED, m_totalElapsedTime, m_processingTimeQuantum);
		NotifySubscribers(TIME_ELAPSED, &event);

		m_totalTimeToProcess -= m_processingTimeQuantum;
		m_totalElapsedTime += m_processingTimeQuantum;
	}

	return R_PASS;
}