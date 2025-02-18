#pragma once
#include "GameObject.h"

class CarnivalHitBoxes : public GameObject
{
public:
	CarnivalHitBoxes();
	~CarnivalHitBoxes();

	void Start() override;
	void Render(Scene& scene) override;
	void Update() override;
};