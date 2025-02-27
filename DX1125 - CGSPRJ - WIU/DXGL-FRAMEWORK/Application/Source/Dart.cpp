#include "Dart.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneBalloon.h"
#include "MeshManager.h"


Dart::Dart()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void Dart::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 1000.f;
	mat.m_friction = 0.7f;

	m_transform.Rotate(0, 0, 90);

	rb = addCylinderCollider(this, 0.77f, 0.05f, mat, glm::vec3(-0.19, 0.0131, 0));
	rb->setSleepingThresholds(0, 0);
	
	//m_transform.Rotate(0, 90, 0);
	//m_transform.Rotate(0, 0, -90);
	m_transform.ScaleBy(0.2, 0.2, 0.2);
}

void Dart::Update()
{
	
}

void Dart::Render(Scene& scene)
{
	MeshManager::GetInstance()->meshList[MeshManager::GEO_BP_DART]->material.kAmbient = glm::vec3(0.7f, 0.0f, 0.0f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_BP_DART]->material.kDiffuse = glm::vec3(0.9f, 0.0f, 0.0f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_BP_DART]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_BP_DART]->material.kShininess = 1.0f;
	scene.RenderRigidMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BP_DART], true, m_transform, rb);
}
