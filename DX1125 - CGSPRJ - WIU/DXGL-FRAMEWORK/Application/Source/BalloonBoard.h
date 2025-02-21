#pragma once
#include "GameObject.h"
#include "Balloon.h"
#include <UnorderedVector.h>

class BalloonBoard : public GameObject
{
public:
	BalloonBoard();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;

	UnorderedVector<Balloon*> balloons;
};

