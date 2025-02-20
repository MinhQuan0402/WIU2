#pragma once
#include "GameObject.h"

class Bottle : public GameObject
{
private:
    glm::vec3 m_color;
public:
    Bottle(glm::vec3 color);
    ~Bottle(void) = default;

    void Start(void) override;
    void Update(void) override;
    void LateUpdate(void);
    void Render(Scene& scene) override;
};

class TossBoard :
    public GameObject
{
public:
    TossBoard(void);
    ~TossBoard(void) = default;

    void Start(void) override;
    void Update(void) override;
    void LateUpdate(void);
    void Render(Scene& scene) override;
};

