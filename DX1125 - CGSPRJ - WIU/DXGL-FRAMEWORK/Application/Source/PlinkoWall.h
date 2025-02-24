#pragma once
#include "GameObject.h"

class PlinkoWall : public GameObject
{
public:
	PlinkoWall();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

