#pragma once
#include "GameObject.h"
class CK_Counter2 :
    public GameObject
{
public:
    CK_Counter2();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

