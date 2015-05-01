#include "STime.h"

FClock STime::mGameClock;
uint64_t STime::mFrameStart = 0;
uint64_t STime::mFrameEnd = 0;
float STime::mDeltaTime = 1.0f / 30.0f;
float STime::mFixedUpdate = 0;

void STime::StartGameTimer()
{
	mFrameStart = FClock::ReadSystemTimer();
}

void STime::UpdateGameTimer()
{
	// Manage frame timers
	mFrameEnd = FClock::ReadSystemTimer();
	uint64_t FrameTime = mFrameEnd - mFrameStart;

	// Update the game clock with this frame's time and
	// compute the frame delta time by taking the game timer's time
	// before and after this update (GameTimer may be time scaled)
	const uint64_t PreUpdateTimer = mGameClock.GetCycles();
	mGameClock.Update(FClock::CyclesToSeconds(FrameTime));
	const uint64_t PostUpdateTimer = mGameClock.GetCycles();

	float DeltaTime = FClock::CyclesToSeconds(PostUpdateTimer - PreUpdateTimer);

	// If large delta time, we were probably in a breakpoint
	if (DeltaTime > 1.5f)
	{
		DeltaTime = 1.0f / 30.0f;
	}

	// Set delta time for this frame
	mDeltaTime = DeltaTime;
	mFrameStart = mFrameEnd;
}