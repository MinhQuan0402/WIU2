#pragma once
#include "GameObject.h"
class BP_Counter : public GameObject
{
public:
	BP_Counter();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

