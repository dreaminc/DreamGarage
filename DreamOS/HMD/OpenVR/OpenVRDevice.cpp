#include "OpenVRDevice.h"

#include "Sandbox/SandboxApp.h"

#include <stdint.h>

#include "DreamConsole/DreamConsole.h"

#include "OpenVRHMDSinkNode.h"

#include "OpenVRController.h"

OpenVRDevice::OpenVRDevice(SandboxApp *pParentSandbox) :
	HMD(pParentSandbox),
	m_pIVRHMD(nullptr),
	m_pRenderModels(nullptr),
	m_pControllerMeshLeft(nullptr),
	m_pControllerMeshRight(nullptr),
	m_pHMDModel(nullptr)
{
	// TODO
}
OpenVRDevice::~OpenVRDevice() {
	if (m_pIVRHMD != nullptr) {
		delete m_pIVRHMD;
		m_pIVRHMD = nullptr;
	}

	vr::VR_Shutdown();
}

// TODO: Implement these
RESULT OpenVRDevice::InitializeHMDSourceNode() {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::InitializeHMDSinkNode() {
	RESULT r = R_PASS;

	OpenGLImp *pOGLImp = dynamic_cast<OpenGLImp*>(m_pHALImp);
	CN(pOGLImp);

	m_pOpenVRHMDSinkNode = new OpenVRHMDSinkNode(pOGLImp, this);
	CN(m_pOpenVRHMDSinkNode);

	CR(m_pOpenVRHMDSinkNode->OGLInitialize());
	CR(m_pOpenVRHMDSinkNode->SetupConnections());

Error:
	return r;
}

HMDSinkNode* OpenVRDevice::GetHMDSinkNode() {
	return (HMDSinkNode*)(m_pOpenVRHMDSinkNode);
}

std::string OpenVRDevice::GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError) {
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

RESULT OpenVRDevice::InitializeHMD(HALImp *halimp, int wndWidth, int wndHeight) {
	RESULT r = R_PASS;
	vr::EVRInitError ivrResult = vr::VRInitError_None;
	m_pHALImp = halimp;

	m_pIVRHMD = vr::VR_Init(&ivrResult, vr::VRApplication_Scene);
	CNM(m_pIVRHMD, "Failed to initialize and allocate IVR HMD");
	CIVRM(ivrResult, "Unable to initialize IVR runtime");

	m_strDriver = "No Driver";
	m_strDisplay = "No Display";

	m_strDriver = GetTrackedDeviceString(m_pIVRHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	m_strDisplay = GetTrackedDeviceString(m_pIVRHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);

	// Eye Widths
	m_pIVRHMD->GetRecommendedRenderTargetSize(&m_eyeWidth, &m_eyeHeight);


	CRM(InitializeRenderModels(), "Failed to load render models");
	
	// TODO: Fix this
	//m_pSenseController = new OVRTouchController(m_ovrSession);
	m_pSenseController = new OpenVRController(m_pIVRHMD);
	CN(m_pSenseController);
	CR(m_pSenseController->Initialize());

	OVERLAY_DEBUG_OUT("HMD Vive - On");

Error:
	return r;
}

void IVRThreadSleep(unsigned long nMilliseconds) {
#if defined(_WIN32)
	::Sleep(nMilliseconds);
#elif defined(POSIX)
	usleep(nMilliseconds * 1000);
#endif
}

// TODO: Use a more generic interface for this
RESULT OpenVRDevice::SetControllerMeshTexture(mesh *pMesh, texture *pTexture, vr::ETrackedControllerRole controllerRole) {
	RESULT r = R_PASS;

	if (controllerRole == vr::TrackedControllerRole_LeftHand) {
		m_pControllerMeshLeft = pMesh;
		m_pControllerMeshLeftTexture = pTexture;
	}
	else if (controllerRole == vr::TrackedControllerRole_RightHand) {
		m_pControllerMeshRight = pMesh;
		m_pControllerMeshRightTexture = pTexture;
	}
	else if (controllerRole == vr::TrackedControllerRole_Invalid) {
		if (m_pControllerMeshLeft == nullptr) {
			m_pControllerMeshLeft = pMesh;
			m_pControllerMeshLeftTexture = pTexture;
		}
		else if (m_pControllerMeshRight == nullptr) {
			m_pControllerMeshRight = pMesh;
			m_pControllerMeshRightTexture = pTexture;
		}
		else {
			CBM((0), "Invalid controller role and both controllers already set");
		}
	}

Error:
	return r;
}

RESULT OpenVRDevice::InitializeRenderModel(uint32_t deviceID) {
	RESULT r = R_PASS;

	vr::RenderModel_t *pRenderModel = nullptr;
	vr::RenderModel_TextureMap_t *pRenderModelTexture = nullptr;
	vr::EVRRenderModelError error = vr::VRRenderModelError_None;
	std::vector<vertex> verts;
	std::vector<dimindex> indices;

	std::string sRenderModelName = GetTrackedDeviceString(m_pIVRHMD, deviceID, vr::Prop_RenderModelName_String);
	CB((sRenderModelName.length() > 0));

	while (1) {
		error = vr::VRRenderModels()->LoadRenderModel_Async(sRenderModelName.c_str(), &pRenderModel);
		if (error == vr::VRRenderModelError_Loading) {
			IVRThreadSleep(1);
		}
		else {
			break;
		}
	}
	CBM((error == vr::VRRenderModelError_None), "Failed to load %s model", sRenderModelName.c_str());

	while (1) {
		error = vr::VRRenderModels()->LoadTexture_Async(pRenderModel->diffuseTextureId, &pRenderModelTexture);
		if (error == vr::VRRenderModelError_Loading) {
			IVRThreadSleep(1);
		}
		else {
			break;
		}
	}
	CBM((error == vr::VRRenderModelError_None), "Failed to load %s model", sRenderModelName.c_str());

	// TODO: Load these as models now
	for (uint32_t i = 0; i < pRenderModel->unVertexCount; i++) {
		vr::RenderModel_Vertex_t pVertex = pRenderModel->rVertexData[i];

		point ptVert = point(pVertex.vPosition.v[0], pVertex.vPosition.v[1], pVertex.vPosition.v[2]);
		vector vVertNormal = vector(pVertex.vNormal.v[0], pVertex.vNormal.v[1], pVertex.vNormal.v[2]);
		uvcoord uvVert = uvcoord(pVertex.rfTextureCoord[0], pVertex.rfTextureCoord[1]);

		vertex newVert = vertex(ptVert, vVertNormal, uvVert);
		verts.push_back(newVert);
	}

	for (uint32_t i = 0; i < pRenderModel->unTriangleCount; i++) {
		dimindex indA = (dimindex)pRenderModel->rIndexData[(i * 3)];
		dimindex indB = (dimindex)pRenderModel->rIndexData[(i * 3) + 1];
		dimindex indC = (dimindex)pRenderModel->rIndexData[(i * 3) + 2];

		// TODO: THIS
		//TriangleIndexGroup triGroup = TriangleIndexGroup(indA, indB, indC);

		indices.push_back(indA);
		indices.push_back(indB);
		indices.push_back(indC);
	}

	CBM((verts.size() == pRenderModel->unVertexCount), "Vertex count mismatch");
	CBM((indices.size() == (pRenderModel->unTriangleCount * 3)), "Index count mismatch");
	
	//model *pModel = nullptr;
	mesh *pControllerMesh = m_pParentSandbox->AddMesh(verts, indices);
	//pModel->SetMaterialAmbient(1.0f);
	CNM(pControllerMesh, "Open VR Controller Models failed to load");

	/*
	uint16_t unWidth, unHeight; // width and height of the texture map in pixels
	const uint8_t *rubTextureMapData;	// Map texture data. All textures are RGBA with 8 bits per channel per pixel. Data size is width * height * 4ub
	*/
	int width = pRenderModelTexture->unWidth;
	int height = pRenderModelTexture->unHeight;
	int channels = 4;
	void *pBuffer = (void*)(pRenderModelTexture->rubTextureMapData);
	int pBuffer_n = sizeof(uint8_t) * width * height * channels;

	texture *pTexture = m_pParentSandbox->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, width, height, PIXEL_FORMAT::Unspecified, channels, pBuffer, pBuffer_n);
	pControllerMesh->SetDiffuseTexture(pTexture);

	vr::ETrackedControllerRole controllerRole = m_pIVRHMD->GetControllerRoleForTrackedDeviceIndex(deviceID);
	CR(SetControllerMeshTexture(pControllerMesh, pTexture, controllerRole))

Error:
	if(pRenderModel != nullptr) {
		vr::VRRenderModels()->FreeRenderModel(pRenderModel);
		pRenderModel = nullptr;
	}

	if (pRenderModelTexture != nullptr) {
		vr::VRRenderModels()->FreeTexture(pRenderModelTexture);
		pRenderModelTexture = nullptr;
	}

	return r;
}

// TODO: Might not want to have this here (move to sandbox or sense)
RESULT OpenVRDevice::InitializeRenderModels() {
	RESULT r = R_PASS;
	vr::EVRInitError ivrResult = vr::VRInitError_None;

	CN(m_pHALImp);

	m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &ivrResult);
	CIVRM(ivrResult, "Unable to get render model interface");

	for (uint32_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++) {
		vr::ETrackedDeviceClass trackedDeviceClass = m_pIVRHMD->GetTrackedDeviceClass(unTrackedDevice);

		// Set up controller
		if (trackedDeviceClass == vr::TrackedDeviceClass_Controller) {
			CR(InitializeRenderModel(unTrackedDevice));
		}
	}

Error:
	return r;
}

composite *OpenVRDevice::GetSenseControllerObject(ControllerType controllerType) {
	/*
	switch (controllerType) {
	case CONTROLLER_LEFT: {
#ifdef _USE_TEST_APP
		return m_pLeftControllerModel->GetFirstChild<sphere>().get();
#else
		return m_pLeftControllerModel;
#endif
	} break;

	case CONTROLLER_RIGHT: {
#ifdef _USE_TEST_APP
		return m_pRightControllerModel->GetFirstChild<sphere>().get();
#else
		return m_pRightControllerModel;
#endif
	} break;
	}
	*/

	// TODO:! 

	return nullptr;
}

RESULT OpenVRDevice::HandleVREvent(vr::VREvent_t event) {
	RESULT r = R_PASS;

	switch (event.eventType) {
		case vr::VREvent_TrackedDeviceActivated: {
			//SetupRenderModelForTrackedDevice(event.trackedDeviceIndex);
			//dprintf("Device %u attached. Setting up render model.\n", event.trackedDeviceIndex);
			DEBUG_LINEOUT("Device %u attached", event.trackedDeviceIndex);
		} break;

		case vr::VREvent_TrackedDeviceDeactivated: {
			DEBUG_LINEOUT("Device %u detached.\n", event.trackedDeviceIndex);
		} break;

		case vr::VREvent_TrackedDeviceUpdated: {
			DEBUG_LINEOUT("Device %u updated.\n", event.trackedDeviceIndex);
		} break;

		// TODO: Lots more events to ultimately map...
	}

//Error:
	return r;
}

Matrix4 OpenVRDevice::ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose) {

	Matrix4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
		);

	return matrixObj;
}

ViewMatrix OpenVRDevice::ConvertSteamVRMatrixToViewMatrix(const vr::HmdMatrix34_t &matPose) {
	Matrix4 mat4 = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
	mat4.invert();

	ViewMatrix viewMat;
	viewMat.identity();

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			viewMat(j, i) = mat4[i * 4 + j];

	return viewMat;
}

RESULT OpenVRDevice::UpdateSenseController(vr::ETrackedControllerRole controllerRole, vr::VRControllerState_t state) {

	ControllerState cState;

	cState.triggerRange = state.rAxis[1].x;
	cState.ptTouchpad = point(state.rAxis[0].x, state.rAxis[0].y, 0.0f);

	cState.fMenu = (state.ulButtonPressed & (1<<1)) != 0;
	cState.fGrip = (state.ulButtonPressed & (1<<2)) != 0;

	if (controllerRole == vr::TrackedControllerRole_LeftHand) {
		cState.type = CONTROLLER_LEFT;
	} 
	else if (controllerRole == vr::TrackedControllerRole_RightHand) {
		cState.type = CONTROLLER_RIGHT;
	}
	m_pSenseController->SetControllerState(cState); 

	return R_PASS;
}

RESULT OpenVRDevice::UpdateHMD() {
	RESULT r = R_PASS;

	// Process SteamVR events
	vr::VREvent_t vrEvent;
	CN(m_pIVRHMD);


	if(m_pIVRHMD->PollNextEvent(&vrEvent, sizeof(vr::VREvent_t))) {
		HandleVREvent(vrEvent);
	}

	// Process SteamVR controller state
	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++) {
		vr::VRControllerState_t state;

		// state.ulButtonPressed/Touched
		// menu button  - 2
		// grip buttons - 4

		// state.rAxis
		// touch pad    - 0 (x,y from [-1,1])
		// trigger      - 1 (x from [0,1])

		// TODO: currently not getting click events from touch pad or trigger
		// more info: https://github.com/ValveSoftware/openvr/wiki/IVRSystem::GetControllerState
		if (m_pIVRHMD->GetControllerState(unDevice, &state, sizeof(vr::VRControllerState_t))) {
			if (m_pIVRHMD->GetTrackedDeviceClass(unDevice) == vr::TrackedDeviceClass_Controller) {
				uint32_t currentFrame = state.unPacketNum;

				if (currentFrame != m_vrFrameCount) {
					m_vrFrameCount = currentFrame;
					vr::ETrackedControllerRole controllerRole = m_pIVRHMD->GetControllerRoleForTrackedDeviceIndex(unDevice);
					UpdateSenseController(controllerRole, state);
				}
			}

			//m_rbShowTrackedDevice[unDevice] = state.ulButtonPressed == 0;
			// TODO: do stuff
		}
	}


	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	m_validPoseCount = 0;
	m_strPoseClasses = "";

	bool fLeftHandTracked = false;
	bool fRightHandTracked = false;

	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice) {
		if (m_rTrackedDevicePose[nDevice].bPoseIsValid) {
			
			m_validPoseCount++;

			Matrix4 poseMat4 = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			Vector4 centerVec4 = poseMat4 * Vector4(0, 0, 0, 1);

			switch (m_pIVRHMD->GetTrackedDeviceClass(nDevice)) {
				case vr::TrackedDeviceClass_Controller: {
					ViewMatrix viewMat;
					viewMat.identity();

					for (int i = 0; i < 4; i++)
						for (int j = 0; j < 4; j++)
							viewMat(j, i) = poseMat4[i * 4 + j];

					vr::ETrackedControllerRole controllerRole = m_pIVRHMD->GetControllerRoleForTrackedDeviceIndex(nDevice);
						
					point ptControllerPosition = point(centerVec4.x, centerVec4.y, centerVec4.z);

					// stereocamera::GetPosition() incorporates the hmd position
					// vive controllers are separate from the hmd position, so camera::GetPosition() is used
					point offset = m_pParentSandbox->GetCamera()->camera::GetPosition();

					RotationMatrix qOffset = RotationMatrix();
					quaternion qRotation = m_pParentSandbox->GetCamera()->GetOffsetOrientation();
					qRotation.Reverse();
					qOffset.SetQuaternionRotationMatrix(qRotation);

					ptControllerPosition = qOffset * ptControllerPosition;

					ptControllerPosition += offset;
					ptControllerPosition.w() = 1.0f;


					quaternion qOrientation = viewMat.GetOrientation();
					qRotation.Reverse();
					qOrientation *= qRotation;
					qOrientation.Reverse();

					if (controllerRole == vr::TrackedControllerRole_LeftHand && m_pControllerMeshLeft != nullptr) {
						m_pControllerMeshLeft->SetPosition(ptControllerPosition);
						m_pControllerMeshLeft->SetOrientation(qOrientation);

						m_pLeftHand->SetPosition(ptControllerPosition);
						m_pLeftHand->SetOrientation(qOrientation);
						m_pLeftHand->SetLocalOrientation(qOrientation);

						fLeftHandTracked = true;
						m_pLeftHand->SetTracked(true);
					}
					else if (controllerRole == vr::TrackedControllerRole_RightHand && m_pControllerMeshRight != nullptr) {
						m_pControllerMeshRight->SetPosition(ptControllerPosition);
						m_pControllerMeshRight->SetOrientation(qOrientation);

						m_pRightHand->SetPosition(ptControllerPosition);
						m_pRightHand->SetOrientation(qOrientation);
						m_pRightHand->SetLocalOrientation(qOrientation);

						fRightHandTracked = true;
						m_pRightHand->SetTracked(true);
					}

				} break;

				case vr::TrackedDeviceClass_HMD: {
					ViewMatrix viewMat;
					viewMat.identity();

					RotationMatrix qOffset = RotationMatrix();
					quaternion qRotation = m_pParentSandbox->GetCamera()->GetOffsetOrientation();
					qRotation.Reverse();
					qOffset.SetQuaternionRotationMatrix(qRotation);

					for (int i = 0; i < 4; i++)
						for (int j = 0; j < 4; j++)
							viewMat(j, i) = poseMat4[i * 4 + j];

					//m_ptOrigin = viewMat.GetPosition();

					m_ptOrigin = point(centerVec4.x, centerVec4.y, centerVec4.z);
					m_ptOrigin = qOffset * m_ptOrigin;
					//m_ptOrigin = -1.0f * point(viewMat(12), viewMat(13), viewMat(14));

					//m_ptOrigin = viewMat.GetPosition();

					m_qOrientation = viewMat.GetOrientation();
					//m_qOrientation.Reverse();
				} break;

				case vr::TrackedDeviceClass_Invalid:
				case vr::TrackedDeviceClass_TrackingReference: 
				default:{
					// TODO: Default handling
				} break;
			}
			
		}

		if (!fLeftHandTracked && m_pLeftHand != nullptr) {
			m_pLeftHand->SetTracked(fLeftHandTracked);
		}
		if (!fRightHandTracked && m_pRightHand != nullptr) {
			m_pRightHand->SetTracked(fRightHandTracked);
		}
	}

Error:
	return r;
}

RESULT OpenVRDevice::ReleaseHMD() {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::SetUpFrame() {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::BindFramebuffer(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::CommitSwapChain(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::SubmitFrame() {
	return m_pOpenVRHMDSinkNode->SubmitFrame();
}

RESULT OpenVRDevice::SetAndClearRenderSurface(EYE_TYPE eye) {
	return R_NOT_IMPLEMENTED;
}

RESULT OpenVRDevice::UnsetRenderSurface(EYE_TYPE eye) {
	return m_pOpenVRHMDSinkNode->UnsetRenderSurface(eye);
}

RESULT OpenVRDevice::RenderHMDMirror() {
	return R_NOT_IMPLEMENTED;
}

ProjectionMatrix OpenVRDevice::GetPerspectiveFOVMatrix(EYE_TYPE eye, float znear, float zfar) {
	vr::EVREye eyeType = (eye == EYE_LEFT) ? vr::Eye_Left : vr::Eye_Right;

	if (m_pIVRHMD == nullptr) {
		return ProjectionMatrix();
	}

	vr::HmdMatrix44_t mat = m_pIVRHMD->GetProjectionMatrix(eyeType, znear, zfar);

	ProjectionMatrix projMat;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			projMat(i, j) = mat.m[i][j];

	return projMat;
}

ViewMatrix OpenVRDevice::GetViewMatrix(EYE_TYPE eye) {
	vr::EVREye eyeType = (eye == EYE_LEFT) ? vr::Eye_Left : vr::Eye_Right;

	if (m_pIVRHMD == nullptr)
		return ViewMatrix();

	vr::HmdMatrix34_t matEye = m_pIVRHMD->GetEyeToHeadTransform(eyeType);
	Matrix4 mat4Eye = ConvertSteamVRMatrixToMatrix4(matEye);
	mat4Eye.invert();

	ViewMatrix eyeMat;
	eyeMat.identity();

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			eyeMat(j, i) = mat4Eye[i * 4 + j];

	Matrix4 mat4View = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
	mat4View.invert();

	ViewMatrix viewMat;
	viewMat.identity();

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			viewMat(j, i) = mat4View[i * 4 + j];
			//viewMat(i, j) = mat4View[i * 4 + j];
		}
	}

	return viewMat;
}