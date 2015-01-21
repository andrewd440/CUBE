#include "..\..\Include\Input\MouseAxis.h"
#include "..\..\Include\Misc\Assertions.h"

Vector2i FMouseAxis::mDefaultMousePosition;
Vector2i FMouseAxis::mMouseDelta;
int32_t FMouseAxis::mWheelDelta = 0;

Vector2i FMouseAxis::GetDelta()
{
	return mMouseDelta;
}

int32_t FMouseAxis::GetWheelDelta()
{
	return mWheelDelta;
}

void FMouseAxis::UpdateEvent(const sf::Event& MouseEvent)
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

void FMouseAxis::UpdateDelta(const sf::Window& Window)
{
	const sf::Vector2i CurrentPosition = sf::Mouse::getPosition(Window);
	const Vector2i DefaultPosition = mDefaultMousePosition;
	mMouseDelta = Vector2i(CurrentPosition.x - DefaultPosition.x, CurrentPosition.y - DefaultPosition.y);
}

void FMouseAxis::ResetAxes()
{
	mWheelDelta = 0;
	mMouseDelta = Vector2i(0, 0);
}

bool FMouseAxis::IsMouseAxisEvent(const sf::Event& MouseEvent)
{
	return MouseEvent.type == sf::Event::MouseMoved || MouseEvent.type == sf::Event::MouseWheelMoved;
}

void FMouseAxis::SetDefaultMousePosition(Vector2i Position)
{
	mDefaultMousePosition = Position;
}

Vector2i FMouseAxis::GetDefaultMousePosition()
{
	return mDefaultMousePosition;
}