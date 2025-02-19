#pragma once
#include "GameObject.h"
class CK_Can :
    public GameObject
{
public:
    CK_Can();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

