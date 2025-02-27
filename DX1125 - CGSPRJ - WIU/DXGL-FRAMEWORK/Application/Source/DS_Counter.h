#pragma once
#include "GameObject.h"
class DS_Counter :
    public GameObject
{
public:
    DS_Counter();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

