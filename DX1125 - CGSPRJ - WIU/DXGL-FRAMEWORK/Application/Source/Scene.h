#ifndef SCENE_H
#define SCENE_H

#include "MatrixStack.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "FPCamera.h"

class Scene
{
private:
	Mesh *axis, *lightMesh;

public:
	enum UNIFORM_TYPE
	{
		U_MVP = 0,

		// Add the following constants
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,

		U_LIGHT0_TYPE,
		U_LIGHT1_TYPE,

		U_LIGHT0_POSITION,
		U_LIGHT1_POSITION,

		U_LIGHT0_COLOR,
		U_LIGHT1_COLOR,

		U_LIGHT0_POWER,
		U_LIGHT1_POWER,

		U_LIGHT0_KC,
		U_LIGHT1_KC,

		U_LIGHT0_KL,
		U_LIGHT1_KL,

		U_LIGHT0_KQ,
		U_LIGHT1_KQ,

		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT1_SPOTDIRECTION,

		U_LIGHT0_COSCUTOFF,
		U_LIGHT1_COSCUTOFF,

		U_LIGHT0_COSINNER,
		U_LIGHT1_COSINNER,

		U_LIGHT0_EXPONENT,
		U_LIGHT1_EXPONENT,

		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	static bool enableLight;
	MatrixStack modelStack, viewStack, projectionStack;

	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	UnorderedVector<Mesh*> meshList;
	UnorderedVector<Light> lights;

public:
	Scene();
	~Scene() {}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;

	void RenderMesh(Mesh* mesh, bool enableLight = false);
	void RenderMesh(Mesh* mesh, bool enableLight, Transform& transform);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, glm::vec3 rotation = glm::vec3{});
	void RenderText(Mesh* mesh, std::string text, glm::vec3 color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y);
	void RenderLine(glm::vec3 startPoint, glm::vec3 endPoint, float thickness, glm::vec3 color, float depth);
protected:
	FPCamera mainCamera;

	virtual void HandleKeyPress(void);
};

#endif