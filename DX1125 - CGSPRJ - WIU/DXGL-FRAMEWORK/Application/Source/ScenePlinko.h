#ifndef SCENE_PLINKO_H
#define SCENE_PLINKO_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

class ScenePlinko : public Scene
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
		GEO_CYLINDER,
		
		NUM_GEOMETRY
	};

	ScenePlinko();
	~ScenePlinko();

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
	float ballZ;
	int plinkoScore;
	bool isOnHold = true;
	bool cameraFollow = false;
	bool addScore = true;
	bool gameEnd = false;
	bool jackpot = false;

	enum OBJINSCENE
	{
		BALL1,
		BALL2,
		BALL3,
		PLINKO,

		NUM_OBJ
	};
	GameObject* objInScene[NUM_OBJ];

	int currentBallIndex;
};

#endif