#pragma once
#include "GameObject.h"
class CK_Ball :
    public GameObject
{
public:
    CK_Ball();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

