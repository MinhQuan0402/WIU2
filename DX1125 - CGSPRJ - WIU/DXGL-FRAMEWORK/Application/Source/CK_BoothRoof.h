#pragma once
#include "GameObject.h"
class CK_BoothRoof :
    public GameObject
{
public:
    CK_BoothRoof();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

