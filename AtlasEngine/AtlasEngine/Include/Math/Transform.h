#pragma once

#include "Vector3.h"
#include "Quaternion.h"

/**
* Class for representing 3D spatial information for an object.
*/
class FTransform
{
public:
	/**
	* Ctor
	* Constructs an identity transform.
	*/
	FTransform();

	/**
	* Ctor
	* Constructs a transform with a specified origin position.
	*/
	FTransform(const Vector3f& Position);

	/**
	* Copy Ctor
	* Copies tranform spatial data and inherits parent information.
	*/
	FTransform(const FTransform& Other);

	/**
	* Dtor
	*/
	~FTransform() = default;

	/**
	* Copies tranform spatial data and inherits parent information.
	*/
	FTransform& operator=(const FTransform& Other);

	/**
	* Constructs a matrix that will transform local coordinates
	* to world coordinates.
	*/
	FMatrix4 LocalToWorldMatrix() const;

	/**
	* Constructs a matrix that will transform world coordinates
	* to local coordinates of this transform.
	*/
	FMatrix4 WorldToLocalMatrix() const;

	/**
	* Set the position of this tranform.
	*/
	void SetLocalPosition(const Vector3f& NewPosition);

	/**
	* Get the position of this transform.
	*/
	Vector3f GetLocalPosition() const;

	/**
	* Get the world position of this transform.
	*/
	Vector3f GetWorldPosition() const;

	/**
	* Translates this tranform.
	*/
	void Translate(const Vector3f& Translation);

	/**
	* Set the rotation.
	*/
	void SetRotation(const FQuaternion& NewRotation);

	/**
	* Get the rotation.
	*/
	FQuaternion GetRotation() const;

	/**
	* Rotates this tranform by a given amount.
	*/
	void Rotate(const FQuaternion& Rotation);

	/**
	* Set the scale.
	*/
	void SetScale(const Vector3f NewScale);

	/**
	* Get the scale.
	*/
	Vector3f GetScale() const;

	/**
	* Set the parent of this object. All transform data will be
	* relative to the parent if set.
	*/
	void SetParent(FTransform* NewParent);

	/**
	* Get the parent of this object. All transform data will be
	* relative to the parent if set.
	*/
	FTransform* GetParent() const;

private:
	FQuaternion mRotation;
	Vector3f mTranslation;
	Vector3f mScale;
	FTransform* mParent;
};

#include "Transform.inl"