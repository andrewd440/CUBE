#include "StringID.h"
#include "Debugging\ConsoleOutput.h"
#include "Misc/Assertions.h"
#include "Misc/StringUtil.h"
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
		FDebug::PrintF("StringID: %s maps to an already used index and must be changed.", Name);
	}
}

FStringID::FStringID(const std::string& Name)
	: FStringID(Name.c_str())
{
}

const std::string FStringID::GetName() const
{
	ASSERT(NameMap.find(mID) != NameMap.end() && "Trying to retrieve a key that is not in NameMap");
	return NameMap[mID];
}