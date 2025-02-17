#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"

constexpr glm::vec3 RED = glm::vec3(1.f, 0.f, 0.f);
constexpr glm::vec3 GREEN = glm::vec3(0.f, 1.f, 0.f);
constexpr glm::vec3 BLUE = glm::vec3(0.f, 0.f, 1.f);
constexpr glm::vec3 BLACK = glm::vec3(0.f, 0.f, 0.f);
constexpr glm::vec3 WHITE = glm::vec3(1.f, 1.f, 1.f);
constexpr glm::vec3 MAINCOLOR = glm::vec3(0.133f, 0.133f, 0.322f);
constexpr glm::vec3 BRIGHTBLUE = glm::vec3(0.f, 1.f, 1.f);
constexpr glm::vec3 YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 GREY = glm::vec3(0.349f, 0.349f, 0.349f);

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/;
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string& meshName, glm::vec3 color, float length = 1.f);
	static Mesh* GenerateSphere(const std::string& meshName, glm::vec3 color, float radius = 1.f, int numSlice = 360, int numStack = 360);
	static Mesh* GenerateRing(const std::string& meshName, glm::vec3 color, int numSlice, float outerR, float innerR);
	static Mesh* GenerateCube(const std::string& meshName, glm::vec3 color, float length);
	static Mesh* GenerateHemisphere(const std::string& meshName, glm::vec3 color, int numStack, int numSlice, float radius);
	static Mesh* GenerateCylinder(const std::string& meshName, glm::vec3 color, int numSlice, float radius, float height);
	static Mesh* GenerateCustomCylinder(const std::string& meshName, glm::vec3 color, int numSlice, float fstRadius, float secRadius, float height);
	static Mesh* GenerateTorus(const std::string& meshName,	glm::vec3 color, float innerR = 1.f, float outerR = 1.f, int numSlice = 360, int numStack = 360);
	static Mesh* GenerateRamp(const std::string& meshName, glm::vec3 color, float length, float height);
	static Mesh* GenerateCircle(const std::string& meshName, glm::vec3 color, float radius = 1.0f);
	static Mesh* GenerateSemiCircle(const std::string& meshName, glm::vec3 color, float radius = 1.0f, int numberOfSides = 180);
	static Mesh* GenerateTrapezoid(const std::string& meshName, glm::vec3 color, float fstLength = 0.5f, float secLength = 1.0f);
	static Mesh* GenerateOBJ(const std::string& meshName, const	std::string& file_path);
	static Mesh* GenerateOBJMTL(const std::string& meshName, const std::string& file_path, const std::string& mtl_path);
	static Mesh* GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol);

};

#endif