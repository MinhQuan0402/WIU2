#include <btBulletDynamicsCommon.h>
#include <UnorderedVector.h>
class GameObject;

struct PhysicsMaterial
{
	float m_mass;
	float m_bounciness;
	float m_friction;

	PhysicsMaterial(void);
};

btRigidBody* addSphereCollider(GameObject* go, const float& rad, PhysicsMaterial& mat);
btRigidBody* addBoxCollider(GameObject* go, const float& width, const float& height, const float& depth, PhysicsMaterial& mat);
btRigidBody* addCylinderCollider(GameObject* go, const float& height, const float& rad, PhysicsMaterial& mat);
btRigidBody* addStaticPlane(GameObject* go, const glm::vec3 normal, PhysicsMaterial& mat);

