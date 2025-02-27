#ifndef SCENE_DUCK_SHOOTING_H
#define SCENE_DUCK_SHOOTING_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "UnorderedVector.h"
#include "DS_Duck.h"
#include "DS_Bullet.h"

class SceneDuckShooting : public Scene
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
		GEO_CUBE,
		GEO_CYLINDER,

		GEO_PLANE,

		NUM_GEOMETRY
	};

	SceneDuckShooting();
	~SceneDuckShooting();

	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void Exit();

private:
	void RenderSkybox(void);
	void RenderGround(int size);
	//void RenderMeshOnScreen(Mesh* mesh, float x, float y, float width, float height, glm::vec2 anchorPoint);

	int numLight;
	Transform player;

	float fov;

	enum OBJINSCENE {
		COUNTER,
		COUNTER2,
		COUNTER3,
		FLOOR,
		BOOTH_WALL,
		BOOTH_ROOF,

		CONVEYOR,
		PILLAR,
		PILLAR2,

		NUM_OBJ
	};
	
	GameObject* objInScene[NUM_OBJ];

	float timer;

	Transform gunTransform;

	float bulletSpeed;
	bool recoil;
	bool recoilBack;
	float recoilTime;
	const float recoilDuration = 0.1f;
	float lightTimer;
	bool isLightChanged;

	UnorderedVector<DS_Duck*> ducks;
	UnorderedVector<DS_Bullet*> bullets;

	float spawnTimer;
	float spawnRate;
	const float minSpawnRate = 0.25f;

	float cooldownTimer;

	int points;

	bool gameComplete;
	float gameCompletePause;
	bool switchedScene;
};

#endif