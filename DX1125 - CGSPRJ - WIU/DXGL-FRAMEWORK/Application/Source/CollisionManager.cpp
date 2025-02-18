#include "CollisionManager.h"
#include "Time.h"

CollisionManager* CollisionManager::m_instance = nullptr;
CollisionManager::CollisionManager(void)
	: m_world{ nullptr }, m_dispatcher{ nullptr }, m_collisionConfig{ nullptr }, 
	m_broadphase{ nullptr }, m_solver{ nullptr }
{
}

CollisionManager::~CollisionManager(void)
{
	if (m_world)
	{
		delete m_dispatcher;
		delete m_collisionConfig;
		delete m_solver;
		delete m_broadphase;
		delete m_world;
	}
}

CollisionManager* CollisionManager::GetInstance(void)
{
	if (!m_instance) m_instance = new CollisionManager();
	return m_instance;
}

void CollisionManager::DestroyInstance(void) { if (m_instance) delete m_instance; }

void CollisionManager::SetUpDynamicWorld(const float& gravity)
{
	m_collisionConfig = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
	m_broadphase = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver();
	m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfig);
	m_world->setGravity(btVector3(0.0f, -gravity, 0.0f));
}

void CollisionManager::UpdateCollision(void)
{
	if (!m_world) return;
	m_world->stepSimulation(btScalar(Time::deltaTime), 1, btScalar(Time::fixedDeltaTime));
}

btDynamicsWorld* CollisionManager::GetDynamicsWorld(void) { return m_world; }
