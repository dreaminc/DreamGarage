#include "TimeManagerModule.h"

TimeManagerModule::TimeManagerModule(DreamOS *pDreamOS, void *pContext, double processingTimeQuantum) :
	DreamModule<TimeManagerModule>(pDreamOS, pContext),
	m_processingTimeQuantum(processingTimeQuantum),
	m_totalElapsedTime(0.0f),
	m_totalTimeToProcess(0.0f),
	m_startTime(std::chrono::high_resolution_clock::now()),
	m_currentTime(std::chrono::high_resolution_clock::now())
{
	RESULT r = R_PASS;

	CR(RegisterEvent(TIME_ELAPSED));
	CR(Reset());

	Validate();
	return;

Error:
	Invalidate();
	return;
}

TimeManagerModule::~TimeManagerModule() {
	Shutdown();
}

// The Self Construct
TimeManagerModule* TimeManagerModule::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	TimeManagerModule *pDreamModule = new TimeManagerModule(pDreamOS, pContext);
	return pDreamModule;
}

RESULT TimeManagerModule::InitializeModule(void *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT TimeManagerModule::OnDidFinishInitializing(void *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT TimeManagerModule::Shutdown(void *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT TimeManagerModule::Reset() {
	
	m_startTime = std::chrono::high_resolution_clock::now();

	m_currentTime = m_startTime;
	m_totalElapsedTime = 0;
	m_totalTimeToProcess = 0;

	return R_PASS;
}

RESULT TimeManagerModule::Update(void *pContext) {
	RESULT r = R_PASS;

	auto now = std::chrono::high_resolution_clock::now();
	auto deltaTime = std::chrono::duration<double>(now - m_currentTime).count();
	auto msDeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - m_currentTime).count();
	m_currentTime = now;

	m_totalTimeToProcess += deltaTime;

	while (m_totalTimeToProcess >= m_processingTimeQuantum) {
		TimeEvent event(TIME_ELAPSED, m_totalElapsedTime, m_processingTimeQuantum);
		CR(NotifySubscribers(TIME_ELAPSED, &event));

		m_totalTimeToProcess -= m_processingTimeQuantum;
		m_totalElapsedTime += m_processingTimeQuantum;
	}

	// FPS
	// TODO: This can be simplified
	m_runTimeFPS = FPS_RRF_YVAL * m_runTimeFPS  + (FPS_RRF_XVAL * (NUM_MICROSECONDS_IN_SECOND / msDeltaTime));
	
	if(m_runTimeFPS > m_maxFPS)
		m_maxFPS = m_runTimeFPS;
	else if (m_runTimeFPS < m_minFPS)
		m_minFPS = m_runTimeFPS;

	m_numFrames++;
	
	CR(PrintFPS());

Error:
	return R_PASS;
}

RESULT TimeManagerModule::PrintFPS() {
	//DEBUG_LINEOUT_RETURN("Runtime FPS: %f numframes:%zd", m_runTimeFPS, m_numFrames);
	
	DEBUG_LINEOUT_RETURN("Runtime FPS: %04f min:%04f max:%04f", m_runTimeFPS, m_minFPS, m_maxFPS);
	
	return R_PASS;
}

double TimeManagerModule::GetRunTimeFrameRate() {
	return m_runTimeFPS;
}

long long TimeManagerModule::GetTotalNumberOfFrames() {
	return m_numFrames;
}

RESULT TimeManagerModule::ResetMinMaxFPS() {
	m_maxFPS = 0.0f;
	m_minFPS = 0.0f;
	return R_PASS;
}