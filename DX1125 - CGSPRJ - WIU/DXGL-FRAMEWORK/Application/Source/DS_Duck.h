#pragma once
#include "GameObject.h"
class DS_Duck:
    public GameObject
{
public:
    float speed;

    DS_Duck();

    void Start() override;
    void Update() override;
    void LateUpdate() override;
    void Render(Scene& scene) override;
};

