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
		GEO_TENT,
		GEO_BENCH,
		GEO_BENCH_POLE,
		GEO_UNDERSEATS,
		GEO_BENCH_STAIRS,
		GEO_CIRCLE_STAGE,
		GEO_STAGE_RING_LOWER,
		GEO_STAGE_RING_UPPER,
		GEO_STAGE_POLE,
		GEO_TENT_POLE,
		GEO_STAGE_HANGING_POLE,
		GEO_STAGE_STRING,
		GEO_STAGE_HOOK,
		GEO_STAGE_HULA_HOOP,
		GEO_DEC_SPHERE,
		GEO_DEC_STRIP,
		GEO_CEILING_RING,
		GEO_DEC_RING,
		GEO_CLOWN_POSE_1,
		GEO_SPHERE,

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
	int numLight;
	float sceneTimer;
	
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