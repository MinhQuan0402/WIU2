#ifndef SCENE_RING_TOSS_H
#define SCENE_RING_TOSS_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

class SceneRingToss : public Scene
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
		GEO_BOARD,
		GEO_BOTTLE,
		GEO_RING, 
		GEO_TIE,
		GEO_PLANE,
		GEO_TABLE,
		GEO_TABLECLOTH,
		GEO_COUNTER,
		GEO_LIGHTPOLE,
		NUM_GEOMETRY
	};

	SceneRingToss();
	~SceneRingToss();

	virtual void Init() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void Exit() override;

private:
	void RenderSkybox(void);
	void RenderGround(int size);
	int numLight;
	bool isShoot;
	bool isPickable;
	int currentIndexRing;
	float lightTimer;
	bool isInView;
	
	enum OBJINSCENE
	{
		GROUND,
		WALLRIGHT,
		WALLLEFT,
		WALLBACK,
		BOARD,
		RING,
		RING2,
		RING3,
		TABLE,
		COUNTER,
		PLAYER,
		NUMOBJ
	};

	GameObject* objInScene[NUMOBJ];
};

#endif