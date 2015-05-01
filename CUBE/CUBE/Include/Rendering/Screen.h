#pragma once

#include "..\Math\Vector2.h"

/**
* Class for retrieving display information.
*/
class SScreen
{
public:
	SScreen() = delete;

	static TVector2<uint32_t> GetResolution();

	static float GetAspectRatio();

private:
	friend class FRenderSystem;
	static void SetResolution(const TVector2<uint32_t> Resolution);

private:
	static TVector2<uint32_t> ScreenResolution;
};