#include "Math\Transform.h"


Vector3f FTransform::GetWorldPosition() const
{
	if (!mParent)
		return mTranslation;

	return mParent->GetWorldPosition() + mTranslation;
}

FMatrix4 FTransform::LocalToWorldMatrix() const
{
	FMatrix4 LocalToWorld;
	LocalToWorld.Scale(mScale);
	LocalToWorld *= mRotation.ToMatrix4();
	LocalToWorld.SetOrigin(mTranslation);

	if (!mParent)
	{
		return LocalToWorld;
	}

	return mParent->LocalToWorldMatrix() * LocalToWorld;
}

FMatrix4 FTransform::WorldToLocalMatrix() const
{
	return LocalToWorldMatrix().GetInverse();
}