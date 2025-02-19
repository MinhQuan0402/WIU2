#pragma once
#include "GameObject.h"
class CK_Table :
    public GameObject
{
public:
    CK_Table();

    void Start() override;
    void Update() override;
    void Render(Scene& scene) override;
};

