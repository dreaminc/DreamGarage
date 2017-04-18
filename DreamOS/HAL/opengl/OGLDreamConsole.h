#ifndef OGLDREAMCONSOLE_H_
#define OGLDREAMCONSOLE_H_

#include "HAL/opengl/OGLProgram.h"

#include "OGLTriangle.h"
#include "OGLQuad.h"
#include "OGLText.h"

#include <memory>

#include "DreamConsole/ProfilerGraph.h"
#include "DreamConsole/DebugConsole.h"

// Dream OS
// DreamOS/HAL/opengl/OGLDreamConsole.h
// OGLDreamConsole renders the profiler to an OGL program

// TODO: Switch to the DreamApp arch 

class OGLRenderContext {
public:
	OGLRenderContext(OpenGLImp* pOGL, std::shared_ptr<OGLProgram> pOGLProgram) :
		m_OGLImp(pOGL),
		m_pOGLProgram(pOGLProgram)
	{}

	virtual ~OGLRenderContext() {}

	void Initialize();
	void Render(point& topLeft, point& rightBottom);

	std::shared_ptr<OGLProgram>	m_pOGLProgram;

protected:
	OpenGLImp*	m_OGLImp;

	std::unique_ptr<OGLTriangle> m_Background;
	float m_BackgroundMargin = 0.0f;// 0.025f;

	// A font for the text. TBD: remove and use a font factory
	std::shared_ptr<Font>	m_OGLFont;
	float m_fontSize;
};

template<typename T>
void Render(const point& topLeft, const point& rightBottom, ProfilerGraph<T>& data)
{}

class OGLProfilerGraph : public OGLRenderContext {
public:
	OGLProfilerGraph(OpenGLImp* pOGL, std::shared_ptr<OGLProgram> pOGLProgram);
	~OGLProfilerGraph();

	void Initialize();

	template<typename T>
	void Render(point& topLeft, point& bottomRight, ProfilerGraph<T>& graph, double vScale = 1.0);

	template<typename T>
	void Render(point& topLeft, point& bottomRight, ProfilerGraph<T>& graph, T minValue, T maxValue);

	template<typename T>
	void RenderMinimal(point& topLeft, point& bottomRight, ProfilerGraph<T>& graph, T minValue, T maxValue);

	void Destroy();

private:
	// Use triangle primitive to draw a line for the graphs
	std::unique_ptr<OGLTriangle> m_OGLTriangle;

	// Use text for FPS
	std::unique_ptr<OGLText>	 m_OGLFPSText;
};

class OGLDebugConsole : public OGLRenderContext {
public:
	OGLDebugConsole(OpenGLImp* pOGL, std::shared_ptr<OGLProgram> pOGLProgram);
	~OGLDebugConsole();

	void Initialize();
	void Render(point& topLeft, point& bottomRight, float fontSize = 4.0f);
	void Destroy();

private:
	std::unique_ptr<OGLText>	m_OGLConsoleText;
	std::unique_ptr<OGLQuad>	m_OGLTextBackground;
	std::unique_ptr<OGLTriangle>	m_OGLTriangle;
};

class OGLDreamConsole : public OGLRenderContext {
public:
	OGLDreamConsole(OpenGLImp* pOGL, std::shared_ptr<OGLProgram> pOGLProgram);
	~OGLDreamConsole();

	void Initialize();
	
	// A mono view is an extended view in case the scene is not stereoscopically rendered for an HMD.
	void Render(bool isMonoView = false);
	void Destroy();

private:
	// Title for the profiler
	std::unique_ptr<OGLText>	 m_OGLTitleText;
	std::unique_ptr<OGLText>	 m_OGLConsoleText;

	OGLProfilerGraph	m_OGLGraph;
	OGLDebugConsole		m_OGLConsole;

};

#endif // ! OGLDREAMCONSOLE_H_