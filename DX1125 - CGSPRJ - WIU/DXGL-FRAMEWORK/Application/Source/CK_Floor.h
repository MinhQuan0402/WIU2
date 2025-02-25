#pragma once
#include "GameObject.h"
class CK_Floor :
    public GameObject
{
public:
    CK_Floor();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

