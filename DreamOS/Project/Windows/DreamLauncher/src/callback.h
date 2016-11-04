#pragma once

#include "json.hpp"

#include <string>

class SquirrelCallback
{
public:
	enum class EventType
	{
		Unknown,
		Progress,
		JSON
	};

	SquirrelCallback();
	SquirrelCallback(const std::string& event);

	EventType GetType() const;
	uint16_t GetProgress() const;
	nlohmann::json GetJson() const;

private:
	EventType		m_type{ EventType::Unknown };
	nlohmann::json	m_json;
	uint16_t		m_progress;
};

class SquirrelEvent
{
public:
	enum class State
	{
		Init,
		Progress,
		Done,
		Invalid
	};

	State OnCallback(const std::string& callback);
	State GetState() const;
	const SquirrelCallback& GetCallback() const;

	friend std::ostream& operator<<(std::ostream& os, const SquirrelEvent& event);

private:
	State		m_state{ State::Init };

	SquirrelCallback	m_callback;
};

std::ostream& operator<<(std::ostream& os, const SquirrelEvent& event);
