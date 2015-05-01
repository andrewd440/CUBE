#pragma once
#include <cstdint>
#include <string>

/**
* A class the allows string names to be represented
* as a compact integer, which allows for faster and more
* efficient operations than when done with strings. All strings
* are hashed into IDs and stored in a hashtable for fast lookups.
*/
class FStringID
{
public:
	using StringID = uint32_t;

	/**
	* Construct a string id from a c-string.
	* @param Name for the string id
	*/
	explicit FStringID(const char* Name);

	/**
	* Construct a string id from a std::string.
	* @param Name for the string id
	*/
	explicit FStringID(const std::string& Name);

	// Default the copy ctor and assignment
	FStringID(const FStringID& Other) = default;
	FStringID& operator=(const FStringID& Other) = default;

	~FStringID() = default;

	/**
	* Retrieves the integer representation of the string id.
	*/
	StringID GetID() const { return mID; };

	/**
	* Retrieves the string representation of the string id.
	*/
	const std::string GetName() const;

private:
	StringID mID;
};

