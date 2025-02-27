#ifndef SCENE_DUCKFISHING_H
#define SCENE_DUCKFISHING_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

class SceneDuckFishing : public Scene
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
		

		GEO_FD_DUCKY,
		GEO_FD_TENTFRAME,
		GEO_FD_TENTROOF,
		GEO_FD_POOL,
		GEO_FD_WATER,
		GEO_FD_STEP_STOOL,
		GEO_FD_DUCKBOTTOM,

		NUM_GEOMETRY
	};

	SceneDuckFishing();
	~SceneDuckFishing();

	virtual void Init() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void Exit() override;

private:
	void RenderSkybox(void);
	void RenderGround(int size);
	int numLight;
	
	float duckX[20];
	float duckZ[20];
	float duckRotation[20];

	float spinnerRotation;
	float threshold = 1.1f;
	bool isPickable; 
	bool isInView;
	bool isSelected; 
	int lightPower; 

	float time;
	float lightTimer1;
	int random_number;
	float fov;
	glm::vec3 spotLightDuckPosition;
	enum OBJINSCENE
	{
		GROUND,
		/*BOX, 
		SPHERE,
		CYLINDER,*/
		DUCK,
		DUCK2,
		DUCK3,
		DUCK4,
		DUCK5,
		DUCK6,
		DUCK7,
		DUCK8,
		DUCK9,
		DUCK10,
		DUCK11,
		DUCK12,
		DUCK13,
		DUCK14,
		DUCK15,
		DUCK16,
		DUCK17,
		DUCK18,
		DUCK19,
		DUCK20,

		POOLWALL,
		POOLWALL2,
		POOLWALL3,

		POOL_SPINNER,
		POOL_SPINNER2,
		POOL_SPINNER3,
		POOL_SPINNER4,
		POOL_SPINNER5,
		POOL_SPINNER6,
		POOL_SPINNER7,
		POOL_SPINNER8,
		POOL_SPINNER9,

		POOLFLOOR,

		NUMOBJ
	};

	GameObject* objInScene[NUMOBJ];
};

#endif