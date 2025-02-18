#pragma once
#include "GameObject.h"

class PlinkoGlass : public GameObject
{
public:
	PlinkoGlass();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

