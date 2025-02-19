#include "PlinkoBoard.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "PlinkoCylinder.h"
#include "PlinkoWall.h"
#include "ScenePlinko.h"

PlinkoBoard::PlinkoBoard()
{
	float start = 1.35;
	float startY = 0;
	float dist = 0.3;
	float distY = 0.26;

	GameObjectManager::GetInstance()->addItem(this);

	// Plinko pins
	for (unsigned row = 0; row < 13; row++)
	{
		unsigned numColumn = 9;
		if (row % 2 == 0) numColumn = 10;
		for (unsigned column = 0; column < numColumn; ++column)
		{
			PlinkoCylinder* cylinder = new PlinkoCylinder();
			cylinder->m_transform.Translate(0, startY, start);
			cylinders.push_back(cylinder);
			start -= dist;
		}

		if (numColumn == 9) {
			start = 1.35;
		}
		else {
			start = 1.2;
		}
		startY -= distY;
	}

	float wallRotate = 60;
	float wallStart = 1.3;
	float wallStartY = 4.85;
	float wallDistY = 0.26;

	// Plinko wall 30 degree
	for (unsigned row = 0; row < 12; row++) 
	{
		PlinkoWall* wall = new PlinkoWall();
		wall->m_transform.Translate(0, wallStartY, wallStart);
		//wall->m_transform.Rotate(-60, 0, 0);
		walls.push_back(wall);

		wallStart = -wallStart;
		wallStartY -= wallDistY;
	}

	wallStart = -1.3;
	wallStartY = 4.85;

	// Plinko wall -30 degree
	for (unsigned row = 0; row < 12; row++)
	{
		PlinkoWall* wall = new PlinkoWall();
		wall->m_transform.Translate(0, wallStartY, wallStart);
		wall->m_transform.Rotate(-60, 0, 0);
		walls.push_back(wall);

		wallStart = -wallStart;
		wallStartY -= wallDistY;
	}

	float pointWallStart = 0;

	// Point wall
	for (unsigned column = 0; column < 10; column++) 
	{
		PlinkoPointsWall* pointWall = new PlinkoPointsWall();
		pointWall->m_transform.Translate(0, 0, pointWallStart);
		pointWalls.push_back(pointWall);

		pointWallStart -= dist;
	}
}

void PlinkoBoard::Start()
{
}

void PlinkoBoard::Update()
{
}

void PlinkoBoard::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[ScenePlinko::GEO_PLINKO], false, m_transform);
}
