#ifndef SCENE_MAIN_MENU_H
#define SCENE_MAIN_MENU_H

#include "Scene.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

#include "Button_UI.h"


class MainMenu : public Scene
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
		GEO_PLANE,
		GEO_CUBE,

		GEO_BTNFRAME,

		NUM_GEOMETRY
	};

	MainMenu();
	~MainMenu();

	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void Exit();

	
private:
	void RenderSkybox(void);
	void RenderGround(int size);
	glm::vec3 Lerp(float elapsedTime, float duration, glm::vec3 origin, glm::vec3 endPos);
	float Lerp(float elapsedTime, float duration, float origin, float endPos);

	int numLight;

	enum OBJSCENE
	{
		GROUND,

		NUM_OBJ
	};

	GameObject* objInscene[NUM_OBJ];

	Button_UI playBtn;
	float playBtnSize;
	float playBtnElapsedTime;
	bool playAnim;

	Button_UI exitBtn;
	float exitBtnSize;
	float exitBtnElapsedTime;
	bool exitAnim;

	bool enableUIs;

	bool playScene;
	float elapsedPlaySceneTime;


//	glm::vec3 devVec = glm::vec3{ 15, 15, 15 };
};

#endif