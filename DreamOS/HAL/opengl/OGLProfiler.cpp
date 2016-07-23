#include "OGLProfiler.h"

#include "RESULT/EHM.h"

#include "OGLTexture.h"
#include "OGLTriangle.h"
#include "OGLText.h"

#include "Profiler/Profiler.h"
#include "Profiler/DebugConsole.h"

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
	m_OGLFont = std::make_shared<Font>(L"Arial.fnt");

	m_OGLTitleText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont, "Dream Garage v0.01");
	m_OGLTitleText->MoveTo(-0.7f, -0.7f, 0);

	m_OGLConsoleText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont, std::string(100, '0'));
	m_OGLConsoleText->MoveTo(-0.8f, 0.8f, 0);
}

void OGLProfiler::Destroy()
{

}

void OGLProfiler::Render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);

	m_OGLProgram->UseProgram();
	//pOGLProgram->SetCamera();

	// Render FPS graph
	//m_OGLGraph.Render(point(-0.5, -0.5 + 0.4, 0), point(-0.5 + 0.5, -0.5, 0), Profiler::GetProfiler()->GetFPSGraph(), 0.005);
	m_OGLGraph.Render(point(0.15f, -0.5f + 0.4f, 0), point(0.15f + 0.5f, -0.5f, 0), Profiler::GetProfiler()->GetFPSGraph(), 0.005);

	// Revert to 'default' render state. TODO: refactor rendering states
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_OGLProgram->RenderObject(m_OGLTitleText.get());

	// Render hud text
	float posY = 0;
	const int maxRows = 28;

	for (auto it = (Profiler::GetProfiler()->GetConsoleText().size() > maxRows) ?
			Profiler::GetProfiler()->GetConsoleText().end() - maxRows : Profiler::GetProfiler()->GetConsoleText().begin();
		 it < Profiler::GetProfiler()->GetConsoleText().end();
		 it++)
	{
		m_OGLProgram->RenderObject(m_OGLConsoleText->SetText(*it, 3.1f)->SetPosition(point(-0.4f, 0.8f - posY, 0.0f)));
		posY += 0.05f;
	}

	// Render debug console text
	m_OGLConsole.Render();
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
	m_OGLTriangle = std::make_unique<OGLTriangle>(m_OGLImp);
	m_OGLTriangle->SetColor(color(0.8f, 0.0f, 0.0f, 1));

	m_OGLFont = std::make_shared<Font>(L"Arial.fnt");
	m_OGLFPSText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont, "000");
	//m_OGLFPSText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont, "000");

	m_Background = std::make_unique<OGLTriangle>(m_OGLImp);
	m_Background->SetColor(color(0.1f, 0.1f, 0.1f, 0.5f));
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

		minFPS = min(minFPS, records[index].first);
		maxFPS = max(maxFPS, records[index].first);

		if (cnt == 0)
		{
			// draw current FPS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(records[index].first), 3.0)->MoveTo(right, currentPoint.y() - YSCALE(minFPS) + bottom, 0));
		}

		if (currentPoint.x() < left)
		{
			currentPoint.y() = (prevPoint.x() - left) / (prevPoint.x() - currentPoint.x()) * (currentPoint.y() - prevPoint.y()) + prevPoint.y();
			currentPoint.x() = left;

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			point cr = point(currentPoint.x(), currentPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
			point pr = point(prevPoint.x(), prevPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
			m_OGLProgram->RenderObject(m_OGLTriangle->Set(cr, pr, pr));
			break;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		point cr = point(currentPoint.x(), currentPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		point pr = point(prevPoint.x(), prevPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		m_OGLProgram->RenderObject(m_OGLTriangle->Set(cr, pr, pr));

		index = (index == 0) ? records.size() - 1 : index - 1;
	}

	// Draw local min/max bars
	int fpsDiff = static_cast<int>(maxFPS) - static_cast<int>(minFPS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_OGLProgram->RenderObject(m_OGLTriangle->Set(point(left, YSCALE(0), 0), point(right, YSCALE(0), 0), point(right, YSCALE(0), 0)));
	m_OGLProgram->RenderObject(m_OGLTriangle->Set(point(left, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(minFPS), 3.0f)->SetPosition(point(left, YSCALE(0), 0), text::TOP_RIGHT));
	m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(maxFPS), 3.0f)->SetPosition(point(left, YSCALE(fpsDiff), 0), text::BOTTOM_RIGHT));

//	m_OGLProgram->RenderObject(m_Background->Set(point(0.0f, 0.0f, 0.0f), point(0.0f, 1.0f, 0.0f), point(1.0f, 0.0f, 0.0f)));
	float marginX = 0.05f;
	float marginY = 0.05f;
	float scaledTop = YSCALE(fpsDiff);

	point bl = point(left - marginX, bottom - marginY, 0.0f);
	point br = point(right + marginX, bottom - marginY, 0.0f);
	point tl = point(left - marginX, scaledTop + marginY, 0.0f);
	point tr = point(right + marginX, scaledTop + marginY, 0.0f);

	m_OGLProgram->RenderObject(m_Background->Set(bl, br, tl));
	m_OGLProgram->RenderObject(m_Background->Set(tl, br, tr));

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
	m_OGLFont = std::make_shared<Font>(L"Arial.fnt");

	m_OGLConsoleText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont, std::string(100, '0'));
	m_OGLConsoleText->MoveTo(-0.8f, 0.8f, 0);

	m_Background = std::make_unique<OGLTriangle>(m_OGLImp);
	m_Background->SetColor(color(0.5f, 0.5f, 0.5f, 1.0f));
}

void OGLDebugConsole::Render()
{
	float posY = 0;
	for (const auto& it : DebugConsole::GetDebugConsole()->GetConsoleData())
	{
		m_OGLProgram->RenderObject(m_OGLConsoleText->SetText(it->GetValue(), 3.1f)->MoveTo(0.0f, 0.8f - posY, 0));
		posY += 0.05f;
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_OGLProgram->RenderObject(m_Background->Set(point(0.0f, 0.0f, 0.0f), point(0.0f, 1.0f, 0.0f), point(1.0f, 0.0f, 0.0f)));
}

void OGLDebugConsole::Destroy()
{

}