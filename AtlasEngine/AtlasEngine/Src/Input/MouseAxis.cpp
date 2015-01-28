#include "..\..\Include\Input\MouseAxis.h"
#include "..\..\Include\Misc\Assertions.h"

Vector2i SMouseAxis::mDefaultMousePosition;
Vector2i SMouseAxis::mMouseDelta;
int32_t SMouseAxis::mWheelDelta = 0;

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
	//case sf::Event::MouseMoved:
	//	break;
	case sf::Event::MouseWheelMoved:
		mWheelDelta = MouseEvent.mouseWheel.delta;
		break;
	default:
		break;
	}
}

void SMouseAxis::UpdateDelta(const sf::Window& Window)
{
	const sf::Vector2i CurrentPosition = sf::Mouse::getPosition(Window);
	const Vector2i DefaultPosition = mDefaultMousePosition;
	mMouseDelta = Vector2i(CurrentPosition.x - DefaultPosition.x, CurrentPosition.y - DefaultPosition.y);
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