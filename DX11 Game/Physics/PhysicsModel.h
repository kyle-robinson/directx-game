#pragma once
#ifndef PHYSICSMODEL_H
#define PHYSICSMODEL_H

#include "GameObject.h"

/// <summary>
/// Managers the generic physics calculations for a given object.
/// </summary>
class PhysicsModel
{
public:
	PhysicsModel( GameObject* transform );
	virtual ~PhysicsModel() = default;

	// pressure plate activation
	bool GetActivated() const noexcept { return mActivated; }
	void SetActivated( bool activated ) noexcept { mActivated = activated; }

	// Update Forces
	virtual void Update( const float dt );
	void AddForce( XMFLOAT3 force ) noexcept;
	void ResetForces() noexcept;

	// Get Forces
	float GetMass() const noexcept { return mMass; }
	XMFLOAT3 GetNetForce() const noexcept { return mNetForce; }
	XMFLOAT3 GetVelocity() const noexcept { return mVelocity; }
	XMFLOAT3 GetAcceleration() const noexcept { return mAcceleration; }

	// Set Forces
	void SetMass( float mass ) noexcept { mMass = mass; }
	void SetNetForce( XMFLOAT3 netForce ) noexcept { mNetForce = netForce; }
	void SetVelocity( XMFLOAT3 velocity ) noexcept { mVelocity = velocity; }
	void SetAcceleration( XMFLOAT3 acceleration ) noexcept { mAcceleration = acceleration; }

	float Magnitude( XMFLOAT3 vec ) const noexcept;
	XMFLOAT3 Normalization( XMFLOAT3 vec ) const noexcept;
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
	void CheckFloorCollisions();

	// Constants
	static constexpr float mGravity = 9.81f;
	static constexpr float mDragFactor = 0.75f;
	static constexpr float mFrictionFactor = 0.0002f;

	// Local Variables
	float mMass;
	float mWeight;
	bool mActivated;
	bool mUseLaminar;
	XMFLOAT3 mDrag;
	XMFLOAT3 mFriction;
	XMFLOAT3 mPosition;
	XMFLOAT3 mNetForce;
	XMFLOAT3 mVelocity;
	XMFLOAT3 mAcceleration;
	GameObject* mTransform;
};

#endif