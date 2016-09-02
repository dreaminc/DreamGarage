#include "OGLProfiler.h"

#include "RESULT/EHM.h"

#include "OGLTexture.h"
#include "OGLTriangle.h"
#include "OGLText.h"

#include "Profiler/Profiler.h"

#include <Windows.h>
#include <string>
#include <algorithm>

// OGLRenderContext
void OGLRenderContext::Init()
{
	m_OGLFont = std::make_shared<Font>(L"Arial.fnt");
	m_fontSize = 4.0f;

	m_Background = std::make_unique<OGLTriangle>(m_OGLImp);
	m_Background->SetColor(color(0.0f, 0.0f, 0.0f, 0.2f));
}

void OGLRenderContext::Render(point& topLeft, point& bottomRight)
{
	point bl = point(topLeft.x() - m_BackgroundMargin, bottomRight.y() - m_BackgroundMargin, 0.0f);
	point br = point(bottomRight.x() + m_BackgroundMargin, bottomRight.y() - m_BackgroundMargin, 0.0f);
	point tl = point(topLeft.x() - m_BackgroundMargin, topLeft.y() + m_BackgroundMargin, 0.0f);
	point tr = point(bottomRight.x() + m_BackgroundMargin, topLeft.y() + m_BackgroundMargin, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_OGLProgram->RenderObject(m_Background->Set(bl, br, tl));
	m_OGLProgram->RenderObject(m_Background->Set(tl, br, tr));
}

// OGLProfiler

OGLProfiler::OGLProfiler(OpenGLImp* pOGL, OGLProgram* pOGLProgram) :
	OGLRenderContext(pOGL, pOGLProgram),
	m_OGLGraph(pOGL, pOGLProgram),
	m_OGLConsole(pOGL, pOGLProgram)
{
	Init();
}

OGLProfiler::~OGLProfiler()
{
	Destroy();
}

void OGLProfiler::Init()
{
	OGLRenderContext::Init();

	//m_OGLTitleText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont, "Dream Garage v0.01");
	//m_OGLTitleText->MoveTo(-0.7f, -0.7f, 0);

	m_OGLConsoleText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont);
	m_OGLConsoleText->MoveTo(-0.8f, 0.8f, 0);
}

void OGLProfiler::Destroy() {

}

void OGLProfiler::Render() {
	float posY = 0.0f;
	const int maxRows = 28;
	float top = 0.4f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);
//	glDisable(GL_BLEND);

	// Render FPS graph
	m_OGLGraph.Render(point(-0.5f, -0.4f + 0.2f, 0), point(-0.5f + 0.4f, -0.4f, 0), Profiler::GetProfiler()->GetFPSGraph(), static_cast<uint16_t>(0), static_cast<uint16_t>(200));

	// Revert to 'default' render state. TODO: refactor rendering states
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//m_OGLProgram->RenderObject(m_OGLTitleText.get());

	// Render hud text

	for (auto it = (Profiler::GetProfiler()->GetConsoleText().size() > maxRows) ?
			Profiler::GetProfiler()->GetConsoleText().end() - maxRows : Profiler::GetProfiler()->GetConsoleText().begin();
		 it < Profiler::GetProfiler()->GetConsoleText().end();
		 it++)
	{
		m_OGLProgram->RenderObject(m_OGLConsoleText->SetText(*it, m_fontSize)->SetPosition(point(0.1f, top - posY, 0.0f),text::BOTTOM_RIGHT));
		posY += m_OGLConsoleText->m_height;
	}

	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	if ((time / 100) % 10 > 5)
		m_OGLProgram->RenderObject(m_OGLConsoleText->SetText("_", m_fontSize + 0.02f)->SetPosition(point(0.1f, top - posY, 0.0f), text::BOTTOM_RIGHT));
	
	// Render debug console text
	m_OGLConsole.Render(point(-0.5f, top, 0.0f), point(0.0f, 0.0f, 0.0f));
}

// OGLProfilerGraph

OGLProfilerGraph::OGLProfilerGraph(OpenGLImp* pOGL, OGLProgram* pOGLProgram) :
	OGLRenderContext(pOGL, pOGLProgram)
{
	Init();
}

OGLProfilerGraph::~OGLProfilerGraph()
{
	Destroy();
}

void OGLProfilerGraph::Init()
{
	OGLRenderContext::Init();

	m_OGLTriangle = std::make_unique<OGLTriangle>(m_OGLImp);
	m_OGLTriangle->SetColor(color(0.8f, 0.0f, 0.0f, 1));

	m_OGLFont = std::make_shared<Font>(L"Arial.fnt");
	m_OGLFPSText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont);

	m_BackgroundMargin = 0;
}

void OGLProfilerGraph::Destroy()
{

}

template<typename T>
void OGLProfilerGraph::Render(point& topLeft, point& bottomRight, ProfilerGraph<T>& graph, double vScale)
{
	auto& records = graph.GetData();
	double time_scale = graph.GetRecordTime();

	auto currentTime = std::chrono::high_resolution_clock::now();

	const float left = topLeft.x();
	const float bottom = bottomRight.y();
	const float right = bottomRight.x();
	const float top = topLeft.y();

	float width = right - left;
	float height = top - bottom;

	size_t index = graph.GetNewestIndex();

	#define YSCALE(y) (y * (float)vScale + bottom)

	point prevPoint(right, YSCALE(records[index].first), 0);
	point currentPoint = prevPoint;

	uint16_t minFPS = records[index].first;
	uint16_t maxFPS = minFPS;

	for (size_t cnt = 0; cnt < records.size(); ++cnt)
	{
		prevPoint = currentPoint;
		auto deltaTime = std::chrono::duration<double>(currentTime - records[index].second).count();

		currentPoint.x() = right - (float)deltaTime * width / (float)time_scale;
		currentPoint.y() = YSCALE(static_cast<int>(records[index].first));

		minFPS = std::min(minFPS, records[index].first);
		maxFPS = std::max(maxFPS, records[index].first);

		if (cnt == 0)
		{
			// draw current FPS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(records[index].first), m_fontSize)->MoveTo(right, currentPoint.y() - YSCALE(minFPS) + bottom, 0));
		}

		if (currentPoint.x() < left)
		{
			currentPoint.y() = (prevPoint.x() - left) / (prevPoint.x() - currentPoint.x()) * (currentPoint.y() - prevPoint.y()) + prevPoint.y();
			currentPoint.x() = left;
		}
		else 
		{
			index = (index == 0) ? records.size() - 1 : index - 1;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		point cr = point(currentPoint.x(), currentPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		point pr = point(prevPoint.x(), prevPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		m_OGLProgram->RenderObject(m_OGLTriangle->Set(cr, pr, pr));

	}

	// Draw local min/max bars
	int fpsDiff = static_cast<int>(maxFPS) - static_cast<int>(minFPS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_OGLProgram->RenderObject(m_OGLTriangle->Set(point(left, YSCALE(0), 0), point(right, YSCALE(0), 0), point(right, YSCALE(0), 0)));
	m_OGLProgram->RenderObject(m_OGLTriangle->Set(point(left, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(minFPS), m_fontSize)->SetPosition(point(left, YSCALE(0), 0), text::TOP_RIGHT));
	m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(maxFPS), m_fontSize)->SetPosition(point(left, YSCALE(fpsDiff), 0), text::BOTTOM_RIGHT));

	OGLRenderContext::Render(point(left, YSCALE(fpsDiff), 0.0f), point(right, bottom, 0.0f));

}

template<typename T>
void OGLProfilerGraph::Render(point& topLeft, point& bottomRight, ProfilerGraph<T>& graph, T minValue, T maxValue)
{
	auto& records = graph.GetData();
	double time_scale = graph.GetRecordTime();

	auto currentTime = std::chrono::high_resolution_clock::now();

	const float left = topLeft.x();
	const float bottom = bottomRight.y();
	const float right = bottomRight.x();
	const float top = topLeft.y();

	float width = right - left;
	float height = top - bottom;

	size_t index = graph.GetNewestIndex();

	float vScale = static_cast<float>((top - bottom)) / (maxValue - minValue);

	uint16_t minFPS = static_cast<uint16_t>(minValue);
	uint16_t maxFPS = static_cast<uint16_t>(maxValue);

	#define YSCALE_CAP(y) (y > maxFPS) ? YSCALE(maxFPS) : YSCALE(y)

	point prevPoint(right, YSCALE_CAP(records[index].first), 0);
	point currentPoint = prevPoint;

	OGLRenderContext::Render(point(left, top, 0.0f), point(right, bottom, 0.0f));

	for (size_t cnt = 0; cnt < records.size(); ++cnt)
	{
		prevPoint = currentPoint;
		auto deltaTime = std::chrono::duration<double>(currentTime - records[index].second).count();

		currentPoint.x() = right - (float)deltaTime * width / (float)time_scale;
		currentPoint.y() = YSCALE_CAP(static_cast<int>(records[index].first));

		if (cnt == 0)
		{
			// draw current FPS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			//m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(records[index].first), m_fontSize)->MoveTo(right, currentPoint.y() - YSCALE(minFPS) + bottom, 0));
			m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(records[index].first), m_fontSize)->SetPosition(point(right, currentPoint.y() - YSCALE(minFPS) + bottom, 0), text::RIGHT));
		}

		if (currentPoint.x() < left)
		{
			currentPoint.y() = (prevPoint.x() - left) / (prevPoint.x() - currentPoint.x()) * (currentPoint.y() - prevPoint.y()) + prevPoint.y();
			currentPoint.x() = left;
		}
		else
		{
			index = (index == 0) ? records.size() - 1 : index - 1;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		point cr = point(currentPoint.x(), currentPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		point pr = point(prevPoint.x(), prevPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		m_OGLProgram->RenderObject(m_OGLTriangle->Set(cr, pr, pr));

	}

	// Draw local min/max bars
	int fpsDiff = static_cast<int>(maxFPS) - static_cast<int>(minFPS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_OGLProgram->RenderObject(m_OGLTriangle->Set(point(left, YSCALE(0), 0), point(right, YSCALE(0), 0), point(right, YSCALE(0), 0)));
	m_OGLProgram->RenderObject(m_OGLTriangle->Set(point(left, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(minFPS), m_fontSize)->SetPosition(point(left, YSCALE(0), 0), text::TOP_RIGHT));
	m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(maxFPS), m_fontSize)->SetPosition(point(left, YSCALE(fpsDiff), 0), text::BOTTOM_RIGHT));
}

// OGLDebugConsole
OGLDebugConsole::OGLDebugConsole(OpenGLImp* pOGL, OGLProgram* pOGLProgram) :
	OGLRenderContext(pOGL, pOGLProgram)
{
	Init();
}

OGLDebugConsole::~OGLDebugConsole()
{
	Destroy();
}

void OGLDebugConsole::Init()
{
	OGLRenderContext::Init();
	m_OGLConsoleText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont);	

	m_OGLTextBackground = std::make_unique<OGLQuad>(m_OGLImp, 1, 1);

	m_OGLTriangle = std::make_unique<OGLTriangle>(m_OGLImp);
	m_OGLTriangle->SetColor(color(0.0f, 0.0f, 0.0f, 0.3f));

}

void OGLDebugConsole::Render(point& topLeft, point& bottomRight)
{
	float consoleHeight = 0;

	for (const auto& it : DebugConsole::GetDebugConsole()->GetConsoleData())
	{
		point rowTL = point(topLeft.x(), topLeft.y() - consoleHeight, 0.0f);

		m_OGLConsoleText->SetText(it->GetValue(), m_fontSize);

		m_OGLProgram->RenderObject(m_OGLTriangle->Set(rowTL, rowTL + point(0, -m_OGLConsoleText->m_height, 0), rowTL + point(m_OGLConsoleText->m_width, -m_OGLConsoleText->m_height, 0)));
		m_OGLProgram->RenderObject(m_OGLTriangle->Set(rowTL, rowTL + point(m_OGLConsoleText->m_width, -m_OGLConsoleText->m_height, 0), rowTL + point(m_OGLConsoleText->m_width, 0, 0)));

		consoleHeight += m_OGLConsoleText->m_height;

		m_OGLProgram->RenderObject(m_OGLConsoleText->SetPosition(rowTL, text::BOTTOM_RIGHT));
	}

//	m_OGLTextBackground->SetColor(color(1.0f, 1.0f, 1.0f, 1.0f));
//	m_OGLProgram->RenderObject(m_OGLTextBackground->Set(->SetPosition(point(0, 0, 0)));

	
//	OGLRenderContext::Render(topLeft, point(bottomRight.x(), topLeft.y() - consoleHeight, 0.0f));

}

void OGLDebugConsole::Destroy()
{

}