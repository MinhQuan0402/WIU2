#pragma once
#include <Transform.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "FPCamera.h"

glm::mat4 GetTransformMatrix(btRigidBody* body);
glm::mat4 GetTransformMatrix(float mat[16]);

Transform ConvertMatrix2Transform(float mat[16]);

glm::mat4 addMatrix(glm::mat4 a, glm::mat4 b);

btQuaternion SetRotation(const glm::vec3& rotation);
btQuaternion SetRotation(const float& x, const float& y, const float& z);
btVector3 QuaternionToEuler(btQuaternion quat);

bool isObjectInteractable(const glm::vec3& objectPos, FPCamera camera, float cameraFOV, float maxInteractionDistance);

void printVector(glm::vec3 rhs);

bool isTimeReach(const float& currentTime, const float& from, const float& to);

glm::vec3 Lerp(const float elapsedTime, glm::vec3 startPos, const glm::vec3 endPos, float duration);