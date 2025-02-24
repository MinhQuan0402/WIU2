#include <iostream>
#include "Balloon.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneBalloon.h"
#include "Dart.h"

Balloon::Balloon()
{
	setColour = false;
	colourR = 0.f;
	colourG = 0.f;
	colourB = 0.f;
	popped = false;
	GameObjectManager::GetInstance()->addItem(this);
}

void Balloon::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	m_transform.ScaleBy(3, 3, 3);

	rb = addSphereCollider(this, 0.45f, mat, glm::vec3(0.681, -0.105, 0));

	m_transform.Rotate(0.f, 0.f, -100.f);
}

void Balloon::Update()
{
	if (setColour == false) {

		colourR = ((float)rand() / (RAND_MAX));
		std::cout << colourR << std::endl;

		colourG = ((float)rand() / (RAND_MAX));
		std::cout << colourG << std::endl;

		colourB = ((float)rand() / (RAND_MAX));
		std::cout << colourB << std::endl;

		setColour = true;
	}

	//if (rb->checkCollideWith())
}

void Balloon::Render(Scene& scene)
{
	scene.meshList[SceneBalloon::GEO_BALLOON]->material.kAmbient = glm::vec3(colourR - 0.2, colourG - 0.2, colourB - 0.2);
	scene.meshList[SceneBalloon::GEO_BALLOON]->material.kDiffuse = glm::vec3(colourR, colourG, colourB);
	scene.meshList[SceneBalloon::GEO_BALLOON]->material.kSpecular = glm::vec3(0.200000, 0.200000, 0.200000);
	scene.meshList[SceneBalloon::GEO_BALLOON]->material.kShininess = 1.f;

	scene.RenderMesh(scene.meshList[SceneBalloon::GEO_BALLOON], true, m_transform);
}
