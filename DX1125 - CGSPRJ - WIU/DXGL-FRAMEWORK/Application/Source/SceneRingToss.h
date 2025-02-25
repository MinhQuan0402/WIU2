#ifndef SCENE_RING_TOSS_H
#define SCENE_RING_TOSS_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "Button_UI.h"

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
	int numAttempt;
	
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

	int points;
	GameObject* objInScene[NUMOBJ];

	Button_UI retryButton;
	Button_UI exitButton;
};

#endif