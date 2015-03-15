#include "Debugging\DebugText.h"
#include "Math\OrthoMatrix.h"
#include "freetype-gl.h"
#include "Rendering\Screen.h"
#include "ResourceHolder.h"
#include "SystemResources\SystemFile.h"

static const uint32_t DEFAULT_BUFFER_SIZE = 200;

namespace FDebug
{
	Text::Text()
		: TSingleton()
		, mTextProjection(FOrthoMatrix{ 0, (float)SScreen::GetResolution().x, (float)SScreen::GetResolution().y, 0, 0, 1 })
		, mTextShader()
		, mTextureFont(nullptr)
		, mMesh(GL_DYNAMIC_DRAW, DEFAULT_BUFFER_SIZE)
		, mProjectionUniform()
	{
		FShader VertexShader{ L"Shaders/DebugText.vert", GL_VERTEX_SHADER };
		FShader FragShader{ L"Shaders/DebugText.frag", GL_FRAGMENT_SHADER };
		mTextShader.AttachShader(VertexShader);
		mTextShader.AttachShader(FragShader);
		mTextShader.LinkProgram();

		mProjectionUniform.Bind(mTextShader.GetID(), "Projection");
	}

	Text::~Text()
	{
		texture_atlas_delete(mTextureFont->atlas);
		//texture_font_delete(mTextureFont);
	}

	void Text::AddText(const wchar_t* String, const FColor Color, Vector2i Position)
	{
		// From https://code.google.com/p/freetype-gl/source/browse/trunk/demo-font.c
		size_t i;
		IFileSystem::GetInstance().SetToProgramDirectory();
		texture_font_load_glyphs(mTextureFont, String);
		for (i = 0; i < wcslen(String); ++i)
		{
			texture_glyph_t *glyph = texture_font_get_glyph(mTextureFont, String[i]);
			if (glyph != NULL)
			{
				int32_t kerning = 0;
				if (i > 0)
				{
					kerning = (int32_t)texture_glyph_get_kerning(glyph, String[i - 1]);
				}
				Position.x += kerning;
				int x0 = (int)(Position.x + glyph->offset_x);
				int y0 = (int)(Position.y + glyph->offset_y);
				int x1 = (int)(x0 + glyph->width);
				int y1 = (int)(y0 - glyph->height);
				float s0 = glyph->s0;
				float t0 = glyph->t0;
				float s1 = glyph->s1;
				float t1 = glyph->t1;
				FTextVertex vertices[4] =
				{
					{ Vector3f((float)x0, (float)y0, 0.0f), Vector2f((float)s0, (float)t0), Color },
					{ Vector3f((float)x0, (float)y1, 0.0f), Vector2f((float)s0, (float)t1), Color },
					{ Vector3f((float)x1, (float)y1, 0.0f), Vector2f((float)s1, (float)t1), Color },
					{ Vector3f((float)x1, (float)y0, 0.0f), Vector2f((float)s1, (float)t0), Color }
				};

				const uint32_t BaseIndex = mMesh.AddVertex(vertices, 4);
				const GLuint indices[6] = { 0 + BaseIndex, 1 + BaseIndex, 2 + BaseIndex, 0 + BaseIndex, 2 + BaseIndex, 3 + BaseIndex };
				mMesh.AddIndices(indices, 6);

				Position.x += (int32_t)glyph->advance_x;
			}
		}
	}

	void Text::SetStyle(const char* FontFile, uint32_t FontSize, AtlasInfo Atlas)
	{
		texture_atlas_t* AtlasPtr = texture_atlas_new(Atlas.Width, Atlas.Height, Atlas.Depth);

		if (mTextureFont)
			texture_atlas_delete(mTextureFont->atlas);

		mTextureFont = texture_font_new_from_file(AtlasPtr, (float)FontSize, FontFile);
	}

	void Text::Render()
	{
		mTextShader.Use();

		mProjectionUniform.SetMatrix(1, GL_FALSE, &mTextProjection);

		glActiveTexture(GL_TEXTURE0 + GLUniformBindings::TextTexture);
		glBindTexture(GL_TEXTURE_2D, mTextureFont->atlas->id);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mMesh.Activate();
		mMesh.Render();
		mMesh.ClearData();

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);
	}
}
