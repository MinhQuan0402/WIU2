#ifndef SCENE_CAN_KNOCKDOWN_H
#define SCENE_CAN_KNOCKDOWN_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "RigidBody.h"

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

		TABLE,
		CAN,
		TENT,
		COUNTER,

		NUM_GEOMETRY
	};

	SceneCanKnockdown();
	~SceneCanKnockdown();

	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void Exit();

private:
	void RenderSkybox(void);
	int numLight;
	Transform player;

	enum OBJINSCENE {
		GROUND,
		BALL,
		SODACAN,
		SODACAN1,
		SODACAN2,
		SODACAN3,
		SODACAN4,
		SODACAN5,
		FOLDABLE_TABLE,

		NUM_OBJ
	};
	
	GameObject* objInScene[NUM_OBJ];

	float power;
	bool isShooting;
	bool onCooldown;

	glm::vec3 devVec;
};

#endif