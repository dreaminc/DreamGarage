#ifndef OGLPROFILER_H_
#define OGLPROFILER_H_

#include "HAL/opengl/OGLProgram.h"

#include "OGLTriangle.h"
#include "OGLText.h"

#include <memory>

#include "Profiler/ProfilerGraph.h"


// Dream OS
// DreamOS/HAL/opengl/OGLProfiler.h
// OGLProfiler renders the profiler to an OGL program

class OGLContext {
public:
	OGLContext(OpenGLImp* pOGL, OGLProgram* pOGLProgram) :
		m_OGLImp(pOGL),
		m_OGLProgram(pOGLProgram)
	{}

	virtual ~OGLContext() {}

protected:
	OpenGLImp*	m_OGLImp;
	OGLProgram*	m_OGLProgram;
};

class OGLProfilerGraph : public OGLContext {
public:
	OGLProfilerGraph(OpenGLImp* pOGL, OGLProgram* pOGLProgram);
	~OGLProfilerGraph();

	void Init();

	template<typename T>
	void Render(point& topLeft, point& bottomRight, ProfilerGraph<T>& graph, double vScale = 1.0);

	void Destroy();

private:
	// Use triangle primitive to draw a line for the graphs
	std::unique_ptr<OGLTriangle> m_OGLTriangle;

	// A font for the text. TBD: remove and use a font factory
	std::shared_ptr<Font>	m_OGLFont;

	// Use text for FPS
	std::unique_ptr<OGLText>	 m_OGLFPSText;
};

class OGLProfiler : public OGLContext {
public:
	OGLProfiler(OpenGLImp* pOGL, OGLProgram* pOGLProgram);
	~OGLProfiler();

	void Init();
	void Render();
	void Destroy();

private:
	// A font for the text. TBD: remove and use a font factory
	std::shared_ptr<Font>	m_OGLFont;

	// Title for the profiler
	std::unique_ptr<OGLText>	 m_OGLTitleText;

	OGLProfilerGraph	m_OGLGraph;
};

#endif // ! OGLPROFILER_H