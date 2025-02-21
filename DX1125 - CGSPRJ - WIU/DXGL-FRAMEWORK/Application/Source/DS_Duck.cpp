#include "DS_Duck.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_Duck::DS_Duck() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Duck::Start()
{
	speed = 20;

	PhysicsMaterial mat;
	mat.m_mass = 1;
	m_transform.Translate(-24.3538 * 0.5 - 2.5 * 0.5, 2.17004, -4.78361);
	m_transform.ScaleBy(0.4, 0.4, 0.4);
	
	rb = addBoxCollider(this, 1.5, 3, 0.2, mat);
}

void DS_Duck::Update()
{
	
}

void DS_Duck::LateUpdate()
{
	if (GetRigidbodyPosition().x < 24.3538 * 0.5 + 2.5 * 0.5) {
		SetRigidbodyPosition(GetRigidbodyPosition() + glm::vec3(speed * Time::deltaTime, 0, 0));
	}
	else {
		SetRigidbodyPosition(m_transform.m_position + glm::vec3(speed * Time::deltaTime, 0, 0));
	}

	rb->clearGravity();
	rb->setSleepingThresholds(0, 0);
}

void DS_Duck::Render(Scene& scene)
{
	scene.RenderRigidMesh(scene.meshList[SceneDuckShooting::GEO_DUCK], scene.enableLight, m_transform, rb);
}
