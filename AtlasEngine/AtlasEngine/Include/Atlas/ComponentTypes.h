#pragma once

#include <cstdint>

namespace Atlas
{
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
	}
}