#pragma once
#include "GameObject.h"

class PlinkoBall : public GameObject
{
public:
	PlinkoBall();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

