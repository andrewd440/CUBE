#pragma once

#include "..\Math\Vector2.h"

/**
* Class for retrieving display information.
*/
class SScreen
{
public:
	SScreen() = delete;

	static void SetResolution(const TVector2<uint32_t> Resolution);

	static TVector2<uint32_t> GetResolution();

private:
	static TVector2<uint32_t> ScreenResolution;
};