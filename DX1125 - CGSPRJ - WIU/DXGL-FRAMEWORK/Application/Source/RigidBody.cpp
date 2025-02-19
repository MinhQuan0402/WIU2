#include "RigidBody.h"
#include "CollisionManager.h"
#include "GameObject.h"
#include "ColliderManager.h"

PhysicsMaterial::PhysicsMaterial(void)
	: m_mass{ 1.0f }, m_bounciness{ 0.0f }, m_friction{ 0.0f }
{
}

void SetObjectDynamic(btRigidBody* body, float mass) {
	btVector3 inertia(0, 0, 0);
	body->getCollisionShape()->calculateLocalInertia(mass, inertia); // Recalculate inertia
	body->setMassProps(mass, inertia);
	body->updateInertiaTensor();
}

void SetObjectStatic(btRigidBody* body) {
	body->setMassProps(0, btVector3(0, 0, 0)); // Set mass to 0 (static)
	body->updateInertiaTensor();
}

btRigidBody* addSphereCollider(GameObject* go, const float& rad, PhysicsMaterial& mat, const glm::vec3& offset)
{
	if (!go) return nullptr;
	go->colliderOffset = offset;
	btTransform t;
	t.setIdentity();
	btVector3 finalPosition = btVector3(go->m_transform.m_position.x + offset.x, go->m_transform.m_position.y + offset.y, go->m_transform.m_position.z + offset.z);
	t.setOrigin(btVector3(finalPosition));
	btQuaternion rotationX = btQuaternion(btVector3(1, 0, 0), glm::radians(go->m_transform.m_rotation.x));
	btQuaternion rotationY = btQuaternion(btVector3(0, 1, 0), glm::radians(go->m_transform.m_rotation.y));
	btQuaternion rotationZ = btQuaternion(btVector3(0, 0, 1), glm::radians(go->m_transform.m_rotation.z));
	btQuaternion finalRotation = rotationX * rotationY * rotationZ;
	t.setRotation(finalRotation);
	btSphereShape* sphere = new SphereCollider(go, rad);
	ColliderManager::GetInstance()->AddCollider(sphere);
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
	rb->setUserPointer(go);
	return rb;
}

btRigidBody* addBoxCollider(GameObject* go, const float& width, const float& height, const float& depth, PhysicsMaterial& mat, const glm::vec3& offset)
{
	if (!go) return nullptr;
	go->colliderOffset = offset;
	btTransform t;
	t.setIdentity();
	btVector3 finalPosition = btVector3(go->m_transform.m_position.x + offset.x, go->m_transform.m_position.y + offset.y, go->m_transform.m_position.z + offset.z);
	t.setOrigin(btVector3(finalPosition));
	btQuaternion rotationX = btQuaternion(btVector3(1, 0, 0), glm::radians(go->m_transform.m_rotation.x));
	btQuaternion rotationY = btQuaternion(btVector3(0, 1, 0), glm::radians(go->m_transform.m_rotation.y));
	btQuaternion rotationZ = btQuaternion(btVector3(0, 0, 1), glm::radians(go->m_transform.m_rotation.z));
	btQuaternion finalRotation = rotationX * rotationY * rotationZ;
	t.setRotation(finalRotation);
	btBoxShape* box = new BoxCollider(go, width, height, depth);
	ColliderManager::GetInstance()->AddCollider(box);
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
	rb->setUserPointer(go);
	return rb;
}

btRigidBody* addCylinderCollider(GameObject* go, const float& height, const float& rad, PhysicsMaterial& mat, const glm::vec3& offset)
{
	if (!go) return nullptr;
	go->colliderOffset = offset;
	btTransform t;
	t.setIdentity();
	btVector3 finalPosition = btVector3(go->m_transform.m_position.x + offset.x, go->m_transform.m_position.y + offset.y, go->m_transform.m_position.z + offset.z);
	t.setOrigin(btVector3(finalPosition));
	btQuaternion rotationX = btQuaternion(btVector3(1, 0, 0), glm::radians(go->m_transform.m_rotation.x));
	btQuaternion rotationY = btQuaternion(btVector3(0, 1, 0), glm::radians(go->m_transform.m_rotation.y));
	btQuaternion rotationZ = btQuaternion(btVector3(0, 0, 1), glm::radians(go->m_transform.m_rotation.z));
	btQuaternion finalRotation = rotationX * rotationY * rotationZ;
	t.setRotation(finalRotation);
	btCylinderShape* cylinder = new CylinderCollider(go, height, rad);
	ColliderManager::GetInstance()->AddCollider(cylinder);
	btVector3 inertia(0.f, 0.f, 0.f);
	if (mat.m_mass != 0.0f) cylinder->calculateLocalInertia(mat.m_mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mat.m_mass, motion, cylinder, inertia);

	mat.m_bounciness = glm::clamp(mat.m_bounciness, 0.0f, 1.0f);
	mat.m_friction = glm::clamp(mat.m_friction, 0.0f, 1.0f);
	info.m_restitution = mat.m_bounciness;
	info.m_friction = mat.m_friction;

	btRigidBody* rb = new btRigidBody(info);
	CollisionManager::GetInstance()->GetDynamicsWorld()->addRigidBody(rb);
	rb->setUserPointer(go);
	return rb;
}

btRigidBody* addStaticPlane(GameObject* go, glm::vec3 normal, PhysicsMaterial& mat, const glm::vec3& offset)
{
	mat.m_mass = 0.0f;
	go->colliderOffset = offset;
	btTransform t;
	t.setIdentity();
	btVector3 finalPosition = btVector3(go->m_transform.m_position.x + offset.x, go->m_transform.m_position.y + offset.y, go->m_transform.m_position.z + offset.z);
	t.setOrigin(btVector3(finalPosition));
	btQuaternion rotationX = btQuaternion(btVector3(1, 0, 0), glm::radians(go->m_transform.m_rotation.x));
	btQuaternion rotationY = btQuaternion(btVector3(0, 1, 0), glm::radians(go->m_transform.m_rotation.y));
	btQuaternion rotationZ = btQuaternion(btVector3(0, 0, 1), glm::radians(go->m_transform.m_rotation.z));
	btQuaternion finalRotation = rotationX * rotationY * rotationZ;
	t.setRotation(finalRotation);
	btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), 0);
	plane->setUserPointer(go);
	ColliderManager::GetInstance()->AddCollider(plane);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mat.m_mass, motion, plane);

	mat.m_bounciness = glm::clamp(mat.m_bounciness, 0.0f, 1.0f);
	mat.m_friction = glm::clamp(mat.m_friction, 0.0f, 1.0f);
	info.m_restitution = mat.m_bounciness;
	info.m_friction = mat.m_friction;

	btRigidBody* rb = new btRigidBody(info);
	CollisionManager::GetInstance()->GetDynamicsWorld()->addRigidBody(rb);
	rb->setUserPointer(go);
	return rb;
}
