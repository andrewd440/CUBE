#include "..\Include\Camera.h"


FCamera::FCamera()
{
	// If this is the first camera, assign it to main
	if (!Main)
		Main = this;
}


FCamera::~FCamera()
{
	// If this was the main camera, remove it
	if (Main == this)
		Main = nullptr;
}
