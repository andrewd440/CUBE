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
class FMouseAxis
{	
public:
	FMouseAxis() = delete;
	~FMouseAxis() = delete;

	/**
	* Retrieve the horizontal movement of the mouse
	* during the last game frame.
	*/
	static int32_t GetHorizontalDelta();

	/**
	* Retrieve the vertical movement of the mouse
	* during the last game frame.
	*/
	static int32_t GetVerticalDelta();

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
	static void Update(const sf::Event& MouseEvent);

	/**
	* Resets the mouse movement values from the last frame. This should
	* be called before updating deltas for the current frame.
	*/
	static void ResetAxes();

	/**
	* Checks if a specific windows event is a mouse axis event.
	*/
	static bool IsMouseAxisEvent(const sf::Event& MouseEvent);

private:
	static Vector2i mLastFramePosition;
	static Vector2i mCurrentFramePosition;
	static int32_t mWheelDelta;

};
