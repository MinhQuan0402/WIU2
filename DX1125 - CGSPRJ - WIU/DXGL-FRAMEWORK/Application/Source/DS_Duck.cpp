#include "DS_Duck.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"
#include "ColliderManager.h"

int DS_Duck::nextId = 1;

DS_Duck::DS_Duck() : hit{}, speed{ 7 }, animTime{}
{
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Duck::Start()
{
	srand(time(0) + nextId);
	nextId++;
	size = (rand() % 76 + 50) / 100.0f;

	PhysicsMaterial mat;
	mat.m_mass = 10000;
	m_transform.Translate(-24.3538 * 0.5 - 2.5 * 0.5, 2.3, -4.78361);
	m_transform.ScaleBy(0.4 * size, 0.4 * size, 0.4 * size);
	
	//m_transform.Rotate(-90, 0, 0);
	rb = addBoxCollider(this, 1.5 * size, 3 * size, 0.2 * size, mat);
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
}

void DS_Duck::Render(Scene& scene)
{
	scene.RenderRigidMesh(scene.meshList[SceneDuckShooting::GEO_DUCK], scene.enableLight, m_transform, rb);
	//scene.RenderMesh(scene.meshList[SceneDuckShooting::GEO_DUCK], scene.enableLight, m_transform);
}
