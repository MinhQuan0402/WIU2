#pragma once
#include "GameObject.h"
class PlinkoPointsWall : public GameObject
{
public:
	PlinkoPointsWall();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

