#ifndef SCENE_PLINKO_H
#define SCENE_PLINKO_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

class ScenePlinko : public Scene
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
		GEO_PLINKO,
		NUM_GEOMETRY
	};

	ScenePlinko();
	~ScenePlinko();

	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void Exit();

private:
	void RenderSkybox(void);
	int numLight;
	Transform player;

	enum OBJINSCENE
	{
		BALL,
		CYLINDER,
		CYLINDER1,
		CYLINDER2,
		CYLINDER3,
		CYLINDER4,
		CYLINDER5,
		CYLINDER6,
		CYLINDER7,
		CYLINDER8,
		CYLINDER9,
		CYLINDER10,
		CYLINDER11,
		CYLINDER12,
		CYLINDER13,
		CYLINDER14,
		CYLINDER15,
		CYLINDER16,
		CYLINDER17,
		CYLINDER18,
		CYLINDER19,
		CYLINDER20,
		CYLINDER21,
		CYLINDER22,
		LWALL,
		RWALL,
		BOTWALL,
		BACKWALL,
		WALL1,
		WALL2,
		WALL3,
		WALL4,
		PLINKO,

		NUM_OBJ
	};
	GameObject* objInScene[NUM_OBJ];
};

#endif