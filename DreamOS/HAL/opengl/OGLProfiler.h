#ifndef OGLPROFILER_H_
#define OGLPROFILER_H_

#include "HAL/opengl/OGLProgram.h"

#include "OGLTriangle.h"
#include "OGLQuad.h"
#include "OGLText.h"

#include <memory>

#include "Profiler/ProfilerGraph.h"
#include "Profiler/DebugConsole.h"

// Dream OS
// DreamOS/HAL/opengl/OGLProfiler.h
// OGLProfiler renders the profiler to an OGL program

class OGLRenderContext {
public:
	OGLRenderContext(OpenGLImp* pOGL, OGLProgram* pOGLProgram) :
		m_OGLImp(pOGL),
		m_OGLProgram(pOGLProgram)
	{}

	virtual ~OGLRenderContext() {}

	void Init();
	void Render(point& topLeft, point& rightBottom);

	OGLProgram*	m_OGLProgram;

protected:
	OpenGLImp*	m_OGLImp;

	std::unique_ptr<OGLTriangle> m_Background;
	float m_BackgroundMargin = 0.025f;

	// A font for the text. TBD: remove and use a font factory
	std::shared_ptr<Font>	m_OGLFont;
	float m_fontSize;
};

template<typename T>
void Render(const point& topLeft, const point& rightBottom, ProfilerGraph<T>& data)
{}

class OGLProfilerGraph : public OGLRenderContext {
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

	// Use text for FPS
	std::unique_ptr<OGLText>	 m_OGLFPSText;
};

class OGLDebugConsole : public OGLRenderContext {
public:
	OGLDebugConsole(OpenGLImp* pOGL, OGLProgram* pOGLProgram);
	~OGLDebugConsole();

	void Init();
	void Render(point& topLeft, point& bottomRight);
	void Destroy();

private:
	std::unique_ptr<OGLText>	m_OGLConsoleText;
};

class OGLProfiler : public OGLRenderContext {
public:
	OGLProfiler(OpenGLImp* pOGL, OGLProgram* pOGLProgram);
	~OGLProfiler();

	void Init();
	void Render();
	void Destroy();

private:
	// Title for the profiler
	std::unique_ptr<OGLText>	 m_OGLTitleText;

	std::unique_ptr<OGLText>	 m_OGLConsoleText;

	OGLProfilerGraph	m_OGLGraph;
	OGLDebugConsole		m_OGLConsole;

};

#endif // ! OGLPROFILER_H