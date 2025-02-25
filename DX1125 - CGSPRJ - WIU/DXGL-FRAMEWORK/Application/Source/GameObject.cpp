#define RAD_TO_DEG 57.2958f
#include "GameObject.h"
#include "Time.h"
#include "KeyboardController.h"
#include "Scene.h"
#include "CollisionManager.h"
#include "Utility.h"

GameObject::GameObject(void) 
	: m_parent{ nullptr }, m_transform{}, m_name{}, 
	m_tag{}, isActive{ false }, rb{ nullptr }, rbGOType{ DYNAMIC }, colliderOffset{}
{
}

GameObject::GameObject(const std::string& name)
	: m_parent{ nullptr }, m_transform{}, m_name{ name },
	m_tag{}, isActive{ false }, rb{ nullptr }, rbGOType{ DYNAMIC }, colliderOffset{}
{
}

GameObject::~GameObject(void)
{
	if (rb)
	{
		CollisionManager::GetInstance()->GetDynamicsWorld()->removeRigidBody(rb);
		btMotionState* motion = rb->getMotionState();
		delete motion;
		delete rb;
	}
}

void GameObject::Initialize(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	m_transform.Translate(pos);
	m_transform.Rotate(rot);
	m_transform.ScaleBy(scale);
}

void GameObject::SetParent(GameObject* parent)
{
	m_transform.SetParent(&parent->m_transform);
	m_parent = parent;

	if (parent)
	{
		m_tag = parent->m_tag;
		parent->AddChild(this);
	}
}

GameObject* GameObject::GetParent(void) { return m_parent; }

GameObject* GameObject::FindChild(GameObject* go2Find)
{
	std::vector<GameObject*>::iterator objectFound = std::find(m_children.begin(), m_children.end(), go2Find);
	if (objectFound != m_children.end()) return	*objectFound;
	return nullptr;
}

void GameObject::RemoveChild(GameObject* child2Remove) 
{ 
	m_children.remove(child2Remove); 
	if (child2Remove) child2Remove->SetParent(nullptr);
}

void GameObject::SetRigidbodyPosition(const glm::vec3& newPos)
{
	btTransform t;
	t.setIdentity();
	rb->getMotionState()->getWorldTransform(t);
	t.setOrigin(btVector3(newPos.x, newPos.y, newPos.z));
	rb->setWorldTransform(t);
}

void GameObject::SetRigidbodyPosition(const float& x, const float& y, const float& z)
{
	btTransform t;
	t.setIdentity();
	rb->getMotionState()->getWorldTransform(t);
	t.setOrigin(btVector3(x, y, z));
	rb->setWorldTransform(t);
}

glm::vec3 GameObject::GetRigidbodyPosition(void) const
{
	btTransform t;
	t.setIdentity();
	rb->getMotionState()->getWorldTransform(t);
	return glm::vec3(t.getOrigin().x() - colliderOffset.x, t.getOrigin().y() - colliderOffset.y, t.getOrigin().z() - colliderOffset.z);
}

void GameObject::SetRigidbodyRotation(const glm::vec3& newRotation)
{
	btTransform t;
	t.setIdentity();
	rb->getMotionState()->getWorldTransform(t);
	btQuaternion rotationX = btQuaternion(btVector3(1, 0, 0), glm::radians(newRotation.x));
	btQuaternion rotationY = btQuaternion(btVector3(0, 1, 0), glm::radians(newRotation.y));
	btQuaternion rotationZ = btQuaternion(btVector3(0, 0, 1), glm::radians(newRotation.z));
	btQuaternion finalRotation = rotationX * rotationY * rotationZ;
	t.setRotation(finalRotation);
	rb->getMotionState()->setWorldTransform(t);
}

void GameObject::SetRigidbodyRotation(const float& x, const float& y, const float& z)
{
	btTransform t;
	t.setIdentity();
	rb->getMotionState()->getWorldTransform(t);
	btQuaternion rotationX = btQuaternion(btVector3(1, 0, 0), glm::radians(x));
	btQuaternion rotationY = btQuaternion(btVector3(0, 1, 0), glm::radians(y));
	btQuaternion rotationZ = btQuaternion(btVector3(0, 0, 1), glm::radians(z));
	btQuaternion finalRotation = rotationX * rotationY * rotationZ;
	t.setRotation(finalRotation);
	rb->getMotionState()->setWorldTransform(t);
}

glm::vec3 GameObject::GetRigidbodyRotation(void) const
{
	btTransform t;
	t.setIdentity();
	rb->getMotionState()->getWorldTransform(t);
	btVector3 euler = QuaternionToEuler(t.getRotation()) * RAD_TO_DEG;
	return glm::vec3(euler.x(), euler.y(), euler.z());
}

btCollisionObject* GameObject::getObject(void) const
{
	if (rb)
	{
		btCollisionObject* obj = rb;
		return obj;
	}

	return nullptr;
}

void GameObject::AddChild(GameObject* child) 
{
	child->SetParent(this);
	m_children.push_back(child); 
}

UnorderedVector<GameObject*> GameObject::GetChildren(void) { return m_children; }

bool GameObject::operator==(const GameObject& rhs) { return this == &rhs; }

void GameObject::MoveObj(Transform& transform)
{
	float speed = 10.0f * Time::deltaTime;
	glm::vec3 rawInput = glm::vec3(0.f, 0.f, 0.f);
	if (KeyboardController::GetInstance()->IsKeyDown('I'))
		rawInput.z = 1;
	if (KeyboardController::GetInstance()->IsKeyDown('K'))
		rawInput.z = -1;
	if (KeyboardController::GetInstance()->IsKeyDown('J'))
		rawInput.x = 1;
	if (KeyboardController::GetInstance()->IsKeyDown('L'))
		rawInput.x = -1;
	if (KeyboardController::GetInstance()->IsKeyDown('U'))
		rawInput.y = -1;
	if (KeyboardController::GetInstance()->IsKeyDown('O'))
		rawInput.y = 1;

	transform.Rotate(rawInput * speed);

	if (KeyboardController::GetInstance()->IsKeyPressed('P'))
		std::cout << transform << std::endl;
}
