#include "BalloonBoard.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneBalloon.h"

BalloonBoard::BalloonBoard()
{

	srand((unsigned)time(0));
	

	float start = 5;
	float dist = 1.5;
	float startY = 6;

	for (unsigned row = 0; row < 4; row++)
	{
		for (unsigned column = 0; column < 8; column++)
		{
			Balloon* balloon = new Balloon();
			balloon->m_transform.Translate(-5.1, startY, start);
			balloons.push_back(balloon);

			start -= dist;
		}
		startY -= dist;
		start = 5;
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
	scene.meshList[SceneBalloon::GEO_BP_BALLOONBOARD]->material.kAmbient = glm::vec3(0.5, 0.5, 0.5);
	scene.meshList[SceneBalloon::GEO_BP_BALLOONBOARD]->material.kDiffuse = glm::vec3(0.7f, 0.7f, 0.7f);
	scene.meshList[SceneBalloon::GEO_BP_BALLOONBOARD]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	scene.meshList[SceneBalloon::GEO_BP_BALLOONBOARD]->material.kShininess = 1.0f;

	scene.RenderMesh(scene.meshList[SceneBalloon::GEO_BP_BALLOONBOARD], true, m_transform);
}
