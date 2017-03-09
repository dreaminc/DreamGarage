#include "Utilities.h"

#include <algorithm>

std::vector<std::string> util::TokenizeString(std::string str, char cDelim) {
	std::istringstream strStream(str);
	std::vector<std::string> strTokens;
	std::string strToken;

	while (std::getline(strStream, strToken, cDelim)) {
		if (!strToken.empty()) {
			strTokens.push_back(strToken);
		}
	}

	return strTokens;
}

void util::tolowerstring(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

