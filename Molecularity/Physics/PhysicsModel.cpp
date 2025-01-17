#include "stdafx.h"
#include "PhysicsModel.h"
#include "CubeProperties.h"

PhysicsModel::PhysicsModel( GameObject* transform ) : mTransform( transform )
{
	mIsHeld = false;
	mActivated = false;
	mUseLaminar = true;
	mInvVelocity = false;
	mCheckGroundCollisions = true;
	mPosition = mTransform->GetPositionFloat3();

	mMass = 25.0f;
	mFriction = { 0.0f, 0.0f, 0.0f };
	mNetForce = { 0.0f, 0.0f, 0.0f };
	mVelocity = { 0.0f, 0.0f, 0.0f };
	mAcceleration = { 0.0f, 0.0f, 0.0f };
}

void PhysicsModel::Update( const float dt, std::shared_ptr<CubeProperties>& properties, bool isHeld )
{
	if ( dTimeStart == 0 )
		dTimeStart = 0;

	DWORD dTimeCur = GetTickCount64();

	if ( dTimeStart == 0 )
		dTimeStart = dTimeCur;

	deltaTime += ( dTimeCur - dTimeStart ) / 1000.0f;

	if ( deltaTime < ( 1.0f / 60.0f ) )
		return;

	mIsHeld = isHeld;

	if ( !mActivated )
	{
		if ( !mIsHeld )
		{
			if ( useWeight )
				Weight();
			Friction( dt );
			Velocity( dt );
		}
		Acceleration();
		Drag();
		ComputePosition( dt );
		if ( mCheckGroundCollisions )
			CheckFloorCollisions( properties );
	}
	else
	{
		mVelocity = { 0.0f, 0.0f, 0.0f };
		mAcceleration = { 0.0f, 0.0f, 0.0f };
	}

	mNetForce = { 0.0f, 0.0f, 0.0f };

	dTimeStart = dTimeCur;
	deltaTime -= ( 1.0f / 60.0f );
}

void PhysicsModel::Weight()
{
	mWeight = mMass * mGravity;
	mNetForce.y -= mWeight * 0.001f;
}

void PhysicsModel::Acceleration()
{
	mAcceleration.x = mNetForce.x / mMass;
	mAcceleration.y = mNetForce.y / mMass;
	mAcceleration.z = mNetForce.z / mMass;
}

void PhysicsModel::Velocity( const float dt )
{
	mVelocity.x += mAcceleration.x;
	mVelocity.y += mAcceleration.y;
	mVelocity.z += mAcceleration.z;

	if ( mCheckGroundCollisions && mDoFriction )
	{
		mVelocity.x += mFrictionFactor * -( mVelocity.x );
		mVelocity.z += mFrictionFactor * -( mVelocity.z );
	}
}

void PhysicsModel::Friction( const float dt )
{
	// f = u * N
	XMFLOAT3 invVelocity = { -mVelocity.x, -mVelocity.y, -mVelocity.z };
	if ( Magnitude( mVelocity ) < mFrictionFactor )
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
}

void PhysicsModel::LaminarDrag()
{
	mNetForce.x -= mDragFactor * mVelocity.x;
	mNetForce.y -= mDragFactor * mVelocity.y;
	mNetForce.z -= mDragFactor * mVelocity.z;
}

void PhysicsModel::TurbulentDrag()
{
	float magVelocity = Magnitude( mVelocity );
	XMFLOAT3 unitVelocity = Normalization( mVelocity );
	float dragMagnitude = mDragFactor * magVelocity * magVelocity;

	mNetForce.x -= -dragMagnitude * mDragFactor * unitVelocity.x;
	mNetForce.y -= -dragMagnitude * mDragFactor * unitVelocity.y;
	mNetForce.z -= -dragMagnitude * mDragFactor * unitVelocity.z;
}

void PhysicsModel::ComputePosition( const float dt )
{
	mPosition = mTransform->GetPositionFloat3();
	mPosition.x += mVelocity.x + 0.5f * mAcceleration.x;
	mPosition.y += mVelocity.y + 0.5f * mAcceleration.y;
	mPosition.z += mVelocity.z + 0.5f * mAcceleration.z;

	mVelocity.x += mAcceleration.x;
	mVelocity.y += mAcceleration.y;
	mVelocity.z += mAcceleration.z;
	mTransform->SetPosition( mPosition );
}

void PhysicsModel::CheckFloorCollisions( std::shared_ptr<CubeProperties>& properties )
{
	mPosition = mTransform->GetPositionFloat3();

	static float offset = 0.5f;
	switch ( properties->GetBoxSize() )
	{
	case BoxSize::Small:  offset = 0.25f; break;
	case BoxSize::Normal: offset = 0.5f;  break;
	case BoxSize::Large:  offset = 1.0f;  break;
	}
	if ( mPosition.y < offset )
	{
		// enable bouncing???
		mVelocity.y = mInvVelocity ? -mVelocity.y * mBounciness : 0.0f;

		mPosition.y = offset;
		mTransform->SetPosition( mPosition );
	}
}

void PhysicsModel::AddForce( float x, float y, float z ) noexcept
{
	AddForce( XMFLOAT3( x, y, z ) );
}

void PhysicsModel::AddForce( XMFLOAT3 force ) noexcept
{
	mNetForce.x += force.x;
	mNetForce.y += force.y;
	mNetForce.z += force.z;
}

void PhysicsModel::AddForce( XMVECTOR force ) noexcept
{
	mNetForce.x += XMVectorGetX( force );
	mNetForce.y += XMVectorGetY( force );
	mNetForce.z += XMVectorGetZ( force );
}

void PhysicsModel::ResetForces() noexcept
{
	mNetForce = { 0.0f, 0.0f, 0.0f };
	mVelocity = { 0.0f, 0.0f, 0.0f };
	mAcceleration = { 0.0f, 0.0f, 0.0f };
}

void PhysicsModel::ResetForces_NoY() noexcept
{
	mNetForce = { 0.0f, mNetForce.y, 0.0f };
	mVelocity = { 0.0f, mVelocity.y, 0.0f };
	mAcceleration = { 0.0f, mAcceleration.y, 0.0f };
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
	float unitVector = sqrt( ( vec.x * vec.x ) + ( vec.y * vec.y ) + ( vec.z * vec.z ) );

	vec.x = vec.x / unitVector;
	vec.y = vec.y / unitVector;
	vec.z = vec.z / unitVector;

	return vec;
}