#include "Collisions.h"
#include "Camera.h"
#include "Cube.h"

bool Collisions::CheckCollisionCircle( std::unique_ptr<Camera>& camera, GameObject3D& object, float radius ) noexcept
{
	if (( camera->GetPositionFloat3().x - object.GetPositionFloat3().x ) *
		( camera->GetPositionFloat3().x - object.GetPositionFloat3().x ) +
		( camera->GetPositionFloat3().z - object.GetPositionFloat3().z ) *
		( camera->GetPositionFloat3().z - object.GetPositionFloat3().z ) <= radius * radius )
		return true;
	else
		return false;
}

bool Collisions::CheckCollisionSphere( std::unique_ptr<Camera>& camera, GameObject3D& object, float radius ) noexcept
{
	if (( camera->GetPositionFloat3().x - object.GetPositionFloat3().x ) *
		( camera->GetPositionFloat3().x - object.GetPositionFloat3().x ) +
		( camera->GetPositionFloat3().y - object.GetPositionFloat3().y ) *
		( camera->GetPositionFloat3().y - object.GetPositionFloat3().y ) +
		( camera->GetPositionFloat3().z - object.GetPositionFloat3().z ) *
		( camera->GetPositionFloat3().z - object.GetPositionFloat3().z ) <= radius * radius )
		return true;
	else
		return false;
}