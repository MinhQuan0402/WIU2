#ifndef SCENE_CAN_KNOCKDOWN_H
#define SCENE_CAN_KNOCKDOWN_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"


class SceneCanKnockdown : public Scene
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

	SceneCanKnockdown();
	~SceneCanKnockdown();

	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void Exit();

private:
	void RenderSkybox(void);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float width, float height, glm::vec2 anchorPoint);

	void RenderGround(int size);

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

	int points;
	float lightTimer;

	float power;
	const float maxPower = 25;

	bool isShooting;
	float cooldownTimer;

	float attemptsLeft;
	bool gameComplete;
};

#endif