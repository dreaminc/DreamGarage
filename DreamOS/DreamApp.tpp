//DreamApp template function implementation source

template<class derivedAppType>
RESULT DreamApp<derivedAppType>::Initialize() {
	RESULT r = R_PASS;

	// Grab the context composite from DreamOS
	CN(m_pDreamOS);
	m_pCompositeContext = m_pDreamOS->AddComposite();
	CN(m_pCompositeContext);

	// Initialize the OBB (collisions)
	CR(m_pCompositeContext->InitializeOBB());
	CR(m_pDreamOS->AddObjectToInteractionGraph(m_pCompositeContext));

	// Initialize the App
	CR(InitializeApp(m_pContext));

Error:
	return r;
}

template<class derivedAppType>
vector DreamApp<derivedAppType>::GetCameraLookXZ() {
	vector vLook = GetComposite()->GetCamera()->GetLookVector();
	return vector(vLook.x(), 0.0f, vLook.z()).Normal();
}

template<class derivedAppType>
RESULT DreamApp<derivedAppType>::UpdateCompositeWithCameraLook(float depth, float yPos) {

	composite *pComposite = GetComposite();
	auto pCamera = pComposite->GetCamera();
	vector vLookXZ = GetCameraLookXZ();
	point lookOffset = depth * vLookXZ + point(0.0f, yPos, 0.0f);

	pComposite->SetPosition(pCamera->GetPosition() + lookOffset);
	pComposite->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vLookXZ));

	return R_PASS;
}

template<class derivedAppType>
RESULT DreamApp<derivedAppType>::UpdateCompositeWithHands(float yPos, Axes handAxes) {
	RESULT r = R_PASS;

	composite *pComposite = GetComposite();
	auto pCamera = pComposite->GetCamera();
	vector vLookXZ = GetCameraLookXZ();
	vector vUp = vector(0.0f, 1.0f, 0.0f);

	hand *pLeftHand = GetDOS()->GetHand(HAND_TYPE::HAND_LEFT);
	hand *pRightHand = GetDOS()->GetHand(HAND_TYPE::HAND_RIGHT);

	//TODO: use axes enum to define plane, cylinder, or sphere surface
	//uint16_t axes = static_cast<uint16_t>(handAxes);

	CN(pCamera);
	CN(pLeftHand);
	CN(pRightHand);
	{
		float dist = 0.0f;

		point ptCamera = pCamera->GetPosition();
		vector vPos;
		for (auto& hand : { pLeftHand, pRightHand }) {
			float handDist = 0.0f;
			point ptHand = hand->GetPosition(true);
			vector vHand = ptHand - pCamera->GetOrigin(true);
			vector vTempPos = vLookXZ * (vHand.dot(vLookXZ));
			if (vTempPos.magnitudeSquared() > vPos.magnitudeSquared())
				vPos = vTempPos;
		}

		point lookOffset = vPos + point(0.0f, yPos, 0.0f);

		pComposite->SetPosition(pCamera->GetPosition() + lookOffset);
		pComposite->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vLookXZ));
	}

Error:
	return r;

}
