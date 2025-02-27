#pragma once
#include "GameObject.h"
class BP_Box : public GameObject
{
public:
	BP_Box();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

