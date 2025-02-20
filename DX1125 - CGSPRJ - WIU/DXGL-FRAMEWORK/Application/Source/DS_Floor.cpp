#include "DS_Floor.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_Floor::DS_Floor() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Floor::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	mat.m_friction = 1;
	m_transform.Translate(0, 0, 0);
	m_transform.Rotate(glm::vec3(90, 0, 0));
	rb = addBoxCollider(this, 1000, 1000, 0, mat);
}

void DS_Floor::Update()
{
}

void DS_Floor::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneDuckShooting::GEO_PLANE], scene.enableLight, m_transform);
}
