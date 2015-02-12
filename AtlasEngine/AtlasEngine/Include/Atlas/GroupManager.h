#pragma once
#include "FGameObject.h"

#include <unordered_map>


namespace Atlas
{
	/**
	* Used to organize entities into named groups.
	*/
	class GroupManager : public NonCopyable
	{
	public:
		/** Default Constructor */
		GroupManager();

		/**
		* Adds an FGameObject to a group.
		* @param name - the name of the group
		* @param entity - the FGameObject to be added
		*/
		void addToGroup(const std::string& name, const FGameObject& entity);

		/**
		* Removes an FGameObject from all groups that it is in.
		* @param entity - the FGameObject to be removed
		*/
		void removeFromAllGroups(const FGameObject& entity);

		/**
		* Removes an FGameObject from a specific group.
		* @param name - the name of the group
		* @param entity - the FGameObject to be removed
		* @param fullErase - only true if FGameObject is being removed from all groups
		*/
		void removeFromGroup(const std::string& name, const FGameObject& entity, bool fullErase = false);

		/**
		* Retrieves all Entities from a group.
		* @param name - the name of the group
		* @return container of FGameObject::IDs that are in the group
		*/
		std::vector<FGameObject::ID> getGroup(const std::string& name);

		/**
		* Retrieves all groups that an FGameObject is in.
		* @param entity - the FGameObject of interest.
		* @return container of named groups that the FGameObject is in.
		*/
		std::vector<std::string> getFGameObjectGroups(const FGameObject& entity) const;

		/**
		* Prints each group with the ID's of the Entities that are included.
		*/
		void toString();

	private:
		/** Holds a table organized as Group - Entities */
		std::unordered_map < std::string,
			std::vector < FGameObject::ID >>              mGroupsByName;

		/** Holds a table organized as ID - Groups */
		std::vector<std::vector<std::string>>        mFGameObjectGroups;
	};
}

