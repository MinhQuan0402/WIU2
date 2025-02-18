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
		GEO_SHOOTINGGAME,
		NUM_GEOMETRY
	};

	carnivalroaming();
	~carnivalroaming();

	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void Exit();

private:
	void RenderSkybox(void);
	int numLight;
	Transform player;

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


		NUM_OBJ

	};

	GameObject* objInscene[NUM_OBJ];
};

#endif