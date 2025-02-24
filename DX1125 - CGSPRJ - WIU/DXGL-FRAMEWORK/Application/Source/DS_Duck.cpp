#include "DS_Duck.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_Duck::DS_Duck() : hit{}, speed{ 3 }, animTime{}
{
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Duck::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 10000;
	m_transform.Translate(-24.3538 * 0.5 - 2.5 * 0.5, 2.3, -4.78361);
	m_transform.ScaleBy(0.4, 0.4, 0.4);
	
	//m_transform.Rotate(-90, 0, 0);
	rb = addBoxCollider(this, 1.5, 3, 0.2, mat);
	rb->setSleepingThresholds(0.0f, 0.0f);
}

void DS_Duck::Update()
{
	if (hit) {
		if (animTime < duration) {
			animTime += Time::deltaTime;
		}
		else {
			animTime = duration;
		}
	}
}

void DS_Duck::LateUpdate()
{
	if (GetRigidbodyPosition().x < 24.3538 * 0.5 + 2.5 * 0.5) {
		SetRigidbodyPosition(GetRigidbodyPosition() + glm::vec3(speed * Time::fixedDeltaTime, 0, 0));
	}
	else {
		SetRigidbodyPosition(m_transform.m_position + glm::vec3(speed * Time::fixedDeltaTime, 0, 0));
		animTime = 0;
		hit = false;
	}
	
	SetRigidbodyRotation(animTime / duration * -90, 0, 0);

	rb->clearGravity();
//	rb->setSleepingThresholds(0, 0);

	
}

void DS_Duck::Render(Scene& scene)
{
	scene.RenderRigidMesh(scene.meshList[SceneDuckShooting::GEO_DUCK], scene.enableLight, m_transform, rb);
	//scene.RenderMesh(scene.meshList[SceneDuckShooting::GEO_DUCK], scene.enableLight, m_transform);
}
