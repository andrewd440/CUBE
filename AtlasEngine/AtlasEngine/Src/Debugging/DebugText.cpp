#include "Debugging\DebugText.h"
#include "Math\OrthoMatrix.h"
#include "freetype-gl.h"
#include "text-buffer.h"
#include "Rendering\Screen.h"
#include "ResourceHolder.h"
#include "SystemResources\SystemFile.h"
#include "markup.h"

namespace FDebug
{
	Text::Text()
		: TSingleton()
		, mTextProjection(FOrthoMatrix{ 0, (float)SScreen::GetResolution().x, (float)SScreen::GetResolution().y, 0, -1, 0 })
		, mTextBuffer(nullptr)
		, mProjectionUniform()
		, mViewUniform()
		, mModelUniform()
	{
		mTextBuffer = text_buffer_new(LCD_FILTERING_ON);

		glUseProgram(mTextBuffer->shader);
		mProjectionUniform.Bind(mTextBuffer->shader, "projection");
		mViewUniform.Bind(mTextBuffer->shader, "view");
		mModelUniform.Bind(mTextBuffer->shader, "model");
	}

	Text::~Text()
	{
		text_buffer_delete(mTextBuffer);
	}

	void Text::AddText(std::wstring& String, Vector2i Position, markup_t& Markup)
	{
		IFileSystem::GetInstance().SetToProgramDirectory();
		vec2 P{ {Position.x, Position.y } };
		text_buffer_add_text(mTextBuffer, &P, &Markup, &String[0], String.length());
	}

	void Text::Render()
	{

		glUseProgram(mTextBuffer->shader);
		static const FMatrix4 Identity{};
		mProjectionUniform.SetMatrix(1, GL_FALSE, &mTextProjection);
		mViewUniform.SetMatrix(1, GL_FALSE, &Identity);
		mModelUniform.SetMatrix(1, GL_FALSE, &Identity);

		text_buffer_render(mTextBuffer);
		text_buffer_clear(mTextBuffer);
	}
}
