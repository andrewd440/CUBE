#pragma once

#include "SFML\Window\Event.hpp"
#include <string>

/**
* Facility for retrieving user text input during the
* last frame.
*/
class STextEntered
{
public:
	STextEntered() = delete;
	~STextEntered() = delete;

	/**
	* Clear the current text input buffer.
	*/
	static void Reset();

	/**
	* Add a text event.
	*/
	static void AddTextEvent(const sf::Event& Event);

	/**
	* Retrieve the text input that occured during the
	* last frame.
	*/
	static const std::wstring& GetTextInput();

private:
	static std::wstring mTextInput;
};

