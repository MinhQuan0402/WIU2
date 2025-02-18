#pragma once
#include <UnorderedVector.h>
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btCylinderShape.h"

class BoxCollider : public btBoxShape
{
private:
	float m_width, m_height, m_depth;

public:
	BoxCollider(float width, float height, float depth);
	~BoxCollider(void) = default;
	
	void GetDimension(float& width, float& height, float& depth) const;
};

class SphereCollider : public btSphereShape
{
private :
	float m_rad;

public:
	SphereCollider(float rad);
	~SphereCollider(void) = default;

	float GetRadius(void) const;
};

class CylinderCollider : public btCylinderShape
{
private:
	float m_rad, m_height;

public:
	CylinderCollider(const float& height, const float& rad);
	~CylinderCollider(void) = default;

	void GetDimension(float& rad, float& height) const;
};

class ColliderManager
{
private:
	static ColliderManager* m_instance;

	ColliderManager(void);

public:
	UnorderedVector<btCollisionShape*> colliders;

	~ColliderManager(void) = default;

	static ColliderManager* GetInstance(void);
	static void DestroyInstance(void);
	
	void AddCollider(btCollisionShape* collider2Add);
	void RemoveCollider(btCollisionShape* collider2Remove);
};

