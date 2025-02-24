#ifndef SCENE_BALLOON_H
#define SCENE_BALLOON_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

class SceneBalloon : public Scene
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
		GEO_CYLINDER,
		GEO_BALLOON,
		GEO_BALLOONBOARD,
		GEO_DART,
		GEO_BPBOOTHGUARDS,
		GEO_BPBOOTHROOF,
		GEO_POWERUI_FRAME,
		GEO_POWERUI_BAR,
		NUM_GEOMETRY
	};

	SceneBalloon();
	~SceneBalloon();

	virtual void Init() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void Exit() override;

private:
	void RenderSkybox(void);
	int numLight;

	float fov;
	float cooldownTimer;
	bool isShooting;
	bool gameComplete;
	int attemptsLeft;
	float power;
	float maxPower;

	
	enum OBJINSCENE
	{
		GROUND,
		BALLOON,
		BALLOONBOARD,
		DART,
		NUMOBJ
	};

	GameObject* objInScene[NUMOBJ];
};

#endif