#include "stdafx.h"
#include "PhysicsModel.h"

PhysicsModel::PhysicsModel( GameObject* transform ) : mTransform( transform )
{
	mMass = 100.0f;
	mActivated = false;
	mUseLaminar = false;
	mPosition = mTransform->GetPositionFloat3();

	mDrag = { 0.0f, 0.0f, 0.0f };
	mFriction = { 0.0f, 0.0f, 0.0f };
	mNetForce = { 0.0f, 0.0f, 0.0f };
	mVelocity = { 0.0f, 0.0f, 0.0f };
	mAcceleration = { 0.0f, 0.0f, 0.0f };
}

void PhysicsModel::Update( const float dt )
{
	if ( !mActivated )
	{
		Weight();
		Acceleration();
		Friction( dt );
		Velocity( dt );
		Drag();
		ComputePosition( dt );
		CheckFloorCollisions();
	}

	mNetForce = { 0.0f, 0.0f, 0.0f };
}

void PhysicsModel::Weight()
{
	mWeight = mMass * mGravity;
	mNetForce.y -= mWeight;
}

void PhysicsModel::Acceleration()
{
	mAcceleration.x = mNetForce.x / mMass;
	mAcceleration.y = mNetForce.y / mMass;
	mAcceleration.z = mNetForce.z / mMass;
}

void PhysicsModel::Velocity( const float dt )
{
	mVelocity.x += mAcceleration.x * dt;
	mVelocity.y += mAcceleration.y * dt;
	mVelocity.z += mAcceleration.z * dt;

	// x-axis friction
	if ( mVelocity.x > 0.0f ) mVelocity.x -= mFrictionFactor;
	else if ( mVelocity.x < 0.0f ) mVelocity.x += mFrictionFactor;

	// y-axis friction
	if ( mVelocity.y > 0.0f ) mVelocity.y -= mFrictionFactor;
	else if ( mVelocity.y < 0.0f ) mVelocity.y += mFrictionFactor;

	// z-axis friction
	if ( mVelocity.z > 0.0f ) mVelocity.z -= mFrictionFactor;
	else if ( mVelocity.z < 0.0f ) mVelocity.z += mFrictionFactor;
}

void PhysicsModel::Friction( const float dt )
{
	// f = u * N
	XMFLOAT3 invVelocity = { -mVelocity.x, -mVelocity.y, -mVelocity.z };
	if ( Magnitude( mVelocity ) < mFrictionFactor * dt )
	{
		mFriction.x = invVelocity.x / dt;
		mFriction.y = invVelocity.y / dt;
		mFriction.z = invVelocity.z / dt;
	}
	else
	{
		mFriction.x = Normalization( invVelocity ).x * mFrictionFactor;
		mFriction.y = Normalization( invVelocity ).y * mFrictionFactor;
		mFriction.z = Normalization( invVelocity ).z * mFrictionFactor;
	}
}

void PhysicsModel::Drag()
{
	mUseLaminar ? LaminarDrag() : TurbulentDrag();

	// add to velocity and adjust for negative values
	if ( mNetForce.x < 0.0f ) mVelocity.x *= 1.0f + mNetForce.x;
	else mVelocity.x *= 1.0f - mNetForce.x;

	if ( mNetForce.z < 0.0f ) mVelocity.z *= 1.0f + mNetForce.z;
	else mVelocity.z *= 1.0f - mNetForce.z;
}

void PhysicsModel::LaminarDrag()
{
	mDrag.x = mDragFactor * mVelocity.x;
	mDrag.y = mDragFactor * mVelocity.y;
	mDrag.z = mDragFactor * mVelocity.z;
}

void PhysicsModel::TurbulentDrag()
{
	float magVelocity = Magnitude( mVelocity );
	XMFLOAT3 unitVelocity = Normalization( mVelocity );

	float dragMagnitude = mDragFactor * magVelocity * magVelocity;

	mDrag.x = -dragMagnitude * mDragFactor * unitVelocity.x;
	mDrag.y = -dragMagnitude * mDragFactor * unitVelocity.y;
	mDrag.z = -dragMagnitude * mDragFactor * unitVelocity.z;
}

void PhysicsModel::ComputePosition( const float dt )
{
	mPosition = mTransform->GetPositionFloat3();
	mPosition.x += mVelocity.x * dt + 0.5f * mAcceleration.x * dt * dt;
	mPosition.y += mVelocity.y * dt + 0.5f * mAcceleration.y * dt * dt;
	mPosition.z += mVelocity.z * dt + 0.5f * mAcceleration.z * dt * dt;

	mVelocity.x += mAcceleration.x * dt;
	mVelocity.y += mAcceleration.y * dt;
	mVelocity.z += mAcceleration.z * dt;
	mTransform->SetPosition( mPosition );
}

void PhysicsModel::CheckFloorCollisions()
{
	mPosition = mTransform->GetPositionFloat3();

	if ( mPosition.y < 0.5f )
	{
		mVelocity = { mVelocity.x, 0.0f, mVelocity.z };
		mPosition.y = 0.5f;
		mTransform->SetPosition( mPosition );
	}
}

void PhysicsModel::AddForce( XMFLOAT3 force ) noexcept
{
	mNetForce.x += force.x;
	mNetForce.y += force.y;
	mNetForce.z += force.z;
}

void PhysicsModel::ResetForces() noexcept
{
	mNetForce = { 0.0f, 0.0f, 0.0f };
	mVelocity = { 0.0f, 0.0f, 0.0f };
	mAcceleration = { 0.0f, 0.0f, 0.0f };
}

float PhysicsModel::Magnitude( XMFLOAT3 vec ) const noexcept
{
	return ( sqrtf(
		powf( vec.x, 2 ) +
		powf( vec.y, 2 ) +
		powf( vec.z, 2 ) )
	);
}

XMFLOAT3 PhysicsModel::Normalization( XMFLOAT3 vec ) const noexcept
{
	XMFLOAT3 temp = vec;
	float unitVector = sqrt( ( temp.x * temp.x ) + ( temp.y * temp.y ) + ( temp.z * temp.z ) );

	temp.x = temp.x / unitVector;
	temp.y = temp.y / unitVector;
	temp.z = temp.z / unitVector;

	return temp;
}