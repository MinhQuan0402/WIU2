#define _USE_MATH_DEFINES
#include "MainMenu.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "GameObjectManager.h"
#include <glm/gtc/matrix_inverse.hpp>
#include "LoadTGA.h"
#include <GLFW/glfw3.h>
#include "Time.h"
#include "LoadPNG.h"
#include <btBulletDynamicsCommon.h>
#include "CollisionManager.h"
#include <MyMath.h>
#include "Utility.h"
#include "CarnivalHitBoxes.h"
#include "ColliderManager.h"
#include "RigidBody.h"
#include "SceneManager.h"
#include "GameManager.h"
#include "CircusTent.h"
#include "MeshManager.h"
#include "carnivalroaming.h"


MainMenu::MainMenu() : numLight{ 1 }, playBtn{ glm::vec2(Application::m_consoleWidth * 0.5, Application::m_consoleHeight * 0.375), 20 * 3.5 * 3, 20 * 3 }, exitBtn{ glm::vec2(Application::m_consoleWidth * 0.5, Application::m_consoleHeight * 0.275), 20 * 3.5 * 3, 20 * 3 }
{
	meshList.resize(NUM_GEOMETRY);
	lights.resize(numLight);

	playBtn.label = "PLAY";
	exitBtn.label = "EXIT";
}

MainMenu::~MainMenu()
{
}




void MainMenu::Init()
{
	Scene::Init();

	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	//Enable back face culling
	//glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Load the shader programs
	m_programID = LoadShaders("Shader//Texture.vertexshader",
		"Shader//MainShader.fragmentshader");
	glUseProgram(m_programID);

	// Parameters:
	{
		// Get a handle for our "MVP" uniform
		m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
		m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
		m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
		m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
		m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
		m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
		m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

		m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
		m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
		m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
		m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
		m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
		m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
		m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
		m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
		m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
		m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
		m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

		m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
		m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
		m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
		m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
		m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
		m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	}

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);

	// Meshes:
	{
		meshList[GEO_HITBOX] = MeshBuilder::GenerateCube("HitBox", glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
		meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
		meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
		meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
		meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
		meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", WHITE, 1.0f, 100, 100);
		meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", BLACK, 1.0f);

		meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", WHITE, 75.0f);
		meshList[GEO_PLANE]->textureID = LoadPNG("Images//ground.png");

		meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
		meshList[GEO_TOP]->textureID = LoadPNG("Images//top.png");
		meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
		meshList[GEO_BOTTOM]->textureID = LoadPNG("Images//bottom.png");
		meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
		meshList[GEO_RIGHT]->textureID = LoadPNG("Images//front.png");
		meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
		meshList[GEO_LEFT]->textureID = LoadPNG("Images//back.png");
		meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
		meshList[GEO_FRONT]->textureID = LoadPNG("Images//right.png");
		meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
		meshList[GEO_BACK]->textureID = LoadPNG("Images//left.png");

		meshList[GEO_BTNFRAME] = MeshBuilder::GenerateQuad("ButtonFrame", WHITE, 1);
		meshList[GEO_BTNFRAME]->textureID = LoadPNG("Images//ButtonFrame.png");
	}

	// Camera Setup:
	{
		mainCamera.Init(glm::vec3(2.29091, 56.4154, -376.951), glm::vec3(2.29091, 57.4154, -370.951), VECTOR3_UP);
		mainCamera.sensitivity = 20;

		Application::enableCursor = true;
		Application::showCursor = true;
	}

	// Lights:
	{
		glUniform1i(m_parameters[U_NUMLIGHTS], numLight);

		lights[0].m_transform.m_position = glm::vec3(0, 1000, 0);
		lights[0].color = glm::vec3(1, 1, 1);
		lights[0].type = Light::LIGHT_DIRECTIONAL;
		lights[0].power = 1.0f;
		lights[0].kC = 1.f;
		lights[0].kL = 0.01f;
		lights[0].kQ = 0.001f;
		lights[0].cosCutoff = 150.f;
		lights[0].cosInner = 45.f;
		lights[0].exponent = 3.f;
		lights[0].spotDirection = glm::vec3(0.f, -1.f, 0.f);

		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
		glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
		glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
		glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
		glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
		glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
		glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], cosf(glm::radians<float>(lights[0].cosCutoff)));
		glUniform1f(m_parameters[U_LIGHT0_COSINNER], cosf(glm::radians<float>(lights[0].cosInner)));
		glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

		enableLight = true;
	}

	PhysicsMaterial mat;
	mat.m_mass = 1.0f;

	PhysicsMaterial groundMat;
	groundMat.m_friction = 0.5f;
	objInscene[GROUND] = new GameObject();
	objInscene[GROUND]->rb = addStaticPlane(objInscene[GROUND], VECTOR3_UP, groundMat);
	GameObjectManager::GetInstance()->addItem(objInscene[GROUND]);

	GameObjectManager::GetInstance()->IniAll();

	enableUIs = true;
	playScene = false;
	elapsedPlaySceneTime = 0;

	playAnim = false;
	playBtnSize = 1;
	playBtnElapsedTime = 0;

	exitAnim = false;
	exitBtnSize = 1;
	exitBtnElapsedTime = 0;
}

void MainMenu::Update()
{
	HandleKeyPress();
	mainCamera.Update();
	glm::vec3 inputMovementDir{};
	/*if (KeyboardController::GetInstance()->IsKeyDown('W'))
		inputMovementDir = mainCamera.view;
	if (KeyboardController::GetInstance()->IsKeyDown('S'))
		inputMovementDir = -mainCamera.view;
	if (KeyboardController::GetInstance()->IsKeyDown('D'))
		inputMovementDir = mainCamera.right;
	if (KeyboardController::GetInstance()->IsKeyDown('A'))
		inputMovementDir = -mainCamera.right;*/

	glm::vec3 finalForce = inputMovementDir * 50.0f * Time::deltaTime;
	//mainCamera.m_transform.Translate(finalForce);
	glm::vec3 prevTarget = mainCamera.target;
	//mainCamera.UpdateCameraRotation();

	//std::cout << mainCamera.m_transform.m_position.x << ", " << mainCamera.m_transform.m_position.y << ", " << mainCamera.m_transform.m_position.z << std::endl;

	GameObjectManager::GetInstance()->UpdateAll();


	if (enableUIs) {

		playBtn.position = glm::vec2(Application::m_consoleWidth * 0.5, Application::m_consoleHeight * 0.375);
		playBtn.width = Application::m_consoleWidth / 5.71f;
		playBtn.height = Application::m_consoleHeight / 11.25f;

		exitBtn.position = glm::vec2(Application::m_consoleWidth * 0.5, Application::m_consoleHeight * 0.275);
		exitBtn.width = Application::m_consoleWidth / 5.71f;
		exitBtn.height = Application::m_consoleHeight / 11.25f;

		float mouseX = MouseController::GetInstance()->GetMousePositionX();
		float mouseY = Application::m_consoleHeight - MouseController::GetInstance()->GetMousePositionY();

		float duration = 0.1;

		if (playAnim) {
			if (playBtnElapsedTime < duration) {
				playBtnElapsedTime += Time::deltaTime;
				playBtnSize = Lerp(playBtnElapsedTime, duration, 1, 1.2);
				playBtn.meshScaleX = Lerp(playBtnElapsedTime, duration, 0, 1);
			}
			else {
				playBtnElapsedTime = duration;
				playBtn.meshScaleX = 1;
				playBtnSize = 1.2;
			}
		}
		else {
			if (playBtnElapsedTime > 0) {
				playBtnElapsedTime -= Time::deltaTime;
				playBtnSize = Lerp(playBtnElapsedTime, duration, 1, 1.2);
				playBtn.meshScaleX = Lerp(playBtnElapsedTime, duration, 0, 1);
			}
			else {
				playBtnElapsedTime = 0;
				playBtn.meshScaleX = 0;
				playBtnSize = 1;
			}
		}

		if (playBtn.IsTouching(mouseX, mouseY)) {
			playAnim = true;
		}
		else {
			playAnim = false;
		}

		if (exitAnim) {
			if (exitBtnElapsedTime < duration) {
				exitBtnElapsedTime += Time::deltaTime;
				exitBtnSize = Lerp(exitBtnElapsedTime, duration, 1, 1.2);
				exitBtn.meshScaleX = Lerp(exitBtnElapsedTime, duration, 0, 1);
			}
			else {
				exitBtnElapsedTime = duration;
				exitBtn.meshScaleX = 1;
				exitBtnSize = 1.2;
			}
		}
		else {
			if (exitBtnElapsedTime > 0) {
				exitBtnElapsedTime -= Time::deltaTime;
				exitBtnSize = Lerp(exitBtnElapsedTime, duration, 1, 1.2);
				exitBtn.meshScaleX = Lerp(exitBtnElapsedTime, duration, 0, 1);
			}
			else {
				exitBtnElapsedTime = 0;
				exitBtn.meshScaleX = 0;
				exitBtnSize = 1;
			}
		}

		if (exitBtn.IsTouching(mouseX, mouseY)) {
			exitAnim = true;
		}
		else {
			exitAnim = false;
		}

		if (MouseController::GetInstance()->IsButtonReleased(0)) {
			if (playBtn.IsTouching(mouseX, mouseY)) {
				enableUIs = false;
				playScene = true;

				Application::enableCursor = false;
				Application::showCursor = false;
			}
			else if (exitBtn.IsTouching(mouseX, mouseY)) {
				enableUIs = false;
				exit(0);
			}
		}
	}
	

}

void MainMenu::LateUpdate()
{
	// Developer Controls:
	{
		//float speed = 20;
		//if (KeyboardController::GetInstance()->IsKeyDown('Y')) {
		//	devVec.y += speed * Time::deltaTime;
		//}
		//if (KeyboardController::GetInstance()->IsKeyDown('U')) {
		//	devVec.y -= speed * Time::deltaTime;
		//}
		//if (KeyboardController::GetInstance()->IsKeyDown('T')) {
		//	devVec.z += speed * Time::deltaTime;
		//}
		//if (KeyboardController::GetInstance()->IsKeyDown('G')) {
		//	devVec.z -= speed * Time::deltaTime;
		//}
		//if (KeyboardController::GetInstance()->IsKeyDown('F')) {
		//	devVec.x += speed * Time::deltaTime;
		//}
		//if (KeyboardController::GetInstance()->IsKeyDown('H')) {
		//	devVec.x -= speed * Time::deltaTime;
		//}

		//std::cout << devVec.x << ", " << devVec.y << ", " << devVec.z << std::endl;

		//lights[0].m_transform.m_position = devVec;

		GameObjectManager::GetInstance()->UpdateAll();

		if (playScene) {
			float duration = 5;
			if (elapsedPlaySceneTime < duration) {
				elapsedPlaySceneTime += Time::deltaTime;

				mainCamera.m_transform.m_position = Lerp(elapsedPlaySceneTime, duration, glm::vec3(2.29091, 56.4154, -376.951), glm::vec3(2.61806, 44.6011, -220.505));
				mainCamera.target = Lerp(elapsedPlaySceneTime, duration, glm::vec3(2.29091, 57.4154, -370.951), glm::vec3(2.61806, 44.6011, -217.505));
			}
			else {
				// Change scenes:
				SceneManager::GetInstance()->ChangeState(new carnivalroaming);
			}
		}

	}
}

void MainMenu::Render()
{

	{
		// Clear color buffer every frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 model = glm::mat4(1.0f);
		// Setup Model View Projection matrix
		projectionStack.LoadMatrix(glm::perspective(45.0f,
			Application::m_consoleWidth / (float)Application::m_consoleHeight,
			0.1f, 100000.0f));

		viewStack.LoadIdentity();
		viewStack.LookAt(
			mainCamera.m_transform.m_position,
			mainCamera.target,
			mainCamera.up
		);
		// Load identity matrix into the model stack
		modelStack.LoadIdentity();
	
		if (lights[0].type == Light::LIGHT_DIRECTIONAL)
		{
			glm::vec3 lightDir(lights[0].m_transform.m_position.x, lights[0].m_transform.m_position.y, lights[0].m_transform.m_position.z);
			glm::vec3 lightDirection_cameraspace = viewStack.Top() * glm::vec4(lightDir, 0);
			glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightDirection_cameraspace));
		}
		else if (lights[0].type == Light::LIGHT_SPOT)
		{
			glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(lights[0].m_transform.m_position, 1);
			glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
			glm::vec3 spotDirection_cameraspace = viewStack.Top() * glm::vec4(lights[0].spotDirection, 0);
			glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, glm::value_ptr(spotDirection_cameraspace));
		}
		else {
			// Calculate the light position in camera space
			glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(lights[0].m_transform.m_position, 1);
			glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
		}

		modelStack.PushMatrix();
		//RenderMesh(meshList[GEO_AXIS]);
		modelStack.PopMatrix();
	}

	// Render circus:
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 10.5f, 0.0f);
		modelStack.Scale(45, 45, 45);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CIRCUSTENT], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 0.5f, 0.0f);
		modelStack.Scale(45, 45, 45);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CIRCUSNAME], true);
		modelStack.PopMatrix();
	}

	// Render white cube inside circus:
	{
		modelStack.PushMatrix();
		modelStack.Translate(3.1979, 52.3335, -177.72);
		modelStack.Scale(95.2634, 121.109, 15);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
	}


	RenderSkybox();
	RenderGround(20);

	// Render ui:
	{
		if (enableUIs) {
			playBtn.RenderButton(*this, GEO_BTNFRAME, GEO_TEXT, WHITE, Application::m_consoleHeight / 22.5 * playBtnSize, glm::vec2(-(Application::m_consoleHeight / 22.5 * playBtnSize), -(Application::m_consoleHeight / 22.5 * 0.5 * playBtnSize)));
			exitBtn.RenderButton(*this, GEO_BTNFRAME, GEO_TEXT, WHITE, Application::m_consoleHeight / 22.5 * exitBtnSize, glm::vec2(-(Application::m_consoleHeight / 22.5 * exitBtnSize), -(Application::m_consoleHeight / 22.5 * 0.5 * playBtnSize)));
		}
	}

#ifdef DRAW_HITBOX
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (btCollisionShape* shape : ColliderManager::GetInstance()->colliders)
	{
		modelStack.PushMatrix();
		modelStack.LoadIdentity();
		GameObject* userGO = static_cast<GameObject*>(shape->getUserPointer());
		modelStack.LoadMatrix(GetTransformMatrix(userGO->rb));
		if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
		{
			SphereCollider* sphereCollider = static_cast<SphereCollider*>(shape);
			float size = sphereCollider->GetRadius();
			modelStack.Scale(size, size, size);
			RenderMesh(hitboxMeshList[HITBOX_SPHERE]);
		}
		else if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE)
		{
			BoxCollider* boxCollider = static_cast<BoxCollider*>(shape);
			float width, height, depth;
			boxCollider->GetDimension(width, height, depth);
			modelStack.Scale(width, height, depth);
			RenderMesh(hitboxMeshList[HITBOX_BOX]);
		}
		else if (shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
		{
			CylinderCollider* cylinderCollider = static_cast<CylinderCollider*>(shape);
			float rad, height;
			cylinderCollider->GetDimension(rad, height);
			modelStack.Scale(rad / 2.0f, height, rad / 2.0f);
			RenderMesh(hitboxMeshList[HITBOX_CYLINDER]);
		}
		else if (shape->getShapeType() == STATIC_PLANE_PROXYTYPE)
		{
			modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
			RenderMesh(hitboxMeshList[HITBOX_GROUND]);
		}
		modelStack.PopMatrix();
	}

	if (isFillMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

void MainMenu::Exit()
{
	Scene::Exit(); 
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();
	ColliderManager::GetInstance()->ClearAll();

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void MainMenu::RenderSkybox(void)
{
	float size = 400.0f;
	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.f, 0.f, -4.99f * size);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.f, 0.f, 4.99f * size);
	modelStack.Rotate(180.0f, 0, 1, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(4.99f * size, 0.f, 0.0f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(-4.99f * size, 0.f, 0.0f);
	modelStack.Rotate(90.0f, 0, 1, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.0f, 4.99f * size, 0.0f);
	modelStack.Rotate(90.0f, 1, 0, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.0f, -4.99f * size, 0.0f);
	modelStack.Rotate(-90.0f, 1, 0, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();
}

void MainMenu::RenderGround(int size)
{
	if (size % 2 == 0) size += 1;

	int valueToMul = (size - 1) / 2;
	glm::vec3 originPos = glm::vec3{ 75.0f * (float)valueToMul, 0.0f, 75.0f * (float)valueToMul };
	float orignalX = originPos.x;
	meshList[GEO_PLANE]->material = Material::Wood(WHITE);
	for (int i = 0; i < size; i++)
	{
		originPos.x = orignalX;
		for (int j = 0; j < size; ++j)
		{

			modelStack.PushMatrix();
			modelStack.Translate(originPos.x, originPos.y, originPos.z);
			modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
			RenderMesh(meshList[GEO_PLANE], enableLight);
			modelStack.PopMatrix();

			originPos.x -= 75.0f;
		}

		originPos.z -= 75.0f;
	}
}

glm::vec3 MainMenu::Lerp(float elapsedTime, float duration, glm::vec3 origin, glm::vec3 endPos)
{
	glm::vec3 result = endPos;
	if (elapsedTime <= duration) {
		float t = elapsedTime / duration;
		t = 0.5f * (1 - cos(t * glm::pi<float>())); // Apply sine curve easing
		result = origin + (endPos - origin) * t;
	}

	return result;
}

float MainMenu::Lerp(float elapsedTime, float duration, float origin, float endPos)
{
	float result = endPos;
	if (elapsedTime <= duration) {
		float t = elapsedTime / duration;
		t = 0.5f * (1 - cos(t * glm::pi<float>())); // Apply sine curve easing
		result = origin + (endPos - origin) * t;
	}

	return result;
}
