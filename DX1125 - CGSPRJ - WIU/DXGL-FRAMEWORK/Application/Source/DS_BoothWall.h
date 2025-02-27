#pragma once
#include "GameObject.h"
class DS_BoothWall :
    public GameObject
{
public:
    DS_BoothWall();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

