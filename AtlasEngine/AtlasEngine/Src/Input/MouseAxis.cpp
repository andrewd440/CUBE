#include "..\..\Include\Input\MouseAxis.h"
#include "..\..\Include\Misc\Assertions.h"
#include "SFML\Window\Window.hpp"

Vector2i SMouseAxis::mLastMousePosition;
Vector2i SMouseAxis::mDefaultMousePosition;
Vector2i SMouseAxis::mMouseDelta;
sf::Window* SMouseAxis::mWindow = nullptr;
int32_t SMouseAxis::mWheelDelta = 0;
bool SMouseAxis::mIsLocked = false;


Vector2i SMouseAxis::GetDelta()
{
	return mMouseDelta;
}

int32_t SMouseAxis::GetWheelDelta()
{
	return mWheelDelta;
}

void SMouseAxis::UpdateEvent(const sf::Event& MouseEvent)
{
	ASSERT(IsMouseAxisEvent(MouseEvent));
	switch (MouseEvent.type)
	{
	case sf::Event::MouseWheelMoved:
		mWheelDelta = MouseEvent.mouseWheel.delta;
		break;
	default:
		break;
	}
}

void SMouseAxis::UpdateDelta()
{
	const sf::Vector2i CurrentPosition = sf::Mouse::getPosition(*mWindow);
	const Vector2i LastPosition = mLastMousePosition;
	mMouseDelta = Vector2i(CurrentPosition.x - LastPosition.x, CurrentPosition.y - LastPosition.y);
	
	mLastMousePosition = Vector2i{ CurrentPosition.x, CurrentPosition.y };

	if (mIsLocked)
	{
		sf::Mouse::setPosition(sf::Vector2i{ mDefaultMousePosition.x, mDefaultMousePosition.y }, *mWindow);
		mLastMousePosition = mDefaultMousePosition;
	}
}

void SMouseAxis::ResetAxes()
{
	mWheelDelta = 0;
	mMouseDelta = Vector2i(0, 0);
}

bool SMouseAxis::IsMouseAxisEvent(const sf::Event& MouseEvent)
{
	return MouseEvent.type == sf::Event::MouseMoved || MouseEvent.type == sf::Event::MouseWheelMoved;
}

void SMouseAxis::SetDefaultMousePosition(Vector2i Position)
{
	mDefaultMousePosition = Position;
}

Vector2i SMouseAxis::GetDefaultMousePosition()
{
	return mDefaultMousePosition;
}

void SMouseAxis::SetMouseLock(bool IsLocked)
{
	mIsLocked = IsLocked;
}

void SMouseAxis::SetMouseVisible(const bool IsVisible)
{
	mWindow->setMouseCursorVisible(IsVisible);
}

void SMouseAxis::SetWindow(sf::Window& Window)
{
	mWindow = &Window;
}