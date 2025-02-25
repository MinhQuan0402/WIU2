#pragma once
#include "GameObject.h"
class CK_Counter :
    public GameObject
{
public:
    CK_Counter();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

