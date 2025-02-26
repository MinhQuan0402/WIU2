#ifndef SCENE_DUCK_SHOOTING_H
#define SCENE_DUCK_SHOOTING_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "Time.h"
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
		GEO_DS_PLANE,
		GEO_CUBE,
		GEO_DS_QUAD,
		GEO_CYLINDER,

		GEO_DS_BOOTHROOF,
		GEO_DS_BOOTHGUARDS,
		GEO_DS_COUNTER,
		GEO_DS_LIGHTBULB,

		GEO_DS_DUCK,
		GEO_DS_GUN,
		GEO_DS_BULLET,
		GEO_DS_PILLAR,
		GEO_DS_CONVEYOR,

		GEO_DS_FAIRYLIGHT,

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

	glm::vec3 devVec = glm::vec3{ 19.127 - 37.4806, 7.9206, 0.0354775 };
};

#endif