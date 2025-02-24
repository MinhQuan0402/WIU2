#include <btBulletDynamicsCommon.h>
#include <UnorderedVector.h>
#include <glm/vec3.hpp>

class GameObject;

struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback {
    bool collisionDetected;
	//please let me hit mq happy bd :) u old fuck
	MyContactResultCallback();

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
};

struct PhysicsMaterial
{
	float m_mass;
	float m_bounciness;
	float m_friction;

	PhysicsMaterial(void);
};

btRigidBody* addSphereCollider(GameObject* go, const float& rad, PhysicsMaterial& mat, const glm::vec3& offset = glm::vec3{});
btRigidBody* addBoxCollider(GameObject* go, const float& width, const float& height, const float& depth, PhysicsMaterial& mat, const glm::vec3& offset = glm::vec3{});
btRigidBody* addCylinderCollider(GameObject* go, const float& height, const float& rad, PhysicsMaterial& mat, const glm::vec3& offset = glm::vec3{});
btRigidBody* addTorusCollider(GameObject* go, const float& rad, const float& tubeRad, const float& numSegment, PhysicsMaterial& mat, const glm::vec3& offset = glm::vec3{});
btRigidBody* addStaticPlane(GameObject* go, const glm::vec3 normal, PhysicsMaterial& mat, const glm::vec3& offset = glm::vec3{});
btRigidBody* addCapsuleCollider(GameObject* go, const float& rad, const float& height, PhysicsMaterial& mat, const glm::vec3& offset = glm::vec3{});
bool CheckCollisionWith(btCollisionObject* obj1, btCollisionObject* obj2);

