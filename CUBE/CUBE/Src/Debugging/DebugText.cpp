#include "Debugging\DebugText.h"
#include "Math\OrthoMatrix.h"
#include "freetype-gl\freetype-gl.h"
#include "freetype-gl\text-buffer.h"
#include "Rendering\Screen.h"
#include "ResourceHolder.h"
#include "SystemResources\SystemFile.h"
#include "freetype-gl\markup.h"
#include "Rendering\RenderSystem.h"

namespace FDebug
{
	Text::Text()
		: TSingleton()
		, mTextProjection(FOrthoMatrix{ 0, 1, 0, 1, -1, 0 })
		, mTextBuffer(nullptr)
		, mProjectionUniform()
		, mViewUniform()
		, mModelUniform()
	{
		FRenderSystem::OnResolutionChange.AddListener<Text, &Text::OnResolutionChange>(this);
		mTextBuffer = text_buffer_new(LCD_FILTERING_ON);

		glUseProgram(mTextBuffer->shader);
		mProjectionUniform.Bind(mTextBuffer->shader, "projection");
		mViewUniform.Bind(mTextBuffer->shader, "view");
		mModelUniform.Bind(mTextBuffer->shader, "model");
	}

	Text::~Text()
	{
		FRenderSystem::OnResolutionChange.RemoveListener(this);
		text_buffer_delete(mTextBuffer);
	}

	void Text::AddText(std::wstring& String, Vector2i Position, markup_t& Markup)
	{
		vec2 P{ {(float)Position.x, (float)Position.y } };
		text_buffer_add_text(mTextBuffer, &P, &Markup, &String[0], String.length());
	}

	void Text::Render()
	{

		glUseProgram(mTextBuffer->shader);
		static const FMatrix4 Identity{};
		mProjectionUniform.SetMatrix(1, GL_FALSE, &mTextProjection);
		mViewUniform.SetMatrix(1, GL_FALSE, &Identity);
		mModelUniform.SetMatrix(1, GL_FALSE, &Identity);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		text_buffer_render(mTextBuffer);
		text_buffer_clear(mTextBuffer);
	}

	void Text::OnResolutionChange(Vector2ui NewResolution)
	{
		mTextProjection = FOrthoMatrix{ 0, (float)NewResolution.x, (float)NewResolution.y, 0, -1, 0 };
	}
}
