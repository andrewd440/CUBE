#include "..\..\Include\Rendering\Screen.h"

TVector2<uint32_t> SScreen::ScreenResolution;

void SScreen::SetResolution(const TVector2<uint32_t> Resolution)
{
	ScreenResolution = Resolution;
}

TVector2<uint32_t> SScreen::GetResolution()
{
	return ScreenResolution;
}
