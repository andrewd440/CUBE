#include "..\Include\AtlasRoot.h"
#include <cstdint>

#include "..\Include\Clock.h"
#include "..\Include\FTime.h"
#include "..\Include\WindowEvent.h"
#include "..\Include\Input\ButtonEvent.h"
#include "..\Include\Input\MouseAxis.h"
#include "..\Include\Debugging\ConsoleOutput.h"

namespace
{
	static const uint32_t WindowWidth = 1000;
	static const uint32_t WindowHeight = 600;
}

FAtlasRoot::FAtlasRoot()
	: mGameWindow(FVideoMode{ WindowWidth, WindowHeight }, L"Atlas Engine")
{

}


FAtlasRoot::~FAtlasRoot()
{
}


void FAtlasRoot::Start()
{
	GameLoop();
}

void FAtlasRoot::GameLoop()
{
	// Setup game timers
	FClock GameTimer;
	uint64_t FrameStart, FrameEnd;
	FTime::SetDeltaTime(1.0f / 30.0f); // Default delta time with 30fps
	FrameStart = FClock::ReadSystemTimer();

	// Game Loop
	while (mGameWindow.isOpen())
	{
		// Manage frame timers
		FrameEnd = FClock::ReadSystemTimer();
		uint64_t FrameTime = FrameEnd - FrameStart;

		// Update the game clock with this frame's time and
		// compute the frame delta time by taking the game timer's time
		// before and after this update (GameTimer may be time scaled)
		const uint64_t PreUpdateTimer = GameTimer.GetCycles();
		GameTimer.Update(FClock::CyclesToSeconds(FrameTime));
		const uint64_t PostUpdateTimer = GameTimer.GetCycles();

		float DeltaTime = FClock::CyclesToSeconds(PostUpdateTimer - PreUpdateTimer);

		// If large delta time, we were probably in a breakpoint
		if (DeltaTime > 1.5f)
		{
			DeltaTime = 1.0f / 30.0f;
		}

		// Set delta time for this frame
		FTime::SetDeltaTime(DeltaTime);
		FrameStart = FrameEnd;

		ServiceEvents();
		
		mGameWindow.display();
		Sleep(100);
	}
}

void FAtlasRoot::ServiceEvents()
{
	// Windows events
	FWindowEvent Event;

	// Reset button and axes from previous frame
	FButtonEvent::ResetButtonEvents();
	FMouseAxis::ResetAxes();

	// Service window events
	while (mGameWindow.pollEvent(Event))
	{
		if (Event.type == Event.Closed)
			mGameWindow.close();

		if (FButtonEvent::IsButtonEvent(Event))
		{
			FButtonEvent::AddButtonEvent(Event);
		}
		else if (FMouseAxis::IsMouseAxisEvent(Event))
		{
			FMouseAxis::Update(Event);
		}
	}
}