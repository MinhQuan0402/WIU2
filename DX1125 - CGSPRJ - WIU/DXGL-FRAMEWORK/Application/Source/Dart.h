#pragma once
#include "GameObject.h"
class Dart : public GameObject
{
public:
	Dart();

	void Start() override;
	void Update() override;
	void Render(Scene& scene) override;
};

