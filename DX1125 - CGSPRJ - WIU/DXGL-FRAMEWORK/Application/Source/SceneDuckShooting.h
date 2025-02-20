#ifndef SCENE_DUCK_SHOOTING_H
#define SCENE_DUCK_SHOOTING_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "RigidBody.h"

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
		GEO_PLANE,
		GEO_CUBE,
		GEO_CYLINDER,

		GEO_POWERUI_FRAME,
		GEO_POWERUI_BAR,

		GEO_BOOTHROOF,
		GEO_BOOTHGUARDS,
		GEO_COUNTER,
		GEO_LIGHTBULB,

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
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float width, float height, glm::vec2 anchorPoint);

	int numLight;
	Transform player;

	float fov;

	enum OBJINSCENE {
		GROUND,
		BALL,
		CAN,
		CAN2,
		CAN3,
		CAN4,
		CAN5,
		CAN6,
		TABLE,
		COUNTER,
		COUNTER2,
		COUNTER3,
		FLOOR,
		BOOTH_WALL,
		BOOTH_ROOF,

		NUM_OBJ
	};
	
	GameObject* objInScene[NUM_OBJ];

	float power;
	const float maxPower = 30;

	bool isShooting;
	float cooldownTimer;

	float attemptsLeft;
	bool gameComplete;
};

#endif