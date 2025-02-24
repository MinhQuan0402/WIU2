#pragma once
#include "GameObject.h"
class DS_Duck:
    public GameObject
{
public:
    float speed;
    float animTime;
    const float duration = 0.25f;
    bool hit;

    DS_Duck();

    void Start() override;
    void Update() override;
    void LateUpdate() override;
    void Render(Scene& scene) override;
};

