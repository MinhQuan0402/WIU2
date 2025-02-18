#include "GameObject.h"
#include "Time.h"
#include "KeyboardController.h"
#include "Scene.h"
#include "CollisionManager.h"

GameObject::GameObject(void) 
	: m_parent{ nullptr }, m_transform{}, m_name{}, 
	m_tag{}, isActive{ false }, rb{ nullptr }, rbGOType{ DYNAMIC }
{
}

GameObject::GameObject(const std::string& name)
	: m_parent{ nullptr }, m_transform{}, m_name{ name },
	m_tag{}, isActive{ false }, rb{ nullptr }, rbGOType{ DYNAMIC }
{
}

GameObject::~GameObject(void)
{
	if (rb)
	{
		CollisionManager::GetInstance()->GetDynamicsWorld()->removeRigidBody(rb);
		btMotionState* motion = rb->getMotionState();
		btCollisionShape* shape = rb->getCollisionShape();
		delete rb;
		delete shape;
		delete motion;
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

	transform.Translate(rawInput * speed);

	if (KeyboardController::GetInstance()->IsKeyPressed('P'))
		std::cout << transform << std::endl;
}
