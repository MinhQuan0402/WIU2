#include "TossBoard.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneRingToss.h"
#include "ColliderManager.h"
#include "CollisionManager.h"
#include "btBulletDynamicsCommon.h"
#include "Utility.h"
#include "MeshBuilder.h"

btCompoundShape* Bottle::GenerateCollider(void)
{
	btCompoundShape* compoundShape = new btCompoundShape();
	compoundShape->setUserPointer(this);

	btTransform baseTransform;
	baseTransform.setIdentity();
	btCylinderShape* baseCollider = new CylinderCollider(this, 2.55f, 1.05f);
	compoundShape->addChildShape(baseTransform, baseCollider);

	btTransform middleTransform;
	middleTransform.setIdentity();
	middleTransform.setOrigin(btVector3(0.0f, 1.3f, 0.0f));
	btSphereShape* middleCollider = new SphereCollider(this, 0.45f);
	compoundShape->addChildShape(middleTransform, middleCollider);

	btTransform topTransform;
	topTransform.setIdentity();
	topTransform.setOrigin(btVector3(0.0f, 1.f , 0.0f) + middleTransform.getOrigin());
	btCylinderShape* topCollider = new CylinderCollider(this, 1.2f, 0.41f);
	compoundShape->addChildShape(topTransform, topCollider);

	return compoundShape;
}

void Bottle::GenerateRigidbody(void)
{
	PhysicsMaterial mat;
	mat.m_mass = 5.0f;
	btTransform t;
	t.setIdentity();
	btVector3 finalPosition = btVector3(m_transform.m_position.x + colliderOffset.x, m_transform.m_position.y + colliderOffset.y, m_transform.m_position.z + colliderOffset.z);
	t.setOrigin(btVector3(finalPosition));
	t.setRotation(SetRotation(m_transform.m_rotation));
	btCompoundShape* compoundCollider = GenerateCollider();
	ColliderManager::GetInstance()->AddCollider(compoundCollider);
	btVector3 inertia(0.f, 0.f, 0.f);
	if (mat.m_mass != 0.0f) compoundCollider->calculateLocalInertia(mat.m_mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mat.m_mass, motion, compoundCollider, inertia);

	mat.m_bounciness = glm::clamp(mat.m_bounciness, 0.0f, 1.0f);
	mat.m_friction = glm::clamp(mat.m_friction, 0.0f, 1.0f);
	info.m_restitution = mat.m_bounciness;
	info.m_friction = mat.m_friction;

	rb = new btRigidBody(info);
	CollisionManager::GetInstance()->GetDynamicsWorld()->addRigidBody(rb);
	rb->setUserPointer(this);
}

Bottle::Bottle(glm::vec3 color)
	: m_color{ color }
{
	m_transform.ScaleBy(0.1f, 0.1f, 0.1f);
	rbGOType = DYNAMIC;
	GameObjectManager::addItem(this);
}

void Bottle::Start(void)
{
	colliderOffset = glm::vec3{ 0.0f, 1.275f, 0.0f };
	GenerateRigidbody();
}

void Bottle::Update(void)
{
}

void Bottle::LateUpdate(void)
{
}

void Bottle::Render(Scene& scene)
{
	scene.RenderRigidMesh(scene.meshList[SceneRingToss::GEO_BOTTLE], false, m_transform, rb);
}

btCompoundShape* TossBoard::GenerateCollider(void)
{
	btVector3 offset = btVector3(0.0f, 1.8f, 0.0f);
	btCompoundShape* compoundShape = new btCompoundShape();
	compoundShape->setUserPointer(this);
	
	btTransform box1Transform;
	box1Transform.setIdentity();
	box1Transform.setOrigin(offset);
	box1Transform.setRotation(SetRotation(20.0f, 0.0f, 0.0f));
	btCollisionShape* box1Collider = new BoxCollider(this, 5.0f, 1.0f, 10.0f);
	compoundShape->addChildShape(box1Transform, box1Collider);

	btVector3 offset2 = btVector3(2.3f, 1.05f, 0.4f);
	for (unsigned i = 0; i < 4; ++i)
	{
		btTransform localTransform;
		localTransform.setIdentity();
		localTransform.setOrigin(offset + offset2);
		localTransform.setRotation(SetRotation(20.0f, 0.0f, 0.0f));
		btCollisionShape* boxCollider = new BoxCollider(this, 0.25f, 1.5f, 10.0f);
		compoundShape->addChildShape(localTransform, boxCollider);

		offset2.setX(offset2.getX() - 1.525f);
	}
	
	btVector3 offset3 = btVector3(0.f, 2.6f, -4.10f);
	for (unsigned i = 0; i < 3; ++i)
	{
		btTransform localTransform;
		localTransform.setIdentity();
		localTransform.setOrigin(offset + offset3);
		localTransform.setRotation(SetRotation(20.0f, 0.0f, 0.0f));
		btCollisionShape* boxCollider = new BoxCollider(this, 4.8f, 1.5f, 0.35f);
		compoundShape->addChildShape(localTransform, boxCollider);

		offset3.setZ(offset3.getZ() + 1.45f);
		offset3.setY(offset3.getY() - 0.55f);
	}

	btVector3 offset4 = btVector3(0.f, 1.0f, 0.425f);
	btTransform box4Transform;
	box4Transform.setIdentity();
	box4Transform.setOrigin(btVector3(offset + offset4));
	box4Transform.setRotation(SetRotation(20.0f, 0.0f, 0.0f));
	btCollisionShape* box4Collider = new BoxCollider(this, 4.8f, 1.5f, 0.6f);
	compoundShape->addChildShape(box4Transform, box4Collider);

	btVector3 offset5 = btVector3(0.f, 0.325f, 2.05f);
	for (unsigned i = 0; i < 3; ++i)
	{
		btTransform localTransform;
		localTransform.setIdentity();
		localTransform.setOrigin(offset + offset5);
		localTransform.setRotation(SetRotation(20.0f, 0.0f, 0.0f));
		btCollisionShape* boxCollider = new BoxCollider(this, 4.8f, 1.5f, 0.35f);
		compoundShape->addChildShape(localTransform, boxCollider);

		offset5.setZ(offset5.getZ() + 1.45f);
		offset5.setY(offset5.getY() - 0.5f);
	}

	return compoundShape;
}

void TossBoard::GenerateRigidbody(void)
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	btTransform t;
	t.setIdentity();
	btVector3 finalPosition = btVector3(m_transform.m_position.x + colliderOffset.x, m_transform.m_position.y + colliderOffset.y, m_transform.m_position.z + colliderOffset.z);
	t.setOrigin(btVector3(finalPosition));
	t.setRotation(SetRotation(m_transform.m_rotation));
	btCompoundShape* compoundCollider = GenerateCollider();
	ColliderManager::GetInstance()->AddCollider(compoundCollider);
	btVector3 inertia(0.f, 0.f, 0.f);
	if (mat.m_mass != 0.0f) compoundCollider->calculateLocalInertia(mat.m_mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mat.m_mass, motion, compoundCollider, inertia);

	mat.m_bounciness = glm::clamp(mat.m_bounciness, 0.0f, 1.0f);
	mat.m_friction = glm::clamp(mat.m_friction, 0.0f, 1.0f);
	info.m_restitution = mat.m_bounciness;
	info.m_friction = mat.m_friction;

	rb = new btRigidBody(info);
	CollisionManager::GetInstance()->GetDynamicsWorld()->addRigidBody(rb);
	rb->setUserPointer(this);
}

TossBoard::TossBoard(void)
{
	m_transform.ScaleBy(0.1f, 0.1f, 0.1f);
	GameObjectManager::addItem(this);

	glm::vec3 originalPos = glm::vec3(1.55f, 1.25f, 4.8f);
	glm::vec3 localPos = originalPos;
	for (unsigned i = 0; i < 9; ++i)
	{
		if (i != 0 && i % 3 == 0)
		{
			localPos.x = originalPos.x;
			localPos.z -= 1.5f;
			localPos.y += 0.5f;
		}

		bottles.push_back(new Bottle(RED));
		bottles[i]->m_transform.Translate(localPos);
		bottles[i]->m_transform.Rotate(20.0f, 0.0f, 0.0f);

		localPos.x -= 1.55f;
	}

	originalPos = glm::vec3(1.55f, originalPos.y * 2, 0.0f);
	localPos = originalPos;
	for (unsigned i = 0; i < 9; ++i)
	{
		if (i != 0 && i % 3 == 0)
		{
			localPos.x = originalPos.x;
			localPos.z -= 1.5f;
			localPos.y += 0.5f;
		}

		bottles.push_back(new Bottle(RED));
		bottles[bottles.size() - 1]->m_transform.Translate(localPos);
		bottles[bottles.size() - 1]->m_transform.Rotate(20.0f, 0.0f, 0.0f);

		localPos.x -= 1.55f;
	}
}

void TossBoard::Start(void)
{
	GenerateRigidbody();
}

void TossBoard::Update(void)
{
}

void TossBoard::LateUpdate(void)
{
}

void TossBoard::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneRingToss::GEO_BOARD], false, m_transform);
}

btCompoundShape* Ring::GenerateCollider(void)
{
	btCompoundShape* compoundShape = new btCompoundShape();
	compoundShape->setUserPointer(this);

	btTransform torusTransform;
	torusTransform.setIdentity();
	torusTransform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
	torusTransform.setRotation(SetRotation(90.f, 0.0f, 0.0f));
	TorusCollider* torusCollider = new TorusCollider(this, 0.55f, 0.35f, 50);
	compoundShape->addChildShape(torusTransform, torusCollider);

	btTransform tieTransform;
	tieTransform.setIdentity();
	tieTransform.setOrigin(btVector3(0.45f, 0.0f, 0.3f));
	tieTransform.setRotation(SetRotation(90.f, 0.f, 35.0f));
	CylinderCollider* tieCollider = new CylinderCollider(this, 0.45f, 0.3f);
	compoundShape->addChildShape(tieTransform, tieCollider);

	return compoundShape;
}

void Ring::GenerateRigidbody(void)
{
	PhysicsMaterial mat;
	mat.m_mass = 1.0f;
	btTransform t;
	t.setIdentity();
	btVector3 finalPosition = btVector3(m_transform.m_position.x + colliderOffset.x, m_transform.m_position.y + colliderOffset.y, m_transform.m_position.z + colliderOffset.z);
	t.setOrigin(btVector3(finalPosition));
	t.setRotation(SetRotation(m_transform.m_rotation));
	btCompoundShape* compoundCollider = GenerateCollider();
	ColliderManager::GetInstance()->AddCollider(compoundCollider);
	btVector3 inertia(0.f, 0.f, 0.f);
	if (mat.m_mass != 0.0f) compoundCollider->calculateLocalInertia(mat.m_mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mat.m_mass, motion, compoundCollider, inertia);

	mat.m_bounciness = glm::clamp(mat.m_bounciness, 0.0f, 1.0f);
	mat.m_friction = glm::clamp(mat.m_friction, 0.0f, 1.0f);
	info.m_restitution = mat.m_bounciness;
	info.m_friction = mat.m_friction;

	rb = new btRigidBody(info);
	CollisionManager::GetInstance()->GetDynamicsWorld()->addRigidBody(rb);
	rb->setUserPointer(this);
}

Ring::Ring(glm::vec3 color)
	: m_color{ color }
{
	m_transform.ScaleBy(0.1f, 0.1f, 0.1f);
	GameObjectManager::addItem(this);
}

void Ring::Start(void)
{
	GenerateRigidbody();
}

void Ring::Update(void)
{
}

void Ring::LateUpdate(void)
{
}

void Ring::Render(Scene& scene)
{
	scene.RenderRigidMesh(scene.meshList[SceneRingToss::GEO_TIE], false, m_transform, rb);
	scene.RenderRigidMesh(scene.meshList[SceneRingToss::GEO_RING], false, m_transform, rb);
}
