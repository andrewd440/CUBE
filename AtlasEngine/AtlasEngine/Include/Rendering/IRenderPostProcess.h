#pragma once

class IRenderPostProcess
{
public:

	IRenderPostProcess() = default;
	virtual ~IRenderPostProcess(){};

	virtual void OnPreLightingPass(){}
	virtual void OnPostLightingPass(){}
	virtual void OnPostGUIPass(){}
};

