#pragma once
#include "GameObject.h"

class CircusTent : public GameObject
{
public:
	CircusTent();
	~CircusTent();

	void Start() override;
	void Render(Scene& scene) override;
	void Update() override;
};