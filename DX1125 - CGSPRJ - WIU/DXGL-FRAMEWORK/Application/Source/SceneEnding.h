#ifndef SCENE_ENDING_H
#define SCENE_ENDING_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

class SceneEnding : public Scene
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
		GEO_PLANE,
		
		
		NUM_GEOMETRY
	};

	SceneEnding();
	~SceneEnding();

	virtual void Init() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void Exit() override;

private:
	void RenderSkybox(void);
	void RenderGround(int size);
	void RenderPennywise(void);

	int numLight;
	float sceneTimer;
	float sceneTimerN;
	glm::vec3 lightTarget1;
	glm::vec3 currentCameraTarget;
	float timerOffset;
	int currentClownPose;
	glm::vec3 ballonPos;
	float lightSpeed;
	glm::vec3 pennywisePos;
	float rotationXAngle;
	
	enum OBJINSCENE
	{
		GROUND,
		BOX, 
		SPHERE,
		CYLINDER,
		NUMOBJ
	};

	GameObject* objInScene[NUMOBJ];
};

#endif