#pragma once
#include "GameObject.h"
class Ducks :
    public GameObject
{
public:
    Ducks();
    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;

};

