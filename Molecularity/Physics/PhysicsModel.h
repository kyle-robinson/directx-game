#pragma once
#ifndef PHYSICSMODEL_H
#define PHYSICSMODEL_H

#include "GameObject.h"
class CubeProperties;

/// <summary>
/// Managers the generic physics calculations for a given object.
/// </summary>
class PhysicsModel
{
public:
	PhysicsModel( GameObject* transform );
	virtual ~PhysicsModel() = default;

	// Pressure Plate Activation
	bool GetActivated() const noexcept { return mActivated; }
	void SetActivated( bool activated ) noexcept { mActivated = activated; }

	// Update Forces
	virtual void Update( const float dt, std::shared_ptr<CubeProperties>& properties, bool isHeld = false );
	void AddForce( float x, float y, float z ) noexcept;
	void AddForce( XMFLOAT3 force ) noexcept;
	void AddForce( XMVECTOR force ) noexcept;
	void ResetForces() noexcept;
	void ResetForces_NoY() noexcept;
	void InvVelocity( bool invert ) noexcept { mInvVelocity = invert; }

	// Get Forces
	float GetMass() const noexcept { return mMass; }
	XMFLOAT3 GetNetForce() const noexcept { return mNetForce; }
	XMFLOAT3 GetVelocity() const noexcept { return mVelocity; }
	XMFLOAT3 GetAcceleration() const noexcept { return mAcceleration; }
	float GetBounciness() const noexcept { return mBounciness; }

	// Set Forces
	void SetMass( float mass ) noexcept { mMass = mass; }
	void SetNetForce( XMFLOAT3 netForce ) noexcept { mNetForce = netForce; }
	void SetVelocity( XMFLOAT3 velocity ) noexcept { mVelocity = velocity; }
	void SetAcceleration( XMFLOAT3 acceleration ) noexcept { mAcceleration = acceleration; }
	void SetBounciness( float bounciness ) noexcept { mBounciness = bounciness; }

	float Magnitude( XMFLOAT3 vec ) const noexcept;
	XMFLOAT3 Normalization( XMFLOAT3 vec ) const noexcept;

	void CheckGroundCollisions( bool collisions, bool friction = true ) noexcept { mCheckGroundCollisions = collisions; mDoFriction = friction; }
	void UseWeight( bool use )noexcept { useWeight = use; }
private:
	// Update Forces
	void Weight();
	void Acceleration();
	void Velocity( const float dt );
	void Friction( const float dt );
	void Drag();
	void LaminarDrag();
	void TurbulentDrag();
	void ComputePosition( const float dt );
	void CheckFloorCollisions( std::shared_ptr<CubeProperties>& properties );

	// Constants
	static constexpr float mGravity = 9.81f;
	static constexpr float mDragFactor = 2.0f;
	static constexpr float mFrictionFactor = 0.1f;

	// Local Variables
	float mMass;
	float mWeight;
	bool mUseLaminar;
	bool mIsDissCube;
	float mBounciness;

	bool mIsHeld;
	bool mActivated;
	bool mInvVelocity;
	bool mCheckGroundCollisions;
	bool mDoFriction;

	XMFLOAT3 mFriction;
	XMFLOAT3 mPosition;
	XMFLOAT3 mNetForce;
	XMFLOAT3 mVelocity;
	XMFLOAT3 mAcceleration;
	GameObject* mTransform;

	bool useWeight = true;


	float deltaTime = 0.0f;
	DWORD dTimeStart = 0;
};

#endif