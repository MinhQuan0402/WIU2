#pragma once
#include "GameObject.h"
class CK_Cylinder :
    public GameObject
{
public:
    CK_Cylinder();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

