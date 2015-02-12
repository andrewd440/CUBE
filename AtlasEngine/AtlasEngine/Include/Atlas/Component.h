#pragma once
#include <memory>
#include <cstdint>

namespace Atlas
{
	/**
	* Base class for all Components
	*/
	struct EComponent
	{
		enum Type : uint32_t
		{
			RigidBody,
			Mesh,
			Collider,
			Count
		};
	};

	namespace ComponentTraits
	{
		template <EComponent::Type T>
		struct Object
		{
			using Type = void;
		};

		template <>
		struct Object<EComponent::RigidBody>
		{
			using Type = int;
		};
	}

	class IComponent
	{
	public:
		/** Default Constructor */
		IComponent();

		/** Virtual Destructor */
		virtual	~IComponent(){};
	};
}