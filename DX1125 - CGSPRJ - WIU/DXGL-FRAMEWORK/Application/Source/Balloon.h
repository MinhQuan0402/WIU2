#pragma once
#include "GameObject.h"


class Balloon : public GameObject
{
public:
	bool setColour;
	float colourR;
	float colourB;
	float colourG;
	bool popped;

	Balloon();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

