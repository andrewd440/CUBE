#pragma once

/**
* Interface for useful time values.
*/
class STime
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

float STime::DeltaTime = 0;
float STime::FixedUpdate = 0;

