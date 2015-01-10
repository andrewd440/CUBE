#include "..\Include\StringID.h"
#include "../Include/Misc/Assertions.h"
#include "../Include/Misc/StringUtil.h"
#include <unordered_map>

static std::unordered_map<uint32_t, std::string> NameMap;

FStringID::FStringID(const char* Name)
{
	mID = FString::HashCRC32(Name);
	
	if (NameMap.find(mID) == NameMap.end())
	{
		// Name is a new entry into the namemap
		NameMap[mID] = Name;
	}
	
	if (NameMap[mID] != Name)
	{
		std::cerr << "StringID: " << Name << " maps to an already used index and must be changed." << std::endl;
	}
}

FStringID::FStringID(const std::string& Name)
{
	mID = FString::HashCRC32(Name.c_str());

	if (NameMap.find(mID) == NameMap.end())
	{
		// Name is a new entry into the namemap
		NameMap[mID] = Name;
	}

	if (NameMap[mID] != Name)
	{
		std::cerr << "StringID: " << Name << " maps to an already used index and must be changed." << std::endl;
	}
}

const std::string FStringID::GetName() const
{
	ASSERT(NameMap.find(mID) != NameMap.end() && "Trying to retrieve a key that is not in NameMap");
	return NameMap[mID];
}