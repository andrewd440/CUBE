#pragma once

#include "Singleton.h"
#include "Rendering\ShaderProgram.h"
#include "Math\Vector2.h"
#include "Math\Vector3.h"
#include "Math\Color.h"
#include "Rendering\Mesh.h"
#include "Math\OrthoMatrix.h"

// Forward declaration
struct texture_font_t;

/**
* Vertex type for text rendering.
*/
struct FTextVertex
{
	Vector3f Position;
	Vector2f UV;
	FColor   Color;
};

/**
* Singleton class for drawing debug text to an OpenGL window.
*/
class FDebugText : public TSingleton<FDebugText>
{
public:
	struct AtlasInfo
	{
		uint32_t Width;
		uint32_t Height;
		uint32_t Depth;
	};

public:
	/**
	* Ctor
	*/
	FDebugText();
	~FDebugText();

	/**
	* Add debug text to be displayed this frame. All data added will be erased
	* after each frame.
	* @param String - Text to be displayed.
	* @param Color - Color of the text.
	* @param Position - Size position of the text. Bottom-Left is (0, 0)
	*/
	void AddText(const wchar_t* String, const FColor Color, Vector2i Position);

	/**
	* Set the font style for debug text.
	* @param FontFile - Filepath to a font file.
	* @param FontSize - Size of the font.
	* @param Atlas - Details for the texture atlas for the font.
	*/
	void SetStyle(const char* FontFile, uint32_t FontSize, AtlasInfo Atlas);

	/**
	* Render all text data held by this object. Local data will be cleared
	* after each render call.
	*/
	void Render();

private:
	FOrthoMatrix mTextProjection;
	FShaderProgram mTextShader;
	texture_font_t* mTextureFont;
	TMesh<FTextVertex> mMesh;
};

