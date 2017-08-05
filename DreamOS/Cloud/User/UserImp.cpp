#include "Logger/Logger.h"
#include "User.h"

std::ostream& operator<<(std::ostream& os, const User& user) {
	os << user.GetScreenName() << "," << user.GetEmail() << ",uid=" << user.GetUserID() << ",eid=" << user.GetDefaultEnvironmentID();
	return os;
}