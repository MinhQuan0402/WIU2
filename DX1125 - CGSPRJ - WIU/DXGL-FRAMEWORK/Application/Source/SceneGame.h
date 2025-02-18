#ifndef SCENE_GAME_H
#define SCENE_GAME_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

class SceneGame : public Scene
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
		NUM_GEOMETRY
	};

	SceneGame();
	~SceneGame();

	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void Exit();

private:
	void RenderSkybox(void);
	int numLight;
	
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