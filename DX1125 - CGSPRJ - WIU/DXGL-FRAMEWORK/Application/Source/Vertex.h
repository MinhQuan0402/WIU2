#ifndef VERTEX_H
#define VERTEX_H

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

constexpr glm::vec3 VECTOR3_RIGHT = glm::vec3(1, 0, 0);
constexpr glm::vec3 VECTOR3_UP = glm::vec3(0, 1, 0);
constexpr glm::vec3 VECTOR3_FORWARD = glm::vec3(0, 0, 1);
constexpr glm::vec3 ZERO = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 ONE = glm::vec3(1.0f, 1.0f, 1.0f);

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 textCoord; // texture coordinate
};
#endif