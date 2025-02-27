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
		GEO_TEXT,
		GEO_SPHERE,
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

	float treesX[50];
	float treesZ[50];
	float treesRotate[50];
	bool treeSpawn = false;
	bool treesSpawned;

	int Rstart = -480;
	int dist = 40;
	float RLfence[25];
	
	int Tstart = -730;
	float Tfence[38];

	int FRstart = 730;
	int FLstart = -730;

	float FRfence[15];
	float FLfence[15];
	bool fenceSpawned;

	bool CanEnterCircus = false;
	float timer = 0.0f;

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

	//glm::vec3 devVec = glm::vec3{ 0, 0, 0 };

	float lightTimer;
	bool isLightChanged;
};

#endif