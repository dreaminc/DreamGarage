#ifndef OGL_TEXT_H_
#define OGL_TEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLText.h

#include <memory>

#include "OGLObj.h"
#include "Primitives/font.h"
#include "Primitives/text.h"
#include "OGLTexture.h"

class OGLText : public text, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:

	OGLText(OpenGLImp *pParentImp, std::shared_ptr<Font> pFont, const std::string& text = "", double size = 1.0f, bool isBillboard = false) :
		text(pFont, text, size, isBillboard),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();

		std::wstring font(L"Fonts/" + pFont->GetGlyphImageFile());
		// Load appropriate glyph texture

		#pragma message ("bad code - use const std::string")

		texture *pColorTexture = new OGLTexture(pParentImp, (wchar_t*)font.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);

		SetColorTexture(pColorTexture);

		if (isBillboard) {

			// switch z and y coordinates to match quad
			vertex* textVertices = GetVertices();
			for (unsigned int i = 0; i < NumberVertices(); i++) {
				point current = textVertices[i].GetPoint();
				//centers billboard
				textVertices[i].SetPoint(point(current.x() - (m_width*0.5f), current.z(), -current.y() + (m_height*0.5f)));
			}

			//SetPosition(point(0.0f, 0.0f, 0.0f));
			UpdateOGLBuffers();
		
		}
	}

	OGLText* SetText(const std::string& text, double size = 1.0)
	{
		text::SetText(text, size);

		// TODO: need to be able to deal with changing vertex amounts automatically
		if (CheckAndCleanDirty())
		{
			ReleaseOGLBuffers();
			OGLInitialize();
		}

		SetDirty();

		return this;
	}

	RESULT Render() {
		RESULT r = R_PASS;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// TODO: Rethink this since it's in the critical path
		DimObj *pDimObj = GetDimObj();

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);
		//glDrawElements(GL_LINES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);
		//glDrawElements(GL_POINT, pDimObj->NumberVertices(), GL_UNSIGNED_INT, NULL);

		glDisable(GL_BLEND);

	Error:
		return r;
	}
};

#endif // ! OGL_TEXT_H_
