#ifndef DEBUGCONSOLE_H_
#define DEBUGCONSOLE_H_

#include "Primitives/valid.h"
#include <vector>
#include <memory>

class DebugData {

public:
	const std::string& GetValue();
	void SetValue(std::string value);
	const std::string& GetName();

	DebugData(const std::string& uniqueName = "");
	~DebugData();

private:
	std::string m_uniqueName;

	std::string m_value;
};

class DebugConsole : public valid {

public:

	static DebugConsole* GetDebugConsole() {
		static DebugConsole debugConsole;
		return &debugConsole;
	}

	DebugConsole();
	~DebugConsole();

	const std::vector<std::shared_ptr<DebugData>>& GetConsoleData();
	void Unregister(std::shared_ptr<DebugData> data);
	std::shared_ptr<DebugData> Register(const std::string& uniqueName = "");
	std::shared_ptr<DebugData> Get(const std::string& uniqueName);

private:	
	std::vector<std::shared_ptr<DebugData>> m_data;
};

#endif // !DEBUGCONSOLE_H_
