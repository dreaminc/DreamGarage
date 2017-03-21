#include "MenuNode.h"

#include "Core/Utilities.h"

MenuNode::MenuNode(nlohmann::json jsonMenuNode) {
	if (jsonMenuNode["/node_type"_json_pointer].is_string()) {
		std::string strNodeType = jsonMenuNode["/node_type"_json_pointer].get<std::string>();
		m_nodeType = NodeTypeFromString(strNodeType);
	}

	if(jsonMenuNode["/title"_json_pointer].is_string())
		m_strTitle = jsonMenuNode["/title"_json_pointer].get<std::string>();
	
	if (jsonMenuNode["/path"_json_pointer].is_string())
		m_strPath = jsonMenuNode["/path"_json_pointer].get<std::string>();

	if (jsonMenuNode["/scope"_json_pointer].is_string())
		m_strScope = jsonMenuNode["/scope"_json_pointer].get<std::string>();

	if (jsonMenuNode["/mime_type"_json_pointer].is_string())
		m_strMIMEType = jsonMenuNode["/mime_type"_json_pointer].get<std::string>();

	if (jsonMenuNode["/submenu"_json_pointer].is_array()) {
		for (auto &subMenuNode : jsonMenuNode["/submenu"_json_pointer]) {
			std::shared_ptr<MenuNode> pSubMenuNode = std::make_shared<MenuNode>(subMenuNode);
			m_menuNodes.push_back(pSubMenuNode);
		}
	}
}

MenuNode::MenuNode(MenuNode::type nodeType, std::string strPath, std::string strScope, std::string strTitle, std::string strMIMEType) :
	m_nodeType(nodeType),
	m_strPath(strPath),
	m_strScope(strScope),
	m_strTitle(strTitle),
	m_strMIMEType(strMIMEType)
{
	// empty
}

RESULT MenuNode::PrintMenuNode() {
	DEBUG_LINEOUT("Node Type %s", NodeTypeString(m_nodeType).c_str());
	DEBUG_LINEOUT("Node title: %s", m_strTitle.c_str());
	DEBUG_LINEOUT("Node path: %s", m_strPath.c_str());
	DEBUG_LINEOUT("Node scope: %s", m_strScope.c_str());
	DEBUG_LINEOUT("Node MIME type: %s", m_strMIMEType.c_str());

	if (m_menuNodes.size() > 0) {
		DEBUG_LINEOUT("Sub Menu Nodes:");

		for (auto &pSubMenuNode : m_menuNodes) {
			pSubMenuNode->PrintMenuNode();
		}
	}

	return R_PASS;
}

std::string MenuNode::NodeTypeString(MenuNode::type nodeType) {
	switch (nodeType) {
		case MenuNode::type::FOLDER: return "folder"; break;
		case MenuNode::type::FILE: return "file"; break;
		default: return "invalid"; break;
	}

	return "invalid";
}

MenuNode::type MenuNode::NodeTypeFromString(std::string strNodeType) {
	auto strTokens = util::TokenizeString(strNodeType, '.');
	util::tolowerstring(strTokens[1]);

	if (strTokens.size() == 2) {
		if (strTokens[1] == "folder")
			return MenuNode::type::FOLDER;
		else if (strTokens[1] == "file")
			return MenuNode::type::FILE;

	}

	return MenuNode::type::INVALID;
}

size_t MenuNode::NumSubMenuNodes() {
	return m_menuNodes.size();
}

std::vector<std::shared_ptr<MenuNode>> MenuNode::GetSubMenuNodes() {
	return m_menuNodes;
}

const std::string& MenuNode::GetPath() {
	return m_strPath;
}

const std::string& MenuNode::GetScope() {
	return m_strScope;
}

const std::string& MenuNode::GetMIMEType() {
	return m_strMIMEType;
}

const std::string& MenuNode::GetTitle() {
	return m_strTitle;
}
