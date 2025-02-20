#pragma once
#include "GameObject.h"
class DS_Floor :
    public GameObject
{
public:
    DS_Floor();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

