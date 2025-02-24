#include "Dart.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneBalloon.h"


Dart::Dart()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void Dart::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 1.f;

	m_transform.Rotate(0, 0, 90);

	rb = addCylinderCollider(this, 0.77f, 0.05f, mat, glm::vec3(-0.19, 0.0131, 0));

	m_transform.Rotate(0, 90, 0);
	m_transform.Rotate(0, 0, -90);
	m_transform.ScaleBy(0.2, 0.2, 0.2);
}

void Dart::Update()
{

}

void Dart::Render(Scene& scene)
{
	scene.meshList[SceneBalloon::GEO_DART]->material.kAmbient = glm::vec3(0.7f, 0.0f, 0.0f);
	scene.meshList[SceneBalloon::GEO_DART]->material.kDiffuse = glm::vec3(0.9f, 0.0f, 0.0f);
	scene.meshList[SceneBalloon::GEO_DART]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	scene.meshList[SceneBalloon::GEO_DART]->material.kShininess = 1.0f;
	scene.RenderMesh(scene.meshList[SceneBalloon::GEO_DART], true, m_transform);
}
