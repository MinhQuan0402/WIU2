#pragma once
#include "GameObject.h"
class CK_BoothWall :
    public GameObject
{
public:
    CK_BoothWall();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

