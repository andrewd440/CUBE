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

void FMouseAxis::Update(const FWindowEvent& MouseEvent)
{
	ASSERT(IsMouseAxisEvent(MouseEvent));
	switch (MouseEvent.type)
	{
	case FWindowEvent::MouseMoved:
		mCurrentFramePosition = Vector2i(MouseEvent.mouseMove.x, MouseEvent.mouseMove.y);
		break;
	case FWindowEvent::MouseWheelMoved:
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

bool FMouseAxis::IsMouseAxisEvent(const FWindowEvent& MouseEvent)
{
	return MouseEvent.type == FWindowEvent::MouseMoved || MouseEvent.type == FWindowEvent::MouseWheelMoved;
}
