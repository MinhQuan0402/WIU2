#pragma once
#include "GameObject.h"
class PoolFloor :
    public GameObject
{
 public:
    PoolFloor();
    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

