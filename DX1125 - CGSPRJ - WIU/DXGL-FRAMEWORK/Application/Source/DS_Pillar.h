#pragma once
#include "GameObject.h"
class DS_Pillar :
    public GameObject
{
public:
    DS_Pillar();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

