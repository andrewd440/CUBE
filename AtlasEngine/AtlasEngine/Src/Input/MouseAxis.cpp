#include "..\..\Include\Input\MouseAxis.h"
#include "..\..\Include\Misc\Assertions.h"

Vector2i FMouseAxis::mLastFramePosition;
Vector2i FMouseAxis::mCurrentFramePosition;
int32_t FMouseAxis::mWheelDelta = 0;

int32_t FMouseAxis::GetHorizontalDelta()
{
	return mCurrentFramePosition.x - mLastFramePosition.x;
}

int32_t FMouseAxis::GetVerticalDelta()
{
	return mCurrentFramePosition.y - mLastFramePosition.y;
}

int32_t FMouseAxis::GetWheelDelta()
{
	return mWheelDelta;
}

void FMouseAxis::Update(const sf::Event& MouseEvent)
{
	ASSERT(IsMouseAxisEvent(MouseEvent));
	switch (MouseEvent.type)
	{
	case sf::Event::MouseMoved:
		mCurrentFramePosition = Vector2i(MouseEvent.mouseMove.x, MouseEvent.mouseMove.y);
		break;
	case sf::Event::MouseWheelMoved:
		mWheelDelta = MouseEvent.mouseWheel.delta;
		break;
	default:
		break;
	}
}

void FMouseAxis::ResetAxes()
{
	mWheelDelta = 0;
	mLastFramePosition = mCurrentFramePosition;
}

bool FMouseAxis::IsMouseAxisEvent(const sf::Event& MouseEvent)
{
	return MouseEvent.type == sf::Event::MouseMoved || MouseEvent.type == sf::Event::MouseWheelMoved;
}
