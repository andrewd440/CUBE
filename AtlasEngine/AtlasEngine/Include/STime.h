#pragma once

#include "Clock.h"

/**
* Interface for useful time values.
*/
class STime
{
public:
	static float GetDeltaTime()
	{
		return mDeltaTime;
	}

	static float GetFixedUpdate()
	{
		return mFixedUpdate;
	}

	static void SetFixedUpdate(float Time)
	{
		mFixedUpdate = Time;
	}

	static FClock& GetGameClock()
	{
		return mGameClock;
	}

	static void StartGameTimer();

	static void UpdateGameTimer();

private:
	static FClock mGameClock;
	static uint64_t mFrameStart;
	static uint64_t mFrameEnd;
	static float mDeltaTime;
	static float mFixedUpdate;
};

