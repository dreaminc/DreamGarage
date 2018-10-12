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