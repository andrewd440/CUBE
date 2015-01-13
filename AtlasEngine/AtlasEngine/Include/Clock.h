#pragma once
#include "SystemClock.h"
#include <cstdint>

/**
* Class for measuring/recording time. Time is measure
* by cpu clock cycles.
* Instances of this class must call FClock::Update
* with an appropriate delta value for the clock to record
* time differences.
*/
class FClock
{
public:
	/**
	* Reads the internal clock cycle of the
	* cpu.
	*/
	static uint64_t ReadClockTimer();

	/**
	* Transforms cpu clock cycles to seconds.
	*/
	static uint64_t SecondsToCycles(float Seconds)
	{
		return (uint64_t)(Seconds * ClockFrequency);
	}

	/**
	* Transforms seconds to cpu clock cycles.
	*/
	static float CyclesToSeconds(uint64_t Cycles)
	{
		return (float)Cycles / ClockFrequency;
	}

private:
	/**
	* Retrieves the clock frequency of the cpu.
	*/
	static const uint64_t GetClockFrequency();

	const static uint64_t ClockFrequency; // Clock frequency of the cpu

public:
	/**
	* Constructs a clock with a specified start time.
	* If no start time is given, the clock starts at 0.
	*/
	explicit FClock(float StartTime = 0.0f)
		: mClockCycles(SecondsToCycles(StartTime))
		, mTimeScale(1.0f)
		, mIsPaused(false)
	{
	}

	~FClock()
	{
	}

	uint64_t GetClockCycles() const 
	{ 
		return mClockCycles; 
	}

	/**
	* Calculates the time differences, if seconds, between
	* two clock objects.
	* @param Other clock object
	* @return Seconds difference from the two clocks (this - Other)
	*/
	float GetTimeDifference(const FClock& Other) 
	{ 
		return CyclesToSeconds(mClockCycles - Other.GetClockCycles()); 
	}

	/**
	* Updates the clock timer from delta time is seconds.
	* If the clock is paused, no update occurs.
	* @param DeltaSeconds Delta time is seconds.
	*/
	void Update(float DeltaSeconds)
	{
		if (!mIsPaused)
		{
			mClockCycles += SecondsToCycles(DeltaSeconds * mTimeScale);
		}
	}

	void SetPause(bool Paused)
	{
		mIsPaused = Paused;
	}

	bool IsPaused() const 
	{ 
		return mIsPaused; 
	}

	void SetTimeScale(float TimeScale)
	{
		mTimeScale = TimeScale;
	}

	float SetTimeScale() const
	{
		return mTimeScale;
	}

private:
	uint64_t mClockCycles; // Current clock cycle count accumulated by the clock
	float mTimeScale; // Scale for the clock's cycle value. < 0 Is slow-mo
	bool mIsPaused; // If the current clock is paused
};
