#pragma once

class IImageEffect
{
public:

	IImageEffect() = default;
	virtual ~IImageEffect(){};

	virtual void OnPreLightingPass(){}
	virtual void OnPostLightingPass(){}
	virtual void OnPostGUIPass(){}
};

