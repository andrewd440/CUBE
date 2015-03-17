#pragma once

#include "Singleton.h"
#include "Rendering\ShaderProgram.h"
#include "Math\Vector2.h"
#include "Math\Vector3.h"
#include "Math\Color.h"
#include "Rendering\Mesh.h"
#include "Math\OrthoMatrix.h"
#include "Rendering\Uniform.h"

// Forward declaration
struct text_buffer_t;
struct markup_t;

/**
* Singleton class for drawing debug text to an OpenGL window.
*/
namespace FDebug
{
	class Text : public TSingleton<Text>
	{
	public:
		/**
		* Ctor
		*/
		Text();
		~Text();

		/**
		* Add debug text to be displayed this frame. All data added will be erased
		* after each frame.
		* @param String - Text to be displayed.
		* @param Color - Color of the text.
		* @param Position - Size position of the text. Bottom-Left is (0, 0)
		*/
		void AddText(std::wstring& String, Vector2i Position, markup_t& Markup);

		/**
		* Render all text data held by this object. Local data will be cleared
		* after each render call.
		*/
		void Render();

	private:
		FMatrix4 mTextProjection;
		text_buffer_t* mTextBuffer;
		EZGL::FUniform mProjectionUniform;
		EZGL::FUniform mViewUniform;
		EZGL::FUniform mModelUniform;
	};


}