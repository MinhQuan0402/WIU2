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
		GEO_TORUS,

		GEO_FD_DUCKY,
		GEO_FD_TENTFRAME,
		GEO_FD_TENTROOF,
		GEO_FD_POOL,

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
	int numLight;
	
	enum OBJINSCENE
	{
		GROUND,
		/*BOX, 
		SPHERE,
		CYLINDER,*/
		DUCK,
		POOLWALL,
		NUMOBJ
	};

	GameObject* objInScene[NUMOBJ];
};

#endif