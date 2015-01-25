#pragma once

/**
* A voxel block.
*/
class FBlock
{
public:
	static const float BLOCK_SIZE;
public:
	FBlock();
	~FBlock() = default;

	/**
	* Check if the block is active.
	*/
	bool IsActive() const;

	/**
	* Set the status of the block.
	*/
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