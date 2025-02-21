#ifndef SCENE_CARNIVAL_H
#define SCENE_CARNIVAL_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "CircusTent.h"
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


		GEO_CIRCUSTENT,
		GEO_CIRCUSNAME,
		GEO_CANTENT,
		GEO_CANTENTROOF, 
		GEO_STRIPEWALL,

		GEO_GRASS,

		GEO_TABLECAN,
		GEO_CAN, 


		GEO_TOPCOTTONCANDY,
		GEO_MIDDLECOTTONCANDY,
		GEO_INNERMIDDLECOTTONCANDY,
		GEO_COTTONCANDYS,
		GEO_COTTONCANDYMACHINE,
		GEO_CARAMELAPPLE,
		GEO_CARAMELAPPLESTAND,
		GEO_BOTTOMCOTTONCANDY,

		GEO_PLINKO,

		GEO_POOL,
		GEO_POOLWATER, 
		GEO_DUCKY,

		GEO_SHOOTINGDUCK,
		GEO_TARGET,
		GEO_CONVEYER,
		GEO_GUN,

		GEO_BOARD, 
		GEO_BALLOON,

		GEO_BOTTLES,
		GEO_RACK,
		GEO_COUNTER,
		GEO_RING,




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
	int numLight;
	Transform player;

	float ducksX[10];
	float ducksY[10];


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

		PLAYERBOX,

		NUM_OBJ
	};

	GameObject* objInscene[NUM_OBJ];

	glm::vec3 devVec = glm::vec3{ 0, 4000, 0 };
};

#endif