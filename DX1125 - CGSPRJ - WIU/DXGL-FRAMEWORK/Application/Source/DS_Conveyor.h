#pragma once
#include "GameObject.h"
class DS_Conveyor :
    public GameObject
{
public:
    DS_Conveyor();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

