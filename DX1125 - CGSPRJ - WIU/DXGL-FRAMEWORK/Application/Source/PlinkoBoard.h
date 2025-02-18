#pragma once
#include "GameObject.h"
class PlinkoBoard : public GameObject
{
public:
	PlinkoBoard();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

