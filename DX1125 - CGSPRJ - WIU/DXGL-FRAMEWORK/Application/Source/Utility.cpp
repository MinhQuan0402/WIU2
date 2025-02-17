#include "Utility.h"

glm::mat4 GetTransformMatrix(btRigidBody* body)
{
    glm::mat4 mat{};
    btTransform t;
    body->getMotionState()->getWorldTransform(t);
    float tempMat[16];
    t.getOpenGLMatrix(tempMat);
    unsigned index = 0;
    for (unsigned i = 0; i < 4; ++i)
    {
        for (unsigned j = 0; j < 4; ++j)
        {
            mat[i][j] = tempMat[index++];
        }
    }
    return mat;
}

Transform ConvertMatrix2Transform(float mat[16])
{
    Transform t;
    t.m_position = glm::vec3(mat[12], mat[13], mat[14]);
    glm::mat3 rotationMatrix = glm::mat3(
        glm::normalize(glm::vec3(mat[0])),
        glm::normalize(glm::vec3(mat[5])),
        glm::normalize(glm::vec3(mat[10]))
    );
    glm::quat rotationQuat = glm::quat_cast(rotationMatrix);
    t.m_rotation = glm::eulerAngles(rotationQuat);
    t.m_rotation = glm::degrees(t.m_rotation);
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
