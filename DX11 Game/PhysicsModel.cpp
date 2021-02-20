#include "stdafx.h"
#include "PhysicsModel.h"

PhysicsModel::PhysicsModel(RenderableGameObject* transform) : mTransform(transform)
{
	mMass = 100.0f;
	mUseLaminar = false;
	mPosition = mTransform->GetPositionFloat3();

	// object weight = mass * gravity
	mWeight = mMass * mGravity;

	mNetForce = { 0.0f, 0.0f, 0.0f };
	mVelocity = { 0.0f, 0.0f, 0.0f };
	mAcceleration = { 0.0f, 0.0f, 0.0f };
	mDrag = { 0.0f, 0.0f, 0.0f };
}

PhysicsModel::~PhysicsModel()
{

}

void PhysicsModel::Update(const float deltaTime)
{
	AddWeight();
	AddDrag();
	CalculateAcceleration();
	//AddFriction(deltaTime);
	CalculateVelocity(deltaTime);
	CalculatePosition(deltaTime);
	AddGravity();

	mNetForce = { 0.0f, 0.0f, 0.0f };
}

void PhysicsModel::AddWeight()
{
	mNetForce.y -= mWeight;
}

void PhysicsModel::CalculateAcceleration()
{
	mAcceleration.x = mNetForce.x / mMass;
	mAcceleration.y = mNetForce.y / mMass;
	mAcceleration.z = mNetForce.z / mMass;
}

void PhysicsModel::CalculateVelocity(const float deltaTime)
{
	mVelocity.x += mAcceleration.x * deltaTime;
	mVelocity.y += mAcceleration.y * deltaTime;
	mVelocity.z += mAcceleration.z * deltaTime;
}

void PhysicsModel::AddFriction(const float deltaTime)
{

}

void PhysicsModel::AddDrag()
{
	if (mUseLaminar)
		LaminarDrag();
	else
		TurbulentDrag();
}

void PhysicsModel::LaminarDrag()
{
	mDrag.x = mDragFactor * mVelocity.x;
	mDrag.y = mDragFactor * mVelocity.y;
	mDrag.z = mDragFactor * mVelocity.z;
}

void PhysicsModel::TurbulentDrag()
{
	float magVelocity = Magnitude(mVelocity);
	XMFLOAT3 unitVelocity = Normalization(mVelocity);

	float dragMagnitude = mDragFactor * magVelocity * magVelocity;

	mDrag.x = -dragMagnitude * mDragFactor * unitVelocity.x;
	mDrag.y = -dragMagnitude * mDragFactor * unitVelocity.y;
	mDrag.z = -dragMagnitude * mDragFactor * unitVelocity.z;
}

void PhysicsModel::CalculatePosition(const float deltaTime)
{
	mPosition = mTransform->GetPositionFloat3();
	mPosition.x += mVelocity.x * deltaTime + 0.5f * mAcceleration.x * deltaTime * deltaTime;
	mPosition.y += mVelocity.y * deltaTime + 0.5f * mAcceleration.y * deltaTime * deltaTime;
	mPosition.z += mVelocity.z * deltaTime + 0.5f * mAcceleration.z * deltaTime * deltaTime;

	CalculateVelocity(deltaTime);

	mTransform->SetPosition(mPosition);
}

void PhysicsModel::AddGravity()
{
	mPosition = mTransform->GetPositionFloat3();

	if (mPosition.y < 1.0f)
	{
		mVelocity = { mVelocity.x, 0.0f, mVelocity.z };
		mPosition.y = 1.0f;
		mTransform->SetPosition(mPosition);
	}
	else if (mPosition.y > 1.0f)
	{
		mVelocity.y -= 0.01f;
	}
}

XMFLOAT3 PhysicsModel::Normalization(XMFLOAT3 vec) const
{
	XMFLOAT3 temp = vec;
	float unitVector = sqrt((temp.x * temp.x) + (temp.y * temp.y) + (temp.z * temp.z));

	temp.x = temp.x / unitVector;
	temp.y = temp.y / unitVector;
	temp.z = temp.z / unitVector;

	return temp;
}
