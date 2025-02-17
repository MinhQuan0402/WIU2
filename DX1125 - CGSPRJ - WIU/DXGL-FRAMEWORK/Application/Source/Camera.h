#pragma once
#include "GameObject.h"
class Camera :
    public GameObject
{
public:
    glm::vec3 cameraTarget;
    glm::vec3 cameraUp;
    float fov;

private:
    float movementSpeed;
    bool isDirty;   // indicate if there is a need to recalculate the camera attributes

    glm::vec3 CalculateTarget(void);

public:
    Camera(void);
    ~Camera(void);

    void Init(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0, 1, 0));
    void Update(void) override;
};

