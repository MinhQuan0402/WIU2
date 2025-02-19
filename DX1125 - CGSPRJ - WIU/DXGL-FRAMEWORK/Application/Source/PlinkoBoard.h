#pragma once
#include <UnorderedVector.h>
#include "GameObject.h"
#include "PlinkoCylinder.h"
#include "PlinkoWall.h"
#include "PlinkoPointsWall.h"

class PlinkoBoard : public GameObject
{
public:
	PlinkoBoard();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;

	UnorderedVector<PlinkoCylinder*> cylinders;
	UnorderedVector<PlinkoWall*> walls;
	UnorderedVector<PlinkoPointsWall*> pointWalls;
};

