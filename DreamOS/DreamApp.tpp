// DreamApp template function implementation source

//template<class derivedAppType>
//RESULT DreamApp<derivedAppType>::Initialize() {
//	RESULT r = R_PASS;
//
//	// Grab the context composite from DreamOS
//	CN(m_pDreamOS);
//	m_pComposite = m_pDreamOS->AddComposite();
//	CN(m_pComposite);
//
//	// Initialize the OBB (collisions)
//	CR(m_pComposite->InitializeOBB());
//	CR(m_pDreamOS->AddObjectToInteractionGraph(m_pComposite));
//
//	// Initialize the App
//	CR(InitializeApp(m_pContext));
//
//Error:
//	return r;
//}