#ifndef ENVIRONMENT_OBSERVER_H_
#define ENVIRONMENT_OBSERVER_H_

class EnvironmentObserver {
public:
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
	virtual RESULT OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
	virtual RESULT OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
	virtual RESULT OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
	virtual RESULT OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
	virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) = 0;
	virtual RESULT OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
	virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;

	virtual RESULT OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
	virtual RESULT OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
	virtual RESULT OnSendCameraPlacement() = 0;
	virtual RESULT OnStopSendingCameraPlacement() = 0;
	virtual RESULT OnReceiveCameraPlacement(long userID) = 0;
	virtual RESULT OnStopReceivingCameraPlacement() = 0;
};

#endif // ! ENVIRONMENT_OBSERVER_H_