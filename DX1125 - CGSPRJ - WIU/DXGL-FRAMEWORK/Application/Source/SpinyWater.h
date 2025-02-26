#pragma once
#include "GameObject.h"
class SpinyWater :
    public GameObject 
{
public:
    SpinyWater(); 
    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;

};

