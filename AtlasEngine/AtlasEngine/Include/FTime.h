#pragma once

/**
* Interface for useful time values.
*/
class FTime
{
public:
	static float GetDeltaTime()
	{
		return DeltaTime;
	}

	static void SetDeltaTime(float Time)
	{
		DeltaTime = Time;
	}

	static float GetFixedUpdate()
	{
		return FixedUpdate;
	}

	static void SetFixedUpdate(float Time)
	{
		FixedUpdate = Time;
	}

private:
	static float DeltaTime;
	static float FixedUpdate;
};

float FTime::DeltaTime = 0;
float FTime::FixedUpdate = 0;

