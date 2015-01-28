#include "Debugging\DebugText.h"
#include "Math\OrthoMatrix.h"
#include "freetype-gl.h"
#include "Rendering\Screen.h"
#include "ResourceHolder.h"

static const uint32_t DEFAULT_BUFFER_SIZE = 200;

FDebugText::FDebugText()
	: TSingleton()
	, mTextProjection(0, (float)SScreen::GetResolution().x, (float)SScreen::GetResolution().y, 0, 0, 1)
	, mTextShader()
	, mTextureFont(nullptr)
	, mMesh(GL_DYNAMIC_DRAW, DEFAULT_BUFFER_SIZE)
{
	auto& ShaderHolder = FShaderHolder::GetInstance();
	mTextShader.AttachShader(ShaderHolder.Get("DebugTextVertex"));
	mTextShader.AttachShader(ShaderHolder.Get("DebugTextFragment"));
	mTextShader.LinkProgram();
}

FDebugText::~FDebugText()
{
	texture_atlas_delete(mTextureFont->atlas);
	//texture_font_delete(mTextureFont);
}

void FDebugText::AddText(const wchar_t* String, const FColor Color, Vector2i Position)
{
	texture_font_load_glyphs(mTextureFont, String);

	// From https://code.google.com/p/freetype-gl/source/browse/trunk/demo-font.c
	size_t i;
	float r = Color.R, g = Color.G, b = Color.B, a = Color.A;
	for (i = 0; i<wcslen(String); ++i)
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
				{ Vector3f(x0, y0, 0), Vector2f(s0, t0), FColor(r, g, b, a) },
				{ Vector3f(x0, y1, 0), Vector2f(s0, t1), FColor(r, g, b, a) },
				{ Vector3f(x1, y1, 0), Vector2f(s1, t1), FColor(r, g, b, a) },
				{ Vector3f(x1, y0, 0), Vector2f(s1, t0), FColor(r, g, b, a) }
			};

			const uint32_t BaseIndex = mMesh.AddVertex(vertices, 4);
			const GLuint indices[6] = { 0 + BaseIndex, 1 + BaseIndex, 2 + BaseIndex, 0 + BaseIndex, 2 + BaseIndex, 3 + BaseIndex };
			mMesh.AddTriangle(indices, 2);

			Position.x += (int32_t)glyph->advance_x;
		}
	}
}

void FDebugText::SetStyle(const char* FontFile, uint32_t FontSize, AtlasInfo Atlas)
{
	texture_atlas_t* AtlasPtr = texture_atlas_new(Atlas.Width, Atlas.Height, Atlas.Depth);

	if (mTextureFont && mTextureFont->atlas)
		texture_atlas_delete(mTextureFont->atlas);

	mTextureFont = texture_font_new_from_file(AtlasPtr, (float)FontSize, FontFile);

	glBindTexture(GL_TEXTURE_2D, AtlasPtr->id);
}

void FDebugText::Render()
{
	mTextShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(mTextShader.GetID(), "Projection"), 1, GL_FALSE, &mTextProjection.M[0][0]);
	glUniform1i(glGetUniformLocation(mTextShader.GetID(), "Texture"), 0);

	mMesh.Activate();
	mMesh.Render();
	mMesh.ClearData();
}