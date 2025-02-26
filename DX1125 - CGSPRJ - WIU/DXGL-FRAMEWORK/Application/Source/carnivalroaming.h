#ifndef SCENE_CARNIVAL_H
#define SCENE_CARNIVAL_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

class carnivalroaming : public Scene
{
public:
	enum GEOMENTRY_TYPE
	{
		GEO_AXIS,
		GEO_LIGHT,
		GEO_FRONT,
		GEO_BACK,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_RIGHT,
		GEO_LEFT,
		GEO_HITBOX,
		GEO_TEXT,
		GEO_SPHERE,
		GEO_MODEL1,
		GEO_PLANE,
		GEO_CUBE,
		GEO_TICKET,

		NUM_GEOMETRY
	};

	carnivalroaming();
	~carnivalroaming();

	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void Exit();

private:
	void RenderSkybox(void);
	void RenderGround(int size);
	int numLight;
	Transform player;

	float ducksX[10];
	float ducksY[10];

	bool CanEnterCircus = false;
	float timer = 0.0f;

	float energy;
	const float maxEnergy = 100;

	enum OBJSCENE
	{
		CIRCLE,
		BOX,
		BOX2,
		BOX3,
		BOX4,
		BOX5,
		BOX6,
		BOX7,
		BOX8,
		BOX9,
		BOX10,
		BOX11,
		BOX12,
		BOX13,

		PLAYERBOX,
		GROUND,

		NUM_OBJ
	};

	GameObject* objInscene[NUM_OBJ];

	glm::vec3 devVec = glm::vec3{ 0, 4000, 0 };
};

#endif