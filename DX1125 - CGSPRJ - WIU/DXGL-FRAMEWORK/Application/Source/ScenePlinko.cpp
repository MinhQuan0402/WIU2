#define _USE_MATH_DEFINES
#include "ScenePlinko.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "cmath"
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
#include "ColliderManager.h"
#include <MyMath.h>
#include "Utility.h"
#include "PlinkoBall.h"
#include "PlinkoCylinder.h"
#include "PlinkoWall.h"
#include "PlinkoBoard.h"
#include "GameManager.h"
#include "SceneManager.h"
#include "carnivalroaming.h"
#include "MeshManager.h"

ScenePlinko::ScenePlinko() : numLight{ 12 }, ballZ{ 0.0f }, currentBallIndex{ BALL1 }, plinkoScore{ 0 }
{
	meshList.resize(NUM_GEOMETRY);
	lights.resize(numLight);
}

ScenePlinko::~ScenePlinko()
{
}

void ScenePlinko::Init()
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

	// Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);

	for (int i = 0; i < numLight; ++i)
	{
		std::string index = std::to_string(i);
		std::string typeString = "lights[" + index + "].type";
		std::string positionString = "lights[" + index + "].position_cameraspace";
		std::string colorString = "lights[" + index + "].color";
		std::string powerString = "lights[" + index + "].power";
		std::string kCString = "lights[" + index + "].kC";
		std::string kLString = "lights[" + index + "].kL";
		std::string kQString = "lights[" + index + "].kQ";
		std::string spotString = "lights[" + index + "].spotDirection";
		std::string cosCutOffString = "lights[" + index + "].cosCutoff";
		std::string cosInnerString = "lights[" + index + "].cosInner";
		std::string exponentString = "lights[" + index + "].exponent";

		m_parameters[U_LIGHT0_TYPE + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, typeString.data());
		m_parameters[U_LIGHT0_POSITION + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, positionString.data());
		m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, colorString.data());
		m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, powerString.data());
		m_parameters[U_LIGHT0_KC + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, kCString.data());
		m_parameters[U_LIGHT0_KL + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, kLString.data());
		m_parameters[U_LIGHT0_KQ + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, kQString.data());
		m_parameters[U_LIGHT0_SPOTDIRECTION + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, spotString.data());
		m_parameters[U_LIGHT0_COSCUTOFF + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, cosCutOffString.data());
		m_parameters[U_LIGHT0_COSINNER + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, cosInnerString.data());
		m_parameters[U_LIGHT0_EXPONENT + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, exponentString.data());
	}

	meshList[GEO_HITBOX] = MeshBuilder::GenerateCube("HitBox", glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_MODEL1] = MeshBuilder::GenerateOBJ("Doorman", "Models//doorman.obj");
	meshList[GEO_MODEL1]->textureID = LoadPNG("Images//doorman.png");
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", YELLOW, 0.1f, 100, 100);
	meshList[GEO_SPHERE]->material.kDiffuse = YELLOW;
	meshList[GEO_SPHERE]->material.kAmbient = GREY;
	meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.1, 0.1, 0.1);
	meshList[GEO_SPHERE]->material.kShininess = 1;

	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", WHITE, 1.0f);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", GREEN, 75.0f);
	meshList[GEO_PLANE]->textureID = LoadPNG("Images//ground.png");

	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("Cylinder", WHITE, 100, 1.f, 2.f);
	
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

	mainCamera.Init(glm::vec3(7, 4, 0), glm::vec3(0, 4, 0), VECTOR3_UP);
	
	mainCamera.sensitivity = 15.f;

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	//lights[0].m_transform.m_position = glm::vec3{};
	lights[0].m_transform.m_position = glm::vec3(0, 10, 0);
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_POINT;
	lights[0].power = 0.8f;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = 45.f;
	lights[0].cosInner = 30.f;
	lights[0].exponent = 3.f;
	lights[0].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], cosf(glm::radians<float>(lights[0].cosCutoff)));
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], cosf(glm::radians<float>(lights[0].cosInner)));
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	lights[1].m_transform.m_position = glm::vec3(0.3, 7.5, 0);
	lights[1].color = glm::vec3(1, 1, 0);
	lights[1].type = Light::LIGHT_SPOT;
	lights[1].power = 1.f;
	lights[1].kC = 1.f;
	lights[1].kL = 0.01f;
	lights[1].kQ = 0.001f;
	lights[1].cosCutoff = 45.f;
	lights[1].cosInner = 30.f;
	lights[1].exponent = 3.f;
	lights[1].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &lights[1].color.r);
	glUniform1i(m_parameters[U_LIGHT1_TYPE], lights[1].type);
	glUniform1f(m_parameters[U_LIGHT1_POWER], lights[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], lights[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], lights[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], lights[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], cosf(glm::radians<float>(lights[1].cosCutoff)));
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], cosf(glm::radians<float>(lights[1].cosInner)));
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], lights[1].exponent);

	for (int i = 2; i < numLight; ++i)
	{
		lights[i].color = glm::vec3(1, 1, 1);
		lights[i].type = Light::LIGHT_POINT;
		lights[i].power = 0.35f;
		lights[i].kC = 0.5f;
		lights[i].kL = 0.01f;
		lights[i].kQ = 0.001f;
		lights[i].cosCutoff = 45.f;
		lights[i].cosInner = 30.f;
		lights[i].exponent = 3.f;
		lights[i].spotDirection = glm::vec3(0.f, 1.f, 0.f);
	}

	for (int i = 2; i < numLight; ++i)
	{
		glUniform1i(m_parameters[U_LIGHT0_TYPE + U_LIGHT0_EXPONENT * i], lights[i].type);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * i], 1, &lights[i].color.r);
		glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
		glUniform1f(m_parameters[U_LIGHT0_KC + U_LIGHT0_EXPONENT * i], lights[i].kC);
		glUniform1f(m_parameters[U_LIGHT0_KL + U_LIGHT0_EXPONENT * i], lights[i].kL);
		glUniform1f(m_parameters[U_LIGHT0_KQ + U_LIGHT0_EXPONENT * i], lights[i].kQ);
		glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF + U_LIGHT0_EXPONENT * i], cosf(glm::radians<float>(lights[i].cosCutoff)));
		glUniform1f(m_parameters[U_LIGHT0_COSINNER + U_LIGHT0_EXPONENT * i], cosf(glm::radians<float>(lights[i].cosInner)));
		glUniform1f(m_parameters[U_LIGHT0_EXPONENT + U_LIGHT0_EXPONENT * i], lights[i].exponent);
	}

	enableLight = true;

	// Create object in scene
	objInScene[PLINKO] = new PlinkoBoard();
	objInScene[BALL1] = new PlinkoBall();

	GameObjectManager::GetInstance()->IniAll();
}

void ScenePlinko::Update()
{
	HandleKeyPress();
	mainCamera.Update();
	glm::vec3 inputMovementDir{};
	if (KeyboardController::GetInstance()->IsKeyDown('W'))
		inputMovementDir = mainCamera.view;
	if (KeyboardController::GetInstance()->IsKeyDown('S'))
		inputMovementDir = -mainCamera.view;
	if (KeyboardController::GetInstance()->IsKeyDown('D'))
		inputMovementDir = mainCamera.right;
	if (KeyboardController::GetInstance()->IsKeyDown('A'))
		inputMovementDir = -mainCamera.right;
	glm::vec3 finalForce = inputMovementDir * 10.0f * Time::deltaTime;
	mainCamera.m_transform.Translate(finalForce);
	mainCamera.UpdateCameraRotation();

	if (KeyboardController::GetInstance()->IsKeyPressed('P'))
	{
		if (lights[1].power == 1.f) {
			lights[1].power = 0;
		}
		else {
			lights[1].power = 1.f;
		}
		glUniform1f(m_parameters[U_LIGHT1_POWER], lights[1].power);
	}

	GameObjectManager::GetInstance()->UpdateAll();
}

void ScenePlinko::LateUpdate()
{
	mainCamera.Update();
	glm::vec3 ballPosition = objInScene[currentBallIndex]->GetRigidbodyPosition();

	// Drop ball
	if (isOnHold)
	{
		if (KeyboardController::GetInstance()->IsKeyDown('J')) // Left
		{
			ballZ += Time::deltaTime * 2.f;
		}
		if (KeyboardController::GetInstance()->IsKeyDown('L')) // Right
		{
			ballZ -= Time::deltaTime * 2.f;
		}

		if (KeyboardController::GetInstance()->IsKeyDown(VK_SPACE))
		{
			objInScene[currentBallIndex]->rb->activate();
			isOnHold = false;
			cameraFollow = true;
			addScore = true;
			jackpot = false;
		}

		ballZ = Math::Clamp(ballZ, -1.2f, 1.2f);
		objInScene[currentBallIndex]->SetRigidbodyPosition(0.0f, 6.0f, ballZ);
		objInScene[currentBallIndex]->rb->clearGravity();
	}
	else
	{
		if (cameraFollow) {
			mainCamera.target = ballPosition;
			mainCamera.UpdateCameraPosition(2.5, ballPosition.y, ballPosition.z);
		}

		if (objInScene[currentBallIndex]->rb->getActivationState() == ISLAND_SLEEPING && currentBallIndex <= 2)
		{
			cameraFollow = false;
			mainCamera.target = glm::vec3(0, 4, 0);
			mainCamera.UpdateCameraPosition(7, 4, 0);

			// Win
			if (currentBallIndex == BALL3) {
				gameEnd = true;
			}

			if (currentBallIndex < 2) {
				isOnHold = true;
				objInScene[++currentBallIndex] = new PlinkoBall();
				objInScene[currentBallIndex]->Start();
			}
		}
	}

	// Score counter
	if (ballPosition.y < 1.6) {
		if (ballPosition.z < 1.35 && ballPosition.z > 1.05) { // 10 points
			if (addScore) {
				plinkoScore += 10;
				addScore = false;
			}
		}
		else if (ballPosition.z < 1.05 && ballPosition.z > 0.75) { // 20 points
			if (addScore) {
				plinkoScore += 20;
				addScore = false;
			}
		}
		else if (ballPosition.z < 0.75 && ballPosition.z > 0.45) { // 30 points
			if (addScore) {
				plinkoScore += 30;
				addScore = false;
			}
		}
		else if (ballPosition.z < 0.45 && ballPosition.z > 0.15) { // 50 points
			if (addScore) {
				plinkoScore += 50;
				addScore = false;
			}
		}
		else if (ballPosition.z < 0.15 && ballPosition.z > -0.15) { // 100 points
			if (addScore) {
				plinkoScore += 100;
				jackpot = true;
				addScore = false;
			}
		}
		else if (ballPosition.z < -0.15 && ballPosition.z > -0.45) { // 50 points
			if (addScore) {
				plinkoScore += 50;
				addScore = false;
			}
		}
		else if (ballPosition.z < -0.45 && ballPosition.z > -0.75) { // 30 points
			if (addScore) {
				plinkoScore += 30;
				addScore = false;
			}
		}
		else if (ballPosition.z < -0.75 && ballPosition.z > -1.05) { // 20 points
			if (addScore) {
				plinkoScore += 20;
				addScore = false;
			}
		}
		else if (ballPosition.z < -1.05 && ballPosition.z > -1.35) { // 10 points
			if (addScore) {
				plinkoScore += 10;
				addScore = false;
			}
		}
	}
}

void ScenePlinko::Render()
{
	// Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 model = glm::mat4(1.0f);
	// Setup Model View Projection matrix
	projectionStack.LoadMatrix(glm::perspective(45.0f,
		Application::m_consoleWidth / (float)Application::m_consoleHeight,
		0.1f, 1000.0f));

	viewStack.LoadIdentity();
	viewStack.LookAt(
		mainCamera.m_transform.m_position,
		mainCamera.target,
		mainCamera.up
	);
	// Load identity matrix into the model stack
	modelStack.LoadIdentity();

	for (int i = 0; i < numLight; ++i)
	{
		if (lights[i].type == Light::LIGHT_DIRECTIONAL)
		{
			glm::vec3 lightDir(lights[i].m_transform.m_position.x, lights[i].m_transform.m_position.y, lights[i].m_transform.m_position.z);
			glm::vec3 lightDirection_cameraspace = viewStack.Top() * glm::vec4(lightDir, 0);
			glUniform3fv(m_parameters[U_LIGHT0_POSITION + U_LIGHT0_EXPONENT * i], 1, glm::value_ptr(lightDirection_cameraspace));
		}
		else if (lights[i].type == Light::LIGHT_SPOT)
		{
			glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(lights[i].m_transform.m_position, 1);
			glUniform3fv(m_parameters[U_LIGHT0_POSITION + U_LIGHT0_EXPONENT * i], 1, glm::value_ptr(lightPosition_cameraspace));
			glm::vec3 spotDirection_cameraspace = viewStack.Top() * glm::vec4(lights[i].spotDirection, 0);
			glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION + U_LIGHT0_EXPONENT * i], 1, glm::value_ptr(spotDirection_cameraspace));
		}
		else {
			// Calculate the light position in camera space
			glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(lights[i].m_transform.m_position, 1);
			glUniform3fv(m_parameters[U_LIGHT0_POSITION + U_LIGHT0_EXPONENT * i], 1, glm::value_ptr(lightPosition_cameraspace));
		}
	}

	RenderSkybox();
	RenderGround(7);

	for (int i = 0; i < numLight; ++i) 
	{
		modelStack.PushMatrix();
		modelStack.Translate(lights[i].m_transform.m_position.x, lights[i].m_transform.m_position.y, lights[i].m_transform.m_position.z);
		RenderMesh(meshList[GEO_LIGHT], false);
		modelStack.PopMatrix();
	}

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_AXIS]);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_PLANE], true);
	modelStack.PopMatrix();

	GameObjectManager::GetInstance()->RenderAll(*this); 

	modelStack.PushMatrix();
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHROOF], enableLight);
	RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHGUARDS], enableLight);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHROOF]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHROOF]->material.kDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHROOF]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHROOF]->material.kShininess = 2.0f;
	MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHGUARDS]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHGUARDS]->material.kDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHGUARDS]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	MeshManager::GetInstance()->meshList[MeshManager::GEO_PLINKO_BOOTHGUARDS]->material.kShininess = 2.0f;
	modelStack.PopMatrix();

	// Points UI
	{
		// 10
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.2, 1.2);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "1", WHITE);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1, 1.2);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", WHITE);
		modelStack.PopMatrix();

		// 20
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.2, 0.9);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "2", glm::vec3(1, 1, 0.75));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1, 0.9);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", glm::vec3(1, 1, 0.75));
		modelStack.PopMatrix();

		// 30
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.2, 0.6);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "3", glm::vec3(1, 1, 0.5));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1, 0.6);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", glm::vec3(1, 1, 0.5));
		modelStack.PopMatrix();

		// 50
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.2, 0.3);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "5", glm::vec3(1, 1, 0.25));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1, 0.3);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", glm::vec3(1, 1, 0.25));
		modelStack.PopMatrix();

		// 100
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.25, 0);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "1", YELLOW);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1.05, 0);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", YELLOW);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 0.85, 0);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", YELLOW);
		modelStack.PopMatrix();

		// 10
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.2, -1.2);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "1", WHITE);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1, -1.2);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", WHITE);
		modelStack.PopMatrix();

		// 20
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.2, -0.9);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "2", glm::vec3(1, 1, 0.75));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1, -0.9);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", glm::vec3(1, 1, 0.75));
		modelStack.PopMatrix();

		// 30
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.2, -0.6);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "3", glm::vec3(1, 1, 0.5));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1, -0.6);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", glm::vec3(1, 1, 0.5));
		modelStack.PopMatrix();

		// 50
		modelStack.PushMatrix();
		modelStack.Translate(0, 1.2, -0.3);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "5", glm::vec3(1, 1, 0.25));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1, -0.3);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderText(meshList[GEO_TEXT], "0", glm::vec3(1, 1, 0.25));
		modelStack.PopMatrix();
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "Score:" + std::to_string(plinkoScore), RED, 40, 980, 600);
	RenderTextOnScreen(meshList[GEO_TEXT], "CONTROLS:", YELLOW, 40, 70, 430);
	RenderTextOnScreen(meshList[GEO_TEXT], "[J] to move LEFT", YELLOW, 30, 20, 400);
	RenderTextOnScreen(meshList[GEO_TEXT], "[L] to move RIGHT", YELLOW, 30, 20, 370);
	RenderTextOnScreen(meshList[GEO_TEXT], "[SPACE] to DROP ball", YELLOW, 30, 20, 340);

	if (gameEnd) {
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU WIN!!!", GREEN, 80, 400, 400);
		SceneManager::GetInstance()->ChangeState(new carnivalroaming);
		return;
	}
	
//#ifdef DRAW_HITBOX
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	for (btCollisionShape* shape : ColliderManager::GetInstance()->colliders)
//	{
//		modelStack.PushMatrix();
//		modelStack.LoadIdentity();
//		GameObject* userGO = static_cast<GameObject*>(shape->getUserPointer());
//		modelStack.LoadMatrix(GetTransformMatrix(userGO->rb));
//		if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
//		{
//			SphereCollider* sphereCollider = static_cast<SphereCollider*>(shape);
//			float size = sphereCollider->GetRadius();
//			modelStack.Scale(size, size, size);
//			RenderMesh(hitboxMeshList[HITBOX_SPHERE]);
//		}
//		else if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE)
//		{
//			BoxCollider* boxCollider = static_cast<BoxCollider*>(shape);
//			float width, height, depth;
//			boxCollider->GetDimension(width, height, depth);
//			modelStack.Scale(width, height, depth);
//			RenderMesh(hitboxMeshList[HITBOX_BOX]);
//		}
//		else if (shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
//		{
//			CylinderCollider* cylinderCollider = static_cast<CylinderCollider*>(shape);
//			float rad, height;
//			cylinderCollider->GetDimension(rad, height);
//			modelStack.Scale(rad / 2.0f, height, rad / 2.0f);
//			RenderMesh(hitboxMeshList[HITBOX_CYLINDER]);
//		}
//		else if (shape->getShapeType() == STATIC_PLANE_PROXYTYPE)
//		{
//			modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
//			RenderMesh(hitboxMeshList[HITBOX_GROUND]);
//		}
//		modelStack.PopMatrix();
//	}
//
//	if (isFillMode)
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//#endif
}

void ScenePlinko::Exit()
{
	GameManager::GetInstance()->SetPoints(GameManager::GetInstance()->GetPoints() + plinkoScore);
	Scene::Exit(); 
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();
	ColliderManager::GetInstance()->ClearAll(); 

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void ScenePlinko::RenderSkybox(void)
{
	float size = 50.0f;
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

void ScenePlinko::RenderGround(int size)
{
	if (size % 2 == 0) size += 1;

	int valueToMul = (size - 1) / 2;
	glm::vec3 originPos = glm::vec3{ 75.0f * (float)valueToMul, 0.0f, 75.0f * (float)valueToMul };
	float orignalX = originPos.x;
	MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_PLANE]->material = Material::Wood(WHITE);
	for (int i = 0; i < size; i++)
	{
		originPos.x = orignalX;
		for (int j = 0; j < size; ++j)
		{

			modelStack.PushMatrix();
			modelStack.Translate(originPos.x, originPos.y, originPos.z);
			modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_PLANE], enableLight);
			modelStack.PopMatrix();

			originPos.x -= 75.0f;
		}

		originPos.z -= 75.0f;
	}
}