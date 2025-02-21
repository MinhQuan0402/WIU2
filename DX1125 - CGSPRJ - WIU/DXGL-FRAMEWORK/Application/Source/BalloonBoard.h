#pragma once
#include "GameObject.h"
class BalloonBoard : public GameObject
{
public:
	BalloonBoard();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

