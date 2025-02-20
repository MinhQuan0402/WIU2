#pragma once
#include <Transform.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

glm::mat4 GetTransformMatrix(btRigidBody* body);
glm::mat4 GetTransformMatrix(float mat[16]);

Transform ConvertMatrix2Transform(float mat[16]);

glm::mat4 addMatrix(glm::mat4 a, glm::mat4 b);

btVector3 QuaternionToEuler(btQuaternion quat);