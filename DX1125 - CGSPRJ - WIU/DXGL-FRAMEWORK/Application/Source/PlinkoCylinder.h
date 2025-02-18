#pragma once
#include "GameObject.h"

class PlinkoCylinder : public GameObject
{
public:
	PlinkoCylinder();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

