#include "Logger.h"
#include "callback.h"

#include<iostream>
#include<sstream>

SquirrelCallback::SquirrelCallback()
{
}

SquirrelCallback::SquirrelCallback(const std::string& event) :
	m_type(EventType::Unknown)
{
	// checks for a number
	
	std::stringstream ss;

	ss << event;
	ss >> m_progress;

	if (!ss.fail())
	{
		m_type = EventType::Progress;
		return;
	}

	// checks for a json

	try
	{
		m_json = nlohmann::json::parse(event);
		m_type = EventType::JSON;
		return;
	}
	catch (...)
	{

	}

	// unknown type
}

SquirrelCallback::EventType SquirrelCallback::GetType() const
{
	return m_type;
}

uint16_t SquirrelCallback::GetProgress() const
{
	return m_progress;
}

nlohmann::json SquirrelCallback::GetJson() const
{
	return m_json;
}

SquirrelEvent::State SquirrelEvent::OnCallback(const std::string& callback)
{
	SquirrelCallback newCb(callback);

	m_callback = callback;

	if (newCb.GetType() == SquirrelCallback::EventType::Unknown)	
		m_state = SquirrelEvent::State::Invalid;
	
	switch (m_state)
	{
		case SquirrelEvent::State::Init:
		case SquirrelEvent::State::Progress: {

			if (newCb.GetType() == SquirrelCallback::EventType::Progress)
				m_state = SquirrelEvent::State::Progress;
			else if (newCb.GetType() == SquirrelCallback::EventType::JSON)
				m_state = SquirrelEvent::State::Done;

		} break;
		case SquirrelEvent::State::Done: {

			if (newCb.GetType() == SquirrelCallback::EventType::Progress)
				m_state = SquirrelEvent::State::Invalid;
			else if (newCb.GetType() == SquirrelCallback::EventType::JSON)
				m_state = SquirrelEvent::State::Done;

		} break;
		case SquirrelEvent::State::Invalid: {
			// keeps invalid state forever
		} break;
	}

	return m_state;
}

SquirrelEvent::State SquirrelEvent::GetState() const
{
	return m_state;
}

std::ostream& operator<<(std::ostream& os, const SquirrelEvent& event)
{
	int state = static_cast<int>(event.GetState());
	os << state;
	return os;
}

const SquirrelCallback& SquirrelEvent::GetCallback() const
{
	return m_callback;
}
