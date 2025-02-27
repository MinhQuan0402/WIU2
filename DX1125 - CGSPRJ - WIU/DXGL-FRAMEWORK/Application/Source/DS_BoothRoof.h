#pragma once
#include "GameObject.h"
class DS_BoothRoof :
    public GameObject
{
public:
    DS_BoothRoof();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

