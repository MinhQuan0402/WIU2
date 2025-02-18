#pragma once
#include "GameObject.h"
class CK_Cube :
    public GameObject
{
public:
    CK_Cube();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

