#pragma once
#include <UnorderedVector.h>
#include "BulletCollision/CollisionShapes/btBoxShape.h"

class BoxCollider : public btBoxShape
{
private:
	float m_width, m_height, m_depth;

public:

};

class SphereCollider : public btCollisionShape
{

};

class ColliderManager
{
private:
	UnorderedVector<btCollisionShape*> colliders;
	static ColliderManager* m_instance;

	ColliderManager(void);

public:
	~ColliderManager(void) = default;

	static ColliderManager* GetInstance(void);
	static void DestroyInstance(void);
	
	void AddCollider(btCollisionShape* collider2Add);
	void RemoveCollider(btCollisionShape* collider2Remove);
};

