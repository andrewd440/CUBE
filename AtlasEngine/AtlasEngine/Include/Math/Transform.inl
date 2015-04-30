#pragma once

//////////////////////////////////////////////////////////////////////////////////////
////////////////////// Inlined FTransform ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

inline FTransform::FTransform(const Vector3f& Position, const float Scale)
	: mTranslation(Position)
	, mRotation()
	, mScale(Scale, Scale, Scale)
	, mParent(nullptr)
{
}

inline FTransform::FTransform(const FTransform& Other)
	: mTranslation(Other.mTranslation)
	, mRotation(Other.mRotation)
	, mScale(Other.mScale)
	, mParent(Other.mParent)
{
}

inline FTransform& FTransform::operator=(const FTransform& Other)
{
	mTranslation = Other.mTranslation;
	mRotation = Other.mRotation;
	mScale = Other.mScale;
	mParent = Other.mParent;

	return *this;
}

inline void FTransform::SetLocalPosition(const Vector3f& NewPosition)
{
	mTranslation = NewPosition;
}

inline Vector3f FTransform::GetLocalPosition() const
{
	return mTranslation;
}

inline void FTransform::Translate(const Vector3f& Translation)
{
	mTranslation += (mRotation * Translation);
}

inline void FTransform::SetRotation(const FQuaternion& NewRotation)
{
	mRotation = NewRotation;
}

inline FQuaternion FTransform::GetRotation() const
{
	return mRotation;
}

inline void FTransform::Rotate(const FQuaternion& Rotation)
{
	mRotation *= Rotation;
}

inline void FTransform::SetScale(const Vector3f NewScale)
{
	mScale = NewScale;
}

inline Vector3f FTransform::GetScale() const
{
	return mScale;
}

inline void FTransform::SetParent(FTransform* NewParent)
{
	mParent = NewParent;
}

inline FTransform* FTransform::GetParent() const
{
	return mParent;
}


//////////////////////////////////////////////////////////////////////////////////////
////////////////////// Non-Member Functions //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

inline bool operator==(const FTransform& Lhs, const FTransform& Rhs)
{
	return Lhs.GetParent()	== Rhs.GetParent() &&
		Lhs.GetLocalPosition() == Rhs.GetLocalPosition() &&
		Lhs.GetRotation()	== Rhs.GetRotation() &&
		Lhs.GetScale()		== Rhs.GetScale();
}

inline bool operator!=(const FTransform& Lhs, const FTransform& Rhs)
{
	return Lhs.GetParent() != Rhs.GetParent() ||
		Lhs.GetLocalPosition() != Rhs.GetLocalPosition() ||
		Lhs.GetRotation() != Rhs.GetRotation() ||
		Lhs.GetScale() != Rhs.GetScale();
}