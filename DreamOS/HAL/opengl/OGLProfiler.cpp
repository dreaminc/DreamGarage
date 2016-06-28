#include "OGLProfiler.h"

#include "RESULT/EHM.h"

#include "OGLTexture.h"
#include "OGLTriangle.h"
#include "OGLText.h"

#include "Profiler/Profiler.h"

// OGLProfiler

OGLProfiler::OGLProfiler(OpenGLImp* pOGL, OGLProgram* pOGLProgram) :
	OGLRenderContext(pOGL, pOGLProgram),
	m_OGLGraph(pOGL, pOGLProgram)
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
	m_OGLTitleText->MoveTo(-1.0, -1.0, 0);
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
	m_OGLGraph.Render(point(-0.8, -0.8 + 0.4, 0), point(-0.8 + 0.5, -0.8, 0), Profiler::GetProfiler()->GetFPSGraph(), 0.005);

	// Revert to 'default' render state. TODO: refactor rendering states
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_OGLProgram->RenderObject(m_OGLTitleText.get());
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
	m_OGLTriangle = std::make_unique<OGLTriangle>(m_OGLImp, triangle(point(-0.5, -0.5, 0), point(0.5, -0.5, 0), point(0.5, 0.5, 0)));
	m_OGLTriangle->SetColorTexture(new OGLTexture(m_OGLImp, L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR));

	m_OGLFont = std::make_shared<Font>(L"Arial.fnt");
	m_OGLFPSText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont, "000");
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

	const double left = topLeft.x();
	const double bottom = bottomRight.y();
	const double right = bottomRight.x();
	const double top = topLeft.y();

	double width = right - left;
	double height = top - bottom;

	size_t index = graph.GetNewestIndex();

	#define YSCALE(y) (y * vScale + bottom)

	point prevPoint(right, YSCALE(records[index].first), 0);
	point currentPoint = prevPoint;

	uint16_t minFPS = records[index].first;
	uint16_t maxFPS = minFPS;

	for (size_t cnt = 0; cnt < records.size(); ++cnt)
	{
		prevPoint = currentPoint;
		auto deltaTime = std::chrono::duration<double>(currentTime - records[index].second).count();

		currentPoint.x() = right - deltaTime * width / time_scale;
		currentPoint.y() = YSCALE(records[index].first);

		minFPS = min(minFPS, records[index].first);
		maxFPS = max(maxFPS, records[index].first);

		if (cnt == 0)
		{
			// draw current FPS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(records[index].first), 2.0)->MoveTo(right, currentPoint.y(), 0));
		}

		if (currentPoint.x() < left)
		{
			currentPoint.y() = (prevPoint.x() - left) / (prevPoint.x() - currentPoint.x()) * (currentPoint.y() - prevPoint.y()) + prevPoint.y();
			currentPoint.x() = left;

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			m_OGLProgram->RenderObject(m_OGLTriangle->Set(currentPoint, prevPoint, prevPoint));

			break;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_OGLProgram->RenderObject(m_OGLTriangle->Set(currentPoint, prevPoint, prevPoint));

		index = (index == 0) ? records.size() - 1 : index - 1;
	}

	// Draw local min/max bars
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_OGLProgram->RenderObject(m_OGLTriangle->Set(point(left - 0.05, YSCALE(minFPS), 0), point(right + 0.1, YSCALE(minFPS), 0), point(right + 0.1, YSCALE(minFPS), 0)));
	m_OGLProgram->RenderObject(m_OGLTriangle->Set(point(left - 0.05, YSCALE(maxFPS), 0), point(right + 0.1, YSCALE(maxFPS), 0), point(right + 0.1, YSCALE(maxFPS), 0)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(minFPS), 2.0)->MoveTo(left - 0.05, YSCALE(minFPS) - 0.05, 0));
	m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(maxFPS), 2.0)->MoveTo(left - 0.05, YSCALE(maxFPS), 0));
}
