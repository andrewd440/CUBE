#include "..\..\Include\Input\ButtonEvent.h"
#include "..\..\Include\Misc\Assertions.h"

std::bitset<FKeyboard::KeyCount> FButtonEvent::mKeyUps;
std::bitset<FKeyboard::KeyCount> FButtonEvent::mKeyDowns;

std::bitset<FMouse::ButtonCount> FButtonEvent::mMouseUps;
std::bitset<FMouse::ButtonCount> FButtonEvent::mMouseDowns;

void FButtonEvent::ResetButtonEvents()
{
	mKeyDowns.reset();
	mKeyUps.reset();
	mMouseDowns.reset();
	mMouseUps.reset();
}

bool FButtonEvent::AddButtonEvent(const FWindowEvent& ButtonEvent)
{
	// Ensure that only button events are passed to this function.
	ASSERT(IsButtonEvent(ButtonEvent));

	switch (ButtonEvent.type)
	{
	case FWindowEvent::KeyPressed:
		mKeyDowns[ButtonEvent.key.code] = true;
		return true;
		break;
	case FWindowEvent::KeyReleased:
		mKeyUps[ButtonEvent.key.code] = true;
		return true;
		break;
	case FWindowEvent::MouseButtonPressed:
		mMouseDowns[ButtonEvent.key.code] = true;
		return true;
		break;
	case FWindowEvent::MouseButtonReleased:
		mMouseUps[ButtonEvent.key.code] = true;
		return true;
		break;
	}

	return false;
}

bool FButtonEvent::GetKeyUp(const FKeyboard::Key Key)
{
	return mKeyUps[Key];
}

bool FButtonEvent::GetKeyDown(const FKeyboard::Key Key)
{
	return mKeyDowns[Key];
}

bool FButtonEvent::GetMouseUp(const FMouse::Button Button)
{
	return mMouseUps[Button];
}

bool FButtonEvent::GetMouseDown(const FMouse::Button Button)
{
	return mMouseDowns[Button];
}

bool FButtonEvent::IsButtonEvent(const FWindowEvent& Event)
{
	return Event.type == FWindowEvent::KeyPressed || Event.type == FWindowEvent::KeyReleased
		|| Event.type == FWindowEvent::MouseButtonPressed || Event.type == FWindowEvent::MouseButtonReleased;
}