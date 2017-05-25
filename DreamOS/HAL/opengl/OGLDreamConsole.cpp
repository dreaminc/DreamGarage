#include "OGLDreamConsole.h"

#include "RESULT/EHM.h"

#include "OGLTexture.h"
#include "OGLTriangle.h"
#include "OGLText.h"

#include "DreamConsole/Console.h"

#include <Windows.h>
#include <string>
#include <algorithm>

#include "HAL/opengl/OGLFramebuffer.h"

// OGLRenderContext
void OGLRenderContext::Initialize() 
{
	m_pOGLFont = std::make_shared<Font>(L"Arial.fnt");
	
	// Default font size
	m_fontSize = 4.0f;

	m_Background = std::make_unique<OGLTriangle>(m_OGLImp);
	m_Background->SetColor(color(0.0f, 0.0f, 0.0f, 0.2f));
}

void OGLRenderContext::Render(point& topLeft, point& bottomRight) {
	point bl = point(topLeft.x() - m_BackgroundMargin, bottomRight.y() - m_BackgroundMargin, 0.0f);
	point br = point(bottomRight.x() + m_BackgroundMargin, bottomRight.y() - m_BackgroundMargin, 0.0f);
	point tl = point(topLeft.x() - m_BackgroundMargin, topLeft.y() + m_BackgroundMargin, 0.0f);
	point tr = point(bottomRight.x() + m_BackgroundMargin, topLeft.y() + m_BackgroundMargin, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_pParentProgram->RenderObject(m_Background->Set(bl, br, tl));
	m_pParentProgram->RenderObject(m_Background->Set(tl, br, tr));
}

// OGLDreamConsole

OGLDreamConsole::OGLDreamConsole(OpenGLImp* pOGL) :
	//OGLProgramTextureBitBlit(pOGL, "ogldreamconsole"),
	OGLProgramTextureBitBlit(pOGL),
	m_OGLGraph(pOGL, this),
	m_OGLConsole(pOGL, this)
{
	Initialize();
}

OGLDreamConsole::~OGLDreamConsole()
{
	Destroy();
}

void OGLDreamConsole::Initialize() {
	// Font
	m_pOGLFont = std::make_shared<Font>(L"Arial.fnt");
	m_fontSize = 4.0f;

	//m_OGLTitleText = std::make_unique<OGLText>(m_OGLImp, m_OGLFont, "Dream Garage v0.01");
	//m_OGLTitleText->MoveTo(-0.7f, -0.7f, 0);

	m_OGLConsoleText = std::make_unique<OGLText>(m_pParentImp, m_pOGLFont);
	m_OGLConsoleText->MoveTo(-0.8f, 0.8f, 0);
}

RESULT OGLDreamConsole::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebuffer));

	// Outputs
	//CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));
	CR(MakeOutputPassthru<OGLFramebuffer>("output_framebuffer", &m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLDreamConsole::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	if (m_pParentImp->IsRenderProfiler() == false) {
		return R_SKIPPED;
	}

	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		//BindToFramebuffer(m_pOGLFramebuffer);
		m_pOGLFramebuffer->Bind();
	}

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	Render(m_pCamera->GetCameraEye() == EYE_MONO);

	UnbindFramebuffer();

//Error:
	return r;
}

void OGLDreamConsole::Destroy() {
	// empty
}

void OGLDreamConsole::Render(bool fMono) {
	const float viewTop = (fMono) ? 0.8 : 0.4f;
	const float viewBottom = -viewTop;
	const float viewRight = (fMono) ? 0.8 : 0.5f;
	const float viewLeft = -viewRight;

	const int maxRows = (fMono) ? 36 : 19;

	float fontSize = (fMono) ? 3.0f : 4.0f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);
	//	glDisable(GL_BLEND);

		// Render FPS graph
	switch (DreamConsole::GetConsole()->GetConfiguration().graph) {
		case DreamConsole::GraphConfiguration::FPS: {
			m_OGLGraph.Render(point(viewLeft, viewBottom + 0.2f, 0), point(viewLeft + 0.4f, viewBottom, 0), DreamConsole::GetConsole()->GetFPSGraph(), static_cast<uint16_t>(0), static_cast<uint16_t>(200));
		} break;
		case DreamConsole::GraphConfiguration::FPSMinimal: {
			m_OGLGraph.RenderMinimal(point(viewLeft, viewBottom + 0.2f, 0), point(viewLeft + 0.4f, viewBottom, 0), DreamConsole::GetConsole()->GetFPSGraph(), static_cast<uint16_t>(0), static_cast<uint16_t>(200));
		} break;
	}

	// Revert to 'default' render state. TODO: refactor rendering states
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//m_OGLProgram->RenderObject(m_OGLTitleText.get());

	// Render HUD text
	//return;
	float posY = viewBottom;

	DreamConsole::GetConsole()->ForEach([&](const std::string& consoleText) {
		RenderObject((DimObj*)m_OGLConsoleText->SetText(consoleText, fontSize)->SetPosition(point(0.1f, posY, 0.0f), text::TOP_RIGHT));
		posY += m_OGLConsoleText->m_height;

		return !(posY > viewTop);
	});


	float xOff, yOff = 0; // cursor offset
	
	auto currentCmdTxt = DreamConsole::GetConsole()->GetCmdText();
	std::string cmdText = ">" + currentCmdTxt;

	RenderObject((DimObj*)m_OGLConsoleText->SetText(cmdText, fontSize + 0.02f)->SetPosition(point(0.1f, viewBottom, 0.0f), text::BOTTOM_RIGHT));

	yOff = m_OGLConsoleText->m_height / 2;
		
	auto currentCmdTxtUntilCursor = DreamConsole::GetConsole()->GetCmdText().substr(0, DreamConsole::GetConsole()->GetCmtTextCursorPos());

	cmdText = ">" + currentCmdTxtUntilCursor;

	xOff = m_OGLConsoleText->SetText(cmdText, fontSize + 0.02f)->m_width;
		
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	if ((time / 100) % 10 > 5) {
		cmdText = "|";

		RenderObject((DimObj*)m_OGLConsoleText->SetText(cmdText, fontSize + 0.02f)->SetPosition(point(0.1f + xOff, viewBottom - yOff, 0.0f), text::RIGHT));
	}
	
	// Render debug console text
	m_OGLConsole.Render(point(viewLeft, viewTop, 0.0f), point(0.0f, 0.0f, 0.0f), fontSize);
}

// OGLProfilerGraph

OGLProfilerGraph::OGLProfilerGraph(OpenGLImp* pOGL, OGLProgram *pParentProgram) :
	OGLRenderContext(pOGL, pParentProgram)
{
	Initialize();
}

OGLProfilerGraph::~OGLProfilerGraph()
{
	Destroy();
}

void OGLProfilerGraph::Initialize()
{
	OGLRenderContext::Initialize();

	m_OGLTriangle = std::make_unique<OGLTriangle>(m_OGLImp);
	m_OGLTriangle->SetColor(color(0.8f, 0.0f, 0.0f, 1));

	m_pOGLFont = std::make_shared<Font>(L"Arial.fnt");
	m_OGLFPSText = std::make_unique<OGLText>(m_OGLImp, m_pOGLFont);

	m_BackgroundMargin = 0;
}

void OGLProfilerGraph::Destroy()
{
	// empty
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

	for (size_t count = 0; count < records.size(); ++count)
	{
		prevPoint = currentPoint;
		auto deltaTime = std::chrono::duration<double>(currentTime - records[index].second).count();

		currentPoint.x() = right - (float)deltaTime * width / (float)time_scale;
		currentPoint.y() = YSCALE(static_cast<int>(records[index].first));

		minFPS = std::min(minFPS, records[index].first);
		maxFPS = std::max(maxFPS, records[index].first);

		if (count == 0) {
			// draw current FPS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_pParentProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(records[index].first), m_fontSize)->MoveTo(right, currentPoint.y() - YSCALE(minFPS) + bottom, 0));
		}

		if (currentPoint.x() < left) {
			currentPoint.y() = (prevPoint.x() - left) / (prevPoint.x() - currentPoint.x()) * (currentPoint.y() - prevPoint.y()) + prevPoint.y();
			currentPoint.x() = left;
		}
		else  {
			index = (index == 0) ? records.size() - 1 : index - 1;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		point cr = point(currentPoint.x(), currentPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		point pr = point(prevPoint.x(), prevPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		m_pParentProgram->RenderObject(m_OGLTriangle->Set(cr, pr, pr));

	}

	// Draw local min/max bars
	int fpsDiff = static_cast<int>(maxFPS) - static_cast<int>(minFPS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_pParentProgram->RenderObject(m_OGLTriangle->Set(point(left, YSCALE(0), 0), point(right, YSCALE(0), 0), point(right, YSCALE(0), 0)));
	m_pParentProgram->RenderObject(m_OGLTriangle->Set(point(left, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_pParentProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(minFPS), m_fontSize)->SetPosition(point(left, YSCALE(0), 0), text::TOP_RIGHT));
	m_pParentProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(maxFPS), m_fontSize)->SetPosition(point(left, YSCALE(fpsDiff), 0), text::BOTTOM_RIGHT));

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

	for (size_t count = 0; count < records.size(); ++count) {
		prevPoint = currentPoint;
		auto deltaTime = std::chrono::duration<double>(currentTime - records[index].second).count();

		currentPoint.x() = right - (float)deltaTime * width / (float)time_scale;
		currentPoint.y() = YSCALE_CAP(static_cast<int>(records[index].first));

		if (count == 0) {
			// draw current FPS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			//m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(records[index].first), m_fontSize)->MoveTo(right, currentPoint.y() - YSCALE(minFPS) + bottom, 0));
			m_pParentProgram->RenderObject((DimObj*)m_OGLFPSText->SetText(std::to_string(records[index].first), m_fontSize)->SetPosition(point(right, currentPoint.y() - YSCALE(minFPS) + bottom, 0), text::RIGHT));
		}

		if (currentPoint.x() < left) {
			currentPoint.y() = (prevPoint.x() - left) / (prevPoint.x() - currentPoint.x()) * (currentPoint.y() - prevPoint.y()) + prevPoint.y();
			currentPoint.x() = left;
		}
		else {
			index = (index == 0) ? records.size() - 1 : index - 1;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		point cr = point(currentPoint.x(), currentPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		point pr = point(prevPoint.x(), prevPoint.y() - YSCALE(minFPS) + bottom, 0.0f);
		m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(cr, pr, pr));

	}

	// Draw local min/max bars
	int fpsDiff = static_cast<int>(maxFPS) - static_cast<int>(minFPS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(point(left, YSCALE(0), 0), point(right, YSCALE(0), 0), point(right, YSCALE(0), 0)));
	m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(point(left, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_pParentProgram->RenderObject((DimObj*)m_OGLFPSText->SetText(std::to_string(minFPS), m_fontSize)->SetPosition(point(left, YSCALE(0), 0), text::TOP_RIGHT));
	m_pParentProgram->RenderObject((DimObj*)m_OGLFPSText->SetText(std::to_string(maxFPS), m_fontSize)->SetPosition(point(left, YSCALE(fpsDiff), 0), text::BOTTOM_RIGHT));
}

template<typename T>
void OGLProfilerGraph::RenderMinimal(point& topLeft, point& bottomRight, ProfilerGraph<T>& graph, T minValue, T maxValue) {
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

	size_t count = 0;

	{
		prevPoint = currentPoint;
		auto deltaTime = std::chrono::duration<double>(currentTime - records[index].second).count();

		currentPoint.x() = right - (float)deltaTime * width / (float)time_scale;
		currentPoint.y() = YSCALE_CAP(static_cast<int>(records[index].first));

		if (count == 0) {
			// draw current FPS
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			//m_OGLProgram->RenderObject(m_OGLFPSText->SetText(std::to_string(records[index].first), m_fontSize)->MoveTo(right, currentPoint.y() - YSCALE(minFPS) + bottom, 0));
			m_pParentProgram->RenderObject((DimObj*)m_OGLFPSText->SetText(std::to_string(records[index].first), 3.0f)->SetPosition(point((left+right)/2, currentPoint.y() - YSCALE(minFPS) + bottom, 0), text::CENTER));
		}
	}

	// Draw local min/max bars
	int fpsDiff = static_cast<int>(maxFPS) - static_cast<int>(minFPS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(point(left, YSCALE(0), 0), point(right, YSCALE(0), 0), point(right, YSCALE(0), 0)));
	m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(point(left, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0), point(right, YSCALE(fpsDiff), 0)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_pParentProgram->RenderObject((DimObj*)m_OGLFPSText->SetText(std::to_string(minFPS), m_fontSize)->SetPosition(point(left, YSCALE(0), 0), text::TOP_RIGHT));
	m_pParentProgram->RenderObject((DimObj*)m_OGLFPSText->SetText(std::to_string(maxFPS), m_fontSize)->SetPosition(point(left, YSCALE(fpsDiff), 0), text::BOTTOM_RIGHT));
}

// OGLDebugConsole
OGLDebugConsole::OGLDebugConsole(OpenGLImp* pOGL, OGLProgram *pParentProgram) :
	OGLRenderContext(pOGL, pParentProgram)
{
	Initialize();
}

OGLDebugConsole::~OGLDebugConsole()
{
	Destroy();
}

void OGLDebugConsole::Initialize()
{
	OGLRenderContext::Initialize();
	m_OGLConsoleText = std::make_unique<OGLText>(m_OGLImp, m_pOGLFont);	

	m_OGLTextBackground = std::make_unique<OGLQuad>(m_OGLImp, 1, 1);

	m_OGLTriangle = std::make_unique<OGLTriangle>(m_OGLImp);
	m_OGLTriangle->SetColor(color(0.0f, 0.0f, 0.0f, 0.3f));

}

void OGLDebugConsole::Render(point& topLeft, point& bottomRight, float fontSize)
{
	float consoleHeight = 0;

	m_OGLTriangle->SetColor(color(0.0f, 0.0f, 0.0f, 0.3f));

	for (const auto& it : DebugConsole::GetDebugConsole()->GetConsoleData())
	{
		point rowTL = point(topLeft.x(), topLeft.y() - consoleHeight, 0.0f);

		m_OGLConsoleText->SetText(it->GetValue(), fontSize);

		m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(rowTL, rowTL + point(0, -m_OGLConsoleText->m_height, 0), rowTL + point(m_OGLConsoleText->m_width, -m_OGLConsoleText->m_height, 0)));
		m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(rowTL, rowTL + point(m_OGLConsoleText->m_width, -m_OGLConsoleText->m_height, 0), rowTL + point(m_OGLConsoleText->m_width, 0, 0)));

		consoleHeight += m_OGLConsoleText->m_height;

		m_pParentProgram->RenderObject((DimObj*)m_OGLConsoleText->SetPosition(rowTL, text::BOTTOM_RIGHT));
	}

	auto currentCmdText = DreamConsole::GetConsole()->GetCmdText();
	
	if (currentCmdText.length() > 0)
	{
		m_OGLTriangle->SetColor(color(0.0f, 0.0f, 0.0f, 0.8f));

		point rowTL = point(topLeft.x(), topLeft.y() - consoleHeight, 0.0f);

		m_OGLConsoleText->SetText(currentCmdText, 1.0);

		m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(point(-m_OGLConsoleText->m_width / 2, -m_OGLConsoleText->m_height / 2, 0), point(+m_OGLConsoleText->m_width / 2, +m_OGLConsoleText->m_height / 2, 0), point(-m_OGLConsoleText->m_width / 2, +m_OGLConsoleText->m_height / 2, 0)));
		m_pParentProgram->RenderObject((DimObj*)m_OGLTriangle->Set(point(-m_OGLConsoleText->m_width / 2, -m_OGLConsoleText->m_height / 2, 0), point(+m_OGLConsoleText->m_width / 2, -m_OGLConsoleText->m_height / 2, 0), point(+m_OGLConsoleText->m_width / 2, +m_OGLConsoleText->m_height / 2, 0)));
		//m_OGLProgram->RenderObject(m_OGLTriangle->Set(rowTL, rowTL + point(m_OGLConsoleText->m_width, -m_OGLConsoleText->m_height, 0), rowTL + point(m_OGLConsoleText->m_width, 0, 0)));

		m_pParentProgram->RenderObject((DimObj*)m_OGLConsoleText->SetPosition(point(0, 0, 0), text::CENTER));

		auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		if ((time / 100) % 10 > 5) {
			auto xOff = -m_OGLConsoleText->m_width / 2;

			auto currentCmdTxtUntilCursor = DreamConsole::GetConsole()->GetCmdText().substr(0, DreamConsole::GetConsole()->GetCmtTextCursorPos());
			xOff += m_OGLConsoleText->SetText(currentCmdTxtUntilCursor, 1.0)->m_width;

			point pt = point(xOff, 0, 0);

			m_pParentProgram->RenderObject((DimObj*)m_OGLConsoleText->SetText("|", 1.0)->SetPosition(pt, text::RIGHT));
		}
	}
}

void OGLDebugConsole::Destroy() {
	// empty
}