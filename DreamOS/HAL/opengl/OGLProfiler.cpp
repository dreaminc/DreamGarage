#include "OGLProfiler.h"

#include "RESULT/EHM.h"

#include "OGLTexture.h"
#include "OGLTriangle.h"
#include "OGLText.h"

#include "Profiler/Profiler.h"

void OGLProfiler::Render(OpenGLImp* pOGL, OGLProgram* pOGLProgram)
{
	RESULT r = R_PASS;

	// Enable color blending for text
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);

	pOGLProgram->UseProgram();
//	CR(pOGLProgram->SetCamera(m_pCamera));

	static OGLTriangle* pOGLTrianlge = nullptr;
	static OGLText* pFPS = nullptr;

	if (pOGLTrianlge == nullptr)
	{
		pOGLTrianlge = new OGLTriangle(pOGL, triangle(point(-0.5, -0.5, 0), point(0.5, -0.5, 0), point(0.5, 0.5, 0)));

		texture *pColorTexture = new OGLTexture(pOGL, L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
		pOGLTrianlge->SetColorTexture(pColorTexture);
	}


	if (pFPS == nullptr)
	{
		static std::shared_ptr<Font> pFont = std::make_shared<Font>(L"Arial.fnt");

		pFPS = new OGLText(pOGL, pFont, "000");
	}

	{
		triangle* ptri = dynamic_cast<triangle*>(pOGLTrianlge);

		auto& records = Profiler::GetProfiler()->GetFPSGraph().GetData();

		double left = -0.8;
		double bottom = -0.8;
		double right = left + 0.5;
		double top = bottom + 0.4;

		double width = right - left;
		double height = top - bottom;

		double time_scale = 5.0;
		uint16_t	fps_scale = 200;

		size_t index = Profiler::GetProfiler()->GetFPSGraph().GetNewestIndex();

#define YSCALE(y) (y * height / fps_scale + bottom)

		point p1(right, YSCALE(records[index].first), 0);
		point p2 = p1;

		size_t cnt = 0;

		auto last_time = std::chrono::high_resolution_clock::now();
		double xxx = 0;
		bool done = false;

		uint16_t minFPS = 500;
		uint16_t maxFPS = 0;

		for (;;)
		{
			p1 = p2;
			xxx += 0.1;
			auto deltaTime = std::chrono::duration<double>(last_time - records[index].second).count();

			p2.x() = right - deltaTime * width / time_scale;
			p2.y() = YSCALE(records[index].first);

			minFPS = min(minFPS, records[index].first);
			maxFPS = max(maxFPS, records[index].first);

			if (cnt == 0)
			{
				// draw current FPS

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				pFPS->SetText(std::to_string(records[index].first), 2.0);
				pFPS->MoveTo(right, p2.y(), 0);
				pOGLProgram->RenderObject(pFPS);
			}

			if (p2.x() < left)
			{
				p2.y() = (p1.x() - left) / (p1.x() - p2.x()) * (p2.y() - p1.y()) + p1.y();
				p2.x() = left;
				done = true;
			}

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			ptri->set(p2, p1, p1);
			pOGLTrianlge->UpdateOGLBuffers();
			pOGLProgram->RenderObject(pOGLTrianlge);

			index = (index == 0) ? records.size() - 1 : index - 1;

			cnt++;
			if (cnt >= records.size() || done)
			{
				break;
			}
		}

		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			ptri->set(point(left - 0.05, YSCALE(minFPS), 0), point(right + 0.1, YSCALE(minFPS), 0), point(right + 0.1, YSCALE(minFPS), 0));
			pOGLTrianlge->UpdateOGLBuffers();
			pOGLProgram->RenderObject(pOGLTrianlge);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			pFPS->SetText(std::to_string(minFPS), 2.0);
			pFPS->MoveTo(left - 0.05, YSCALE(minFPS) - 0.05, 0);
			pOGLProgram->RenderObject(pFPS);
		}

		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			ptri->set(point(left - 0.05, YSCALE(maxFPS), 0), point(right + 0.1, YSCALE(maxFPS), 0), point(right + 0.1, YSCALE(maxFPS), 0));
			pOGLTrianlge->UpdateOGLBuffers();
			pOGLProgram->RenderObject(pOGLTrianlge);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			pFPS->SetText(std::to_string(maxFPS), 2.0);
			pFPS->MoveTo(left - 0.05, YSCALE(maxFPS), 0);
			pOGLProgram->RenderObject(pFPS);
		}
	}

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	static OGLText*	pOGLText = nullptr;

	if (pOGLText == nullptr)
	{
		static std::shared_ptr<Font> pFont = std::make_shared<Font>(L"Arial.fnt");

		pOGLText = new OGLText(pOGL, pFont, "Dream Garage");

		pOGLText->MoveTo(-1.0, -1.0, 0);
	}

	pOGLProgram->RenderObject(pOGLText);

//Error:

}

