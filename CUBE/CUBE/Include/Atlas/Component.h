#pragma once

namespace Atlas
{
	/**
	* Base class for all Components
	*/
	class IComponent
	{
	public:
		/** Default Constructor */
		IComponent(){}

		/** Virtual Destructor */
		virtual	~IComponent(){}
	};
}