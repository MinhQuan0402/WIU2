#pragma once
#include <btBulletDynamicsCommon.h>

class CollisionManager
{
private:
	static CollisionManager* m_instance;
	btDynamicsWorld* m_world;
	btDispatcher* m_dispatcher;
	btCollisionConfiguration* m_collisionConfig;
	btBroadphaseInterface* m_broadphase;
	btConstraintSolver* m_solver;

	CollisionManager(void);

public:
	~CollisionManager(void);
	static CollisionManager* GetInstance(void);
	static void DestroyInstance(void);

	void SetUpDynamicWorld(const float& gravity = 0.0f);
	void UpdateCollision(float fixedDeltaTime);

	btDynamicsWorld* GetDynamicsWorld(void);
};

