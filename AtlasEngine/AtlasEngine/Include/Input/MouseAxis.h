#pragma once
#include "SFML\Window\Mouse.hpp"

#include <cstdint>
#include <functional>
#include "SFML\Window\Event.hpp"
#include "..\Math\Vector2.h"

/**
* Class for retrieving changes in mouse
* axes (i.e. Mouse position and wheel scroll).
*/
class SMouseAxis
{	
public:
	SMouseAxis() = delete;
	~SMouseAxis() = delete;

	/**
	* Retrieve the delta movement of the mouse
	* during the last game frame.
	*/
	static Vector2i GetDelta();

	/**
	* Retrieve the wheel movement of the mouse
	* during the last game frame.
	*/
	static int32_t GetWheelDelta();

	/**
	* Updates mouse movement for the current frame. Upon a new frame
	* this should be preceeded by ResetAxes() to clear the previous
	* frame's values. Then all mouse movement events should be feed
	* into this update.
	* @param MouseEvent - Window event for a mouse axis.
	*/
	static void UpdateEvent(const sf::Event& MouseEvent);

	/**
	* Update the mouse delta movement for the current frame.
	* This is calculated by the difference between the default
	* mouse position and the current mouse position.
	* @param Window - The active window.
	*/
	static void UpdateDelta(const sf::Window& Window);

	/**
	* Resets the mouse movement values from the last frame. This should
	* be called before updating deltas for the current frame.
	*/
	static void ResetAxes();

	/**
	* Checks if a specific windows event is a mouse axis event.
	*/
	static bool IsMouseAxisEvent(const sf::Event& MouseEvent);

	/**
	* Set the default position of the mouse. This is the position that
	* the mouse is reset to at the end of each frame. Mouse delta movement
	* is calculated from this position.
	*/
	static void SetDefaultMousePosition(Vector2i Position);

	/**
	* Get the default position of the mouse. This is the position that
	* the mouse is reset to at the end of each frame. Mouse delta movement
	* is calculated from this position.
	*/
	static Vector2i GetDefaultMousePosition();

private:
	static Vector2i mDefaultMousePosition;
	static Vector2i mMouseDelta;
	static int32_t mWheelDelta;

};
