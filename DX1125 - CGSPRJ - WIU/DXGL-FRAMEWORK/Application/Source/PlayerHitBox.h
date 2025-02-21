#pragma once
#include "GameObject.h"
class PlayerHitBox : public GameObject
{
public:
	PlayerHitBox();
	~PlayerHitBox();

	void Start() override;
	void Render(Scene& scene) override;
	void Update() override;
};