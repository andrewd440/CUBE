#pragma once
#include <bitset>
#include "SFML\Window\Keyboard.hpp"
#include "SFML\Window\Mouse.hpp"
#include "SFML\Window\Event.hpp"

/**
* Manager class for input button events. (i.e. button up, button down)
* Only includes static function and data, so it is
* not ment to be instatiated. Button events are updated
* on each frame.
*/
class FButtonEvent
{
public:
	FButtonEvent() = delete;
	~FButtonEvent() = delete;

	/**
	* Resets all currently store button events.
	*/
	static void ResetButtonEvents();

	/**
	* Registers a button event.
	* @return True if the event was successfully registered. False
	*	if the event was not a button event.
	*/
	static bool AddButtonEvent(const sf::Event& ButtonEvent);

	/**
	* Checks if a keyboard key was released in the last frame.
	* @param Key to be checked.
	*/
	static bool GetKeyUp(const sf::Keyboard::Key Key);

	/**
	* Checks if a keyboard key was pressed in the last frame.
	* @param Key to be checked.
	*/
	static bool GetKeyDown(const sf::Keyboard::Key Key);

	/**
	* Checks if a mouse button was released in the last frame.
	* @param Button to be checked.
	*/
	static bool GetMouseUp(const sf::Mouse::Button Button);

	/**
	* Checks if a mouse button was pressed in the last frame.
	* @param Button to be checked.
	*/
	static bool GetMouseDown(const sf::Mouse::Button Button);

	/**
	* Checks if an event is of a button type. These include
	* KeyReleased, KeyPressed, MouseButtonReleased, and 
	* MouseButtonPressed.
	*/
	static bool IsButtonEvent(const sf::Event& Event);

private:
	static std::bitset<sf::Keyboard::KeyCount> mKeyUps;
	static std::bitset<sf::Keyboard::KeyCount> mKeyDowns;

	static std::bitset<sf::Mouse::ButtonCount> mMouseUps;
	static std::bitset<sf::Mouse::ButtonCount> mMouseDowns;
};

