#include "RigidBody.h"
#include "CollisionManager.h"
#include "GameObject.h"

PhysicsMaterial::PhysicsMaterial(void)
	: m_mass{ 1.0f }, m_bounciness{ 0.0f }, m_friction{ 0.0f }
{
}

btRigidBody* addSphereCollider(GameObject*& go, const float& rad, PhysicsMaterial& mat)
{
	if (!go) return nullptr;

	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(go->m_transform.m_position.x, go->m_transform.m_position.y, go->m_transform.m_position.z));
	btSphereShape* sphere = new btSphereShape(rad);
	btVector3 inertia(0.f, 0.f, 0.f);
	if (mat.m_mass != 0.0f) sphere->calculateLocalInertia(mat.m_mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mat.m_mass, motion, sphere, inertia);

	mat.m_bounciness = glm::clamp(mat.m_bounciness, 0.0f, 1.0f);
	mat.m_friction = glm::clamp(mat.m_friction, 0.0f, 1.0f);
	info.m_restitution = mat.m_bounciness;
	info.m_friction = mat.m_friction;

	btRigidBody* rb = new btRigidBody(info);
	CollisionManager::GetInstance()->GetDynamicsWorld()->addRigidBody(rb);
	bodies.push_back(rb);
	return rb;
}

btRigidBody* addBoxCollider(GameObject*& go, const float& width, const float& height, const float& depth, PhysicsMaterial& mat)
{
	if (!go) return nullptr;

	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(go->m_transform.m_position.x, go->m_transform.m_position.y, go->m_transform.m_position.z));
	btBoxShape* box = new btBoxShape(btVector3(width / 2.0f, height / 2.0f, depth / 2.0f));
	btVector3 inertia(0.f, 0.f, 0.f);
	if (mat.m_mass != 0.0f) box->calculateLocalInertia(mat.m_mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mat.m_mass, motion, box, inertia);

	mat.m_bounciness = glm::clamp(mat.m_bounciness, 0.0f, 1.0f);
	mat.m_friction = glm::clamp(mat.m_friction, 0.0f, 1.0f);
	info.m_restitution = mat.m_bounciness;
	info.m_friction = mat.m_friction;

	btRigidBody* rb = new btRigidBody(info);
	CollisionManager::GetInstance()->GetDynamicsWorld()->addRigidBody(rb);
	bodies.push_back(rb);
	return rb;
}
