#pragma once
#include <cstdint>
#include <string>

class FStringID
{
public:
	using StringID = uint32_t;

	FStringID(const char* Name);
	FStringID(const std::string& Name);
	FStringID(const FStringID& Other) = default;
	~FStringID();

	StringID GetID() const;

	void SetName(const char* Name);
	void SetName(const std::string& Name);

private:
	StringID mID;
};

