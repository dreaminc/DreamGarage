#ifndef MENU_NODE_H_
#define MENU_NODE_H_

#include "RESULT/EHM.h"
#include "Primitives/dirty.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Menu/MenuNode.h
// Menu Node Object

#include <string>
#include <vector>

#include "json.hpp"

class MenuNode : public dirty {
	enum class type {
		FOLDER,
		FILE,
		INVALID
	};
	
public:
	MenuNode(nlohmann::json jsonMenuNode);
	MenuNode(MenuNode::type nodeType, std::string strPath, std::string strScope, std::string strTitle);

	std::string NodeTypeString(MenuNode::type nodeType);
	MenuNode::type NodeTypeFromString(std::string strNodeType);

	RESULT PrintMenuNode();
	size_t NumSubMenuNodes();

	const std::string& GetPath();
	const std::string& GetScope();
	const std::string& GetTitle();

	std::vector<std::shared_ptr<MenuNode>> GetSubMenuNodes();

private:
	MenuNode::type m_nodeType;
	std::string m_strPath;
	std::string m_strScope;
	std::string m_strTitle;

	std::vector<std::shared_ptr<MenuNode>> m_menuNodes;
};

#endif	// ! ENVIRONMENT_H_