#include "TimeObj.h"

TimeObj::TimeObj(double	processingTimeQuantum) : m_processingTimeQuantum(processingTimeQuantum)
{
	RegisterEvent(TIME_ELAPSED);
	reset();
}

void TimeObj::reset()
{
	m_startTime = std::chrono::high_resolution_clock::now();
	m_currentTime = m_startTime;
	m_totalElapsedTime = 0;
	m_totalTimeToProcess = 0;
}

void TimeObj::update()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto deltaTime = std::chrono::duration<double>(now - m_currentTime).count();
	m_currentTime = now;

	m_totalTimeToProcess += deltaTime;

	while (m_totalTimeToProcess >= m_processingTimeQuantum) {
		//this->onTimeUpdate(m_totalElapsedTime, m_processingTimeQuantum);
		TimeEvent event(TIME_ELAPSED, m_totalElapsedTime, m_processingTimeQuantum);
		NotifySubscribers(TIME_ELAPSED, &event);

		m_totalTimeToProcess -= m_processingTimeQuantum;
		m_totalElapsedTime += m_processingTimeQuantum;
	}
}

void TimeObj::addTimeObject(TimeObject* pObj)
{
	m_timeObjects.push_back(pObj);
}

void TimeObj::onTimeUpdate(double currentTime, double deltaTime)
{
	for (auto& obj : m_timeObjects)
	{
		obj->onTimeUpdate(currentTime, deltaTime);
	}
}

