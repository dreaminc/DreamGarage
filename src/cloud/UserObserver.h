#ifndef USER_OBSERVER_H_
#define USER_OBSERVER_H_

class UserObserver {
public:
	virtual RESULT OnDreamVersion(version dreamVersion) = 0;
	virtual RESULT OnAPIConnectionCheck(bool fIsConnected) = 0;

	virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) = 0;
	virtual RESULT OnSetSettings() = 0;

	virtual RESULT OnLogin() = 0;
	virtual RESULT OnLogout() = 0;
	virtual RESULT OnPendLogout() = 0;
	virtual RESULT OnSwitchTeams() = 0;

	virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) = 0;
	virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) = 0;
	virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) = 0;
};

#endif // ! USER_OBSERVER_H_