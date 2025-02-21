#pragma once
#include "GameObject.h"
class DS_Bullet :
    public GameObject
{
public:
    DS_Bullet();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

