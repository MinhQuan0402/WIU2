#include "Utility.h"

glm::mat4 GetTransformMatrix(btRigidBody* body)
{
    glm::mat4 mat{};
    btTransform t;
    t = body->getWorldTransform();
    float tempMat[16];
    t.getOpenGLMatrix(tempMat);
    return GetTransformMatrix(tempMat);
}

glm::mat4 GetTransformMatrix(float tempMat[16])
{
    glm::mat4 mat{};
    unsigned index = 0;
    for (unsigned i = 0; i < 4; ++i) { 
        for (unsigned j = 0; j < 4; ++j) { 
            mat[i][j] = tempMat[index++]; } }
    return mat;
}

Transform ConvertMatrix2Transform(float mat[16])
{
    Transform t;
    t.m_position = glm::vec3(mat[12], mat[13], mat[14]);
    glm::mat3 rotationMatrix = glm::mat3(
        glm::normalize(glm::vec3(mat[0])),
        glm::normalize(glm::vec3(mat[1])),
        glm::normalize(glm::vec3(mat[2]))
    );
    glm::quat rotationQuat = glm::quat_cast(rotationMatrix);
    t.m_rotation = glm::eulerAngles(rotationQuat);
    t.m_rotation = glm::degrees(t.m_rotation);
    t.m_rotation = glm::vec3(t.m_rotation.y, t.m_rotation.x, t.m_rotation.z);
    return t;
}

glm::mat4 addMatrix(glm::mat4 a, glm::mat4 b)
{
    glm::mat4 newMatrix{};
    for (unsigned i = 0; i < 4; ++i)
    {
        for (unsigned j = 0; j < 4; ++j)
        {
            newMatrix[i][j] = a[i][j] + b[i][j];
        }
    }
    return newMatrix;
}

btQuaternion SetRotation(const glm::vec3& rotation)
{
    btQuaternion rotationX = btQuaternion(btVector3(1, 0, 0), glm::radians(rotation.x));
    btQuaternion rotationY = btQuaternion(btVector3(0, 1, 0), glm::radians(rotation.y));
    btQuaternion rotationZ = btQuaternion(btVector3(0, 0, 1), glm::radians(rotation.z));
    btQuaternion finalRotation = rotationX * rotationY * rotationZ;
    return finalRotation;
}

btQuaternion SetRotation(const float& x, const float& y, const float& z)
{
    btQuaternion rotationX = btQuaternion(btVector3(1, 0, 0), glm::radians(x));
    btQuaternion rotationY = btQuaternion(btVector3(0, 1, 0), glm::radians(y));
    btQuaternion rotationZ = btQuaternion(btVector3(0, 0, 1), glm::radians(z));
    btQuaternion finalRotation = rotationX * rotationY * rotationZ;
    return finalRotation;
}

btVector3 QuaternionToEuler(btQuaternion quat) {
    btScalar roll, pitch, yaw;
    btMatrix3x3(quat).getEulerZYX(yaw, pitch, roll); // Convert quaternion to Euler angles (ZYX order)
    return btVector3(roll, pitch, yaw); // Angles in radians
}
