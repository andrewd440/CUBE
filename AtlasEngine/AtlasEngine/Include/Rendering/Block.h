#pragma once

class FBlock
{
public:
	FBlock();
	~FBlock() = default;

	bool IsActive() const;
	void SetActive(bool IsActive);

private:
	bool mIsActive;
};

inline FBlock::FBlock()
	: mIsActive(true)
{

}

inline bool FBlock::IsActive() const
{
	return mIsActive;
}

inline void FBlock::SetActive(bool IsActive)
{
	mIsActive = IsActive;
}