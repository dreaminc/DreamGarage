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
public:
	enum class type {
		FOLDER,
		FILE,
		ACTION,
		INVALID
	};

public:
	enum class MimeType {
		IMAGE_JPG,
		IMAGE_PNG,
		IMAGE_BMP,
		IMAGE_GIF,
		FOLDER,
		INVALID
	};
	
public:
	MenuNode();
	MenuNode(nlohmann::json jsonMenuNode);
	MenuNode(MenuNode::type nodeType, std::string strPath, std::string strScope, std::string strTitle, std::string strMIMEType);
	MenuNode(MenuNode::type nodeType, std::string strPath, std::string strScope, std::string strTitle, std::string strMIMEType, std::string strIconURL, std::string strThumbnailURL);

	std::string NodeTypeString(MenuNode::type nodeType);
	MenuNode::type NodeTypeFromString(std::string strNodeType);

	std::string MimeTypeString(MenuNode::MimeType mimeType);
	MenuNode::MimeType MimeTypeFromString(std::string strMimeType);

	RESULT PrintMenuNode();
	size_t NumSubMenuNodes();

	const std::string& GetPath();
	const std::string& GetScope();
	const std::string& GetMIMEType();
	const std::string& GetTitle();
	const std::string& GetIconURL();
	const std::string& GetThumbnailURL();
	const std::shared_ptr<std::vector<uint8_t>>& GetTextureBufferVector();

	const MenuNode::type& GetNodeType();

	RESULT SetName(std::string strName);
	RESULT SetTextureBufferVector(std::shared_ptr<std::vector<uint8_t>> &pBufferVector);

	std::vector<std::shared_ptr<MenuNode>> GetSubMenuNodes();

private:
	MenuNode::type m_nodeType;
	std::string m_strPath;
	std::string m_strScope;
	std::string m_strTitle;
	std::string m_strMIMEType;
	std::string m_strIconURL;
	std::string m_strThumbnailURL;

	std::shared_ptr<std::vector<uint8_t>> m_pTextureBufferVector = nullptr;

	std::vector<std::shared_ptr<MenuNode>> m_menuNodes;

	std::map<MimeType, std::vector<std::string>> m_MimeToString;

	RESULT InitializeMimeToString();
};

#endif	// ! ENVIRONMENT_H_
