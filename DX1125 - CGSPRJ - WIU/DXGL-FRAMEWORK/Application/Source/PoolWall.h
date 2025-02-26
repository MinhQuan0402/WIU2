#pragma once
#include "GameObject.h"
class PoolWall :
    public GameObject 
{
public:
    PoolWall();
    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

