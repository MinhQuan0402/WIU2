#pragma once
#include "GameObject.h"
class DS_Counter2 :
    public GameObject
{
public:
    DS_Counter2();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

