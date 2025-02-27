#pragma once
#include "GameObject.h"
class DS_Bullet :
    public GameObject
{
public:
    bool hit;
    float timeoutTime;
    const float timeout = 3;

    DS_Bullet();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

