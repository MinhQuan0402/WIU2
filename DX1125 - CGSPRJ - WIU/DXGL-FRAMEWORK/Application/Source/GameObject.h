#pragma once
#include "Transform.h"
#include <iostream>
#include <UnorderedVector.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

class Scene;

class GameObject
{
private:
	bool isActive;

	GameObject* m_parent; //Store pointer for its parent
	UnorderedVector<GameObject*> m_children; //Store children go

public:
	enum RB_TYPE
	{
		DYNAMIC,
		KINEMATIC,
		STATIC
	};
	
	Transform m_transform; //Transform of this object
	btRigidBody* rb;
	std::string m_name; //Store its name
	std::string m_tag; //Store its tag/categories
	glm::vec3 colliderOffset;

	RB_TYPE rbGOType;

public:
	GameObject(void);
	GameObject(const std::string& name);
	virtual ~GameObject(void);

/******************************************************************************/
	/*!
	\brief
	Initializing the transform of this gameobject

	\param position - three values of position [x, y, z]
	\param rotation - three values of rotation angle [x, y, z]
	\param scale - three values of scale [x, y, z]
	
	\exception scales value must be more than 0.0f
	\return void
	*/
/******************************************************************************/
	void Initialize(glm::vec3 pos, glm::vec3 rot = glm::vec3(), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual void Start(void) {};
	virtual void Update(void) {};
	virtual void Render(Scene& scene) {};

	/******************************************************************************/
		/*!
		\brief
		Set parent for this object

		\param parent - object parent in pointer

		*/
	/******************************************************************************/
	void SetParent(GameObject* parent);
	/******************************************************************************/
	/*!
	\brief Get parent value

	\return parent gameobject if found else null

	*/
	/******************************************************************************/
	GameObject* GetParent(void);


	/******************************************************************************/
	/*!
	\brief Add child to children list

	\param child - child gameobject to add

	*/
	/******************************************************************************/
	void AddChild(GameObject* child);
	/******************************************************************************/
	/*!
	\brief Get children list

	\return children list

	*/
	/******************************************************************************/
	UnorderedVector<GameObject*> GetChildren(void);
	/******************************************************************************/
	/*!
	\brief Find child in children list

	\param go2Find - gameobject pointer to find in children list

	\return child gameobject if found else null

	*/
	/******************************************************************************/
	GameObject* FindChild(GameObject* go2Find);
	/******************************************************************************/
	/*!
	\brief Remove a child from children list

	\param child2remove - child to remove from children list

	*/
	/******************************************************************************/
	void RemoveChild(GameObject* child2Remove);

	void SetRigidbodyPosition(const glm::vec3& newPos);
	void SetRigidbodyPosition(const float& x, const float& y, const float& z);
	glm::vec3 GetRigidbodyPosition(void) const;

	void SetRigidbodyRotation(const glm::vec3& newRotation);
	void SetRigidbodyRotation(const float& x, const float& y, const float& z);
	glm::vec3 GetRigidbodyRotation(void) const;
	
	bool operator==(const GameObject& rhs);

	static void MoveObj(Transform& transform);
};

