#ifndef FPCAMERA_H
#define FPCAMERA_H
#include "GameObject.h"

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class FPCamera : public GameObject
{
public:
    glm::vec3 target;
    glm::vec3 up;
    glm::vec3 forward;
    float sensitivity; // Adjust sensitivity for both yaw and pitch

    FPCamera();
    ~FPCamera();

    void Init(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0,1,0));
    void Update(void);

    glm::vec3 view;
    glm::vec3 right;
    void UpdateCameraPosition(float x, float y, float z);
    void UpdateCameraRotation(void);
private:

    bool isDirty;   // indicate if there is a need to recalculate the camera attributes

    void Refresh();
};

#endif

