#pragma once
#include "GameObject.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include <UnorderedVector.h>


class Ring :
    public GameObject
{
private:
    btCompoundShape* GenerateCollider(void);
    void GenerateRigidbody(void);

public:
    GameObject* trigger;

    Ring(void);
    ~Ring(void) = default;

    void Start(void) override;
    void Update(void) override;
    void LateUpdate(void);
    void Render(Scene& scene) override;
};

class Bottle : public GameObject
{
private:
    glm::vec3 m_color;

private:
    btCompoundShape* GenerateCollider(void);
    void GenerateRigidbody(void);

public:
    GameObject* trigger;
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
private:
    btCompoundShape* GenerateCollider(void);
    void GenerateRigidbody(void);
public:
    TossBoard(void);
    ~TossBoard(void) = default;

    void Start(void) override;
    void Update(void) override;
    void LateUpdate(void);
    void Render(Scene& scene) override;

    UnorderedVector<Bottle*> bottles;
};

class TossTable :
    public GameObject
{
private:
    btCompoundShape* GenerateCollider(void);
    void GenerateRigidbody(void);
public:
    TossTable(void);
    ~TossTable(void) = default;

    void Start(void) override;
    void Update(void) override;
    void LateUpdate(void);
    void Render(Scene& scene) override;
};

