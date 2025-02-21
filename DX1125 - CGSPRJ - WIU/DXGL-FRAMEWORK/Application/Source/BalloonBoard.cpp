#include "BalloonBoard.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneBalloon.h"

BalloonBoard::BalloonBoard()
{
	float start = 5;
	float dist = 1.5;

	for (unsigned i = 0; i < 10; i++)
	{
		Balloon* balloon = new Balloon();
		balloon->m_transform.Translate(-5.2, 3, start);
		balloons.push_back(balloon);

		start -= dist;
	}

	GameObjectManager::GetInstance()->addItem(this);
}

void BalloonBoard::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	m_transform.ScaleBy(0.05, 0.05, 0.05);

	rb = addBoxCollider(this, 0.5f, 7.3f, 13.8f, mat);

	m_transform.Rotate(0, -90, 0);
}

void BalloonBoard::Update()
{

}

void BalloonBoard::Render(Scene& scene)
{
	scene.meshList[SceneBalloon::GEO_BALLOONBOARD]->material.kAmbient = glm::vec3(0.6, 0.6, 0.6);
	scene.meshList[SceneBalloon::GEO_BALLOONBOARD]->material.kDiffuse = glm::vec3(0.7f, 0.7f, 0.7f);
	scene.meshList[SceneBalloon::GEO_BALLOONBOARD]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	scene.meshList[SceneBalloon::GEO_BALLOONBOARD]->material.kShininess = 1.0f;
	scene.RenderMesh(scene.meshList[SceneBalloon::GEO_BALLOONBOARD], true, m_transform);
}
