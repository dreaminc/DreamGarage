#include "MenuNode.h"

#include "Core/Utilities.h"

MenuNode::MenuNode() {
	// empty
}

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

	if (jsonMenuNode["/next_page_token"_json_pointer].is_string()) {
		m_strNextPageToken = jsonMenuNode["/next_page_token"_json_pointer].get<std::string>();
	}

	if (jsonMenuNode["/icon_url"_json_pointer].is_string())
		m_strIconURL = jsonMenuNode["/icon_url"_json_pointer].get<std::string>();

	if (jsonMenuNode["/thumbnail_url"_json_pointer].is_string())
		m_strThumbnailURL = jsonMenuNode["/thumbnail_url"_json_pointer].get<std::string>();

	InitializeMimeToString();
}

MenuNode::MenuNode(MenuNode::type nodeType, std::string strPath, std::string strScope, std::string strTitle, std::string strMIMEType) :
	m_nodeType(nodeType),
	m_strPath(strPath),
	m_strScope(strScope),
	m_strTitle(strTitle),
	m_strMIMEType(strMIMEType)
{
	InitializeMimeToString();
}

MenuNode::MenuNode(MenuNode::type nodeType, std::string strPath, std::string strScope, std::string strTitle, std::string strMIMEType, std::string strIconURL, std::string strThumbnailURL) :
	m_nodeType(nodeType),
	m_strPath(strPath),
	m_strScope(strScope),
	m_strTitle(strTitle),
	m_strMIMEType(strMIMEType),
	m_strIconURL(strIconURL),
	m_strThumbnailURL(strThumbnailURL)
{
	InitializeMimeToString();
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
		case MenuNode::type::ACTION: return "action"; break;
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
		else if (strTokens[1] == "action")
			return MenuNode::type::ACTION;
	}

	return MenuNode::type::INVALID;
}

RESULT MenuNode::InitializeMimeToString() {

	m_MimeToString[MimeType::IMAGE_JPG] = { "image/jpg", "image/jpeg" };
	m_MimeToString[MimeType::IMAGE_BMP] = { "image/bmp" };
	m_MimeToString[MimeType::IMAGE_PNG] = { "image/png" };
	m_MimeToString[MimeType::IMAGE_GIF] = { "image/gif" };
	m_MimeToString[MimeType::FOLDER] = { "application/folder", "application/vnd.google-apps.folder" };
	m_MimeToString[MimeType::INVALID] = { "invalid" };

	return R_PASS;
}

std::string MenuNode::MimeTypeString(MenuNode::MimeType mimeType) {
	return m_MimeToString[mimeType][0];
}

MenuNode::MimeType MenuNode::MimeTypeFromString(std::string strMimeType) {
	for (auto& key : m_MimeToString) {
		for (auto& str : key.second) {
			if (str == strMimeType) {
				return key.first;
			}
		}
	}
	return MenuNode::MimeType::INVALID;
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

const std::string& MenuNode::GetNextPageToken() {
	return m_strNextPageToken;
}

const std::string& MenuNode::GetIconURL() {
	return m_strIconURL;
}

const std::string& MenuNode::GetThumbnailURL() {
	return m_strThumbnailURL;
}

const MenuNode::type& MenuNode::GetNodeType() {
	return m_nodeType;
}

texture* MenuNode::GetThumbnailTexture() {
	return m_pThumbnailTexture;
}

std::shared_ptr<UIButton> MenuNode::GetAssociatedButton() {
	return m_pUIButton;
}

RESULT MenuNode::SetName(std::string strName) {
	m_strTitle = strName;
	return R_PASS;
}

RESULT MenuNode::SetThumbnailTexture(texture* pTexture) {
	
	m_pThumbnailTexture = pTexture;

	return R_PASS;
}

RESULT MenuNode::SetAssociatedButton(std::shared_ptr<UIButton> pButton) {
	m_pUIButton = pButton;
	return R_PASS;
}
