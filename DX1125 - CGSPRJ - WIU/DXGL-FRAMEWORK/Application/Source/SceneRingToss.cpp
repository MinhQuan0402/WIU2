#define _USE_MATH_DEFINES
#include "SceneRingToss.h"
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
#include <MyMath.h>
#include "Utility.h"
#include "RigidBody.h"
#include "ColliderManager.h"
#include "TossBoard.h"
#include "MeshManager.h"
#include "carnivalroaming.h"
#include "SceneManager.h"

SceneRingToss::SceneRingToss()
	: numLight{ 25 }, isShoot{ false }, isPickable{ true },
	currentIndexRing{ 0 }, lightTimer{ 0.0f }, isInView{ false },
	points{ 0 }, numAttempt{ 0 }
{
	meshList.resize(NUM_GEOMETRY);
	lights.resize(numLight);
}

SceneRingToss::~SceneRingToss()
{
}

void SceneRingToss::Init()
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
	m_parameters[U_MATERIAL_EMISSION] = glGetUniformLocation(m_programID, "material.kEmission");

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

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
		m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);

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

	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, 0.5f, 50, 50);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	
	/*meshList[GEO_BOARD] = MeshBuilder::GenerateOBJ("board", "Models//ring_toss_rack.obj");
	meshList[GEO_BOARD]->textureID = LoadPNG("Images//wood.png");
	meshList[GEO_BOTTLE] = MeshBuilder::GenerateOBJ("Bottle", "Models//ring_toss_bottle.obj");
	meshList[GEO_RING] = MeshBuilder::GenerateOBJ("Ring", "Models//ring_toss_rope.obj");
	meshList[GEO_RING]->textureID = LoadPNG("Images//rope.png");
	meshList[GEO_TIE] = MeshBuilder::GenerateOBJ("Tie", "Models//ring_toss_tie.obj");
	meshList[GEO_TIE]->textureID = LoadPNG("Images//tie.png");
	meshList[GEO_TABLE] = MeshBuilder::GenerateOBJ("Table", "Models//ring_toss_table.obj");
	meshList[GEO_TABLE]->textureID = LoadPNG("Images//wood.png");
	meshList[GEO_TABLECLOTH] = MeshBuilder::GenerateOBJ("Table cloth", "Models//ring_toss_tablecloth.obj");
	meshList[GEO_TABLECLOTH]->textureID = LoadPNG("Images//tablecloth.png");
	meshList[GEO_COUNTER] = MeshBuilder::GenerateOBJ("Counter", "Models//ring_toss_counter.obj");
	meshList[GEO_COUNTER]->textureID = LoadPNG("Images//counter.png");
	meshList[GEO_LIGHTPOLE] = MeshBuilder::GenerateOBJMTL("Light pole", "Models//ring_toss_lightpole.obj", "Models//ring_toss_lightpole.mtl");

	meshList[GEO_BOARD]
	meshList[GEO_BOARD]->material = Material::Wood(WHITE);*/

	mainCamera.Init(glm::vec3(0, 10, 20.0f), glm::vec3(0, 10, 0), VECTOR3_UP);
	mainCamera.constraintYaw = 0.5f;
	
	glUniform1i(m_parameters[U_NUMLIGHTS], numLight);

	lights[0].m_transform.m_position = glm::vec3(60.0f, 60.0f, -30.0f);
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].power = 2.0f;
	lights[0].kC = 0.5f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = 45.f;
	lights[0].cosInner = 30.f;
	lights[0].exponent = 3.f;

	lights[1].m_transform.m_position = glm::vec3(6.f, 11.75f, -11.85f);
	lights[2].m_transform.m_position = glm::vec3(5.8f, 10.75f, -9.9f);
	lights[3].m_transform.m_position = glm::vec3(5.8f, 9.85f, -7.65f);
	lights[4].m_transform.m_position = glm::vec3(6.1f, 9.2f, -5.75f);
	lights[5].m_transform.m_position = glm::vec3(6.2f, 8.75f, -3.5f);
	lights[6].m_transform.m_position = glm::vec3(6.2f, 8.55f, -1.25f);
	lights[7].m_transform.m_position = glm::vec3(6.0f, 8.25f, 1.25f);
	lights[8].m_transform.m_position = glm::vec3(6.0f, 8.55f, 4.f);
	lights[9].m_transform.m_position = glm::vec3(5.9f, 8.9f, 6.2f);
	lights[10].m_transform.m_position = glm::vec3(5.95f, 9.5f, 8.45f);
	lights[11].m_transform.m_position = glm::vec3(5.9f, 10.25f, 10.5f);
	lights[12].m_transform.m_position = glm::vec3(5.875f, 11.25f, 12.375f);
	lights[13].m_transform.m_position = glm::vec3(-6.f, 11.75f, -11.85f);
	lights[14].m_transform.m_position = glm::vec3(-5.8f, 10.75f, -9.9f);
	lights[15].m_transform.m_position = glm::vec3(-5.8f, 9.85f, -7.65f);
	lights[16].m_transform.m_position = glm::vec3(-6.1f, 9.2f, -5.75f);
	lights[17].m_transform.m_position = glm::vec3(-6.2f, 8.75f, -3.5f);
	lights[18].m_transform.m_position = glm::vec3(-6.2f, 8.55f, -1.25f);
	lights[19].m_transform.m_position = glm::vec3(-6.0f, 8.25f, 1.25f);
	lights[20].m_transform.m_position = glm::vec3(-6.0f, 8.55f, 4.f);
	lights[21].m_transform.m_position = glm::vec3(-5.9f, 8.9f, 6.2f);
	lights[22].m_transform.m_position = glm::vec3(-5.95f, 9.5f, 8.45f);
	lights[23].m_transform.m_position = glm::vec3(-5.9f, 10.25f, 10.5f);
	lights[24].m_transform.m_position = glm::vec3(-5.875f, 11.25f, 12.375f);


	for (int i = 1; i < numLight; ++i)
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

	for (int i = 1; i <= 12; ++i)
	{
		MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_LIGHTPOLE]->materials[i - 1].kEmission = glm::vec3(0.5f, 0.5f, 0.5f);

		glm::vec3 randColor = glm::vec3{
			Math::RandFloatMinMax(0.0f, 1.0f),
			Math::RandFloatMinMax(0.0f, 1.0f),
			Math::RandFloatMinMax(0.0f, 1.0f)
		};

		MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_LIGHTPOLE]->materials[i - 1].kAmbient = glm::vec3(randColor);
		lights[i].color = randColor;
		lights[12 + i].color = randColor;
	}

	for (int i = 0; i < numLight; ++i)
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

	PhysicsMaterial groundMat;
	groundMat.m_friction = 0.5f;
	objInScene[GROUND] = new GameObject();
	objInScene[GROUND]->rb = addStaticPlane(objInScene[GROUND], VECTOR3_UP, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[GROUND]);

	objInScene[WALLLEFT] = new GameObject();
	objInScene[WALLLEFT]->m_transform.Translate(-5.9f, .0f, .0f);
	objInScene[WALLLEFT]->rb = addStaticPlane(objInScene[WALLLEFT], VECTOR3_RIGHT, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[WALLLEFT]);

	objInScene[WALLRIGHT] = new GameObject();
	objInScene[WALLRIGHT]->m_transform.Translate(5.9f, .0f, .0f);
	objInScene[WALLRIGHT]->rb = addStaticPlane(objInScene[WALLRIGHT], -VECTOR3_RIGHT, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[WALLRIGHT]);

	objInScene[WALLBACK] = new GameObject();
	objInScene[WALLBACK]->m_transform.Translate(0.0f, 0.0f, 34.0f);
	objInScene[WALLBACK]->rb = addStaticPlane(objInScene[WALLBACK], -VECTOR3_FORWARD, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[WALLBACK]);

	objInScene[TABLE] = new TossTable();
	objInScene[TABLE]->m_transform.Translate(0.0f, .0f, -4.f);
	objInScene[TABLE]->m_transform.Rotate(0.0f, 90.0f);
	objInScene[TABLE]->m_transform.ScaleBy(2.0f, 2.0f, 2.0f);

	objInScene[BOARD] = new TossBoard();
	objInScene[BOARD]->m_transform.Translate(0.0f, 5.5f, 0.0f);

	float colliderHeight = 7.3f;
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	objInScene[COUNTER] = new GameObject;
	objInScene[COUNTER]->m_transform.Translate(0.0f, .0f, 10.0f);
	objInScene[COUNTER]->m_transform.Rotate(0.0f, 180.0f);
	objInScene[COUNTER]->m_transform.ScaleBy(0.4f, 0.4f, 0.4f);
	objInScene[COUNTER]->rb = addBoxCollider(objInScene[COUNTER], 10.0f, colliderHeight, 6.25f, mat, glm::vec3(0.0f, colliderHeight / 2.f, 0.f));
	GameObjectManager::addItem(objInScene[COUNTER]);

	colliderHeight = 11.0f;
	PhysicsMaterial matP;
	matP.m_mass = 1.0f;
	objInScene[PLAYER] = new GameObject;
	objInScene[PLAYER]->m_transform.Translate(mainCamera.m_transform.m_position.x, 0.0f, mainCamera.m_transform.m_position.z);
	objInScene[PLAYER]->rb = addBoxCollider(objInScene[PLAYER], 2.0f, colliderHeight, 2.0f, matP, glm::vec3(0.0f, colliderHeight / 2.f, 0.f));
	GameObjectManager::addItem(objInScene[PLAYER]);
	objInScene[PLAYER]->rb->setSleepingThresholds(0.0f, 0.0f);
	objInScene[PLAYER]->rb->setAngularFactor(btVector3(0, 0, 0));

	objInScene[RING] = new Ring;
	objInScene[RING]->m_transform.Translate(-2.0f, 8.0f, 12.0f);
	objInScene[RING2] = new Ring;
	objInScene[RING2]->m_transform.Translate(0.0f, 8.0f, 12.0f);
	objInScene[RING3] = new Ring;
	objInScene[RING3]->m_transform.Translate(2.0f, 8.0f, 12.0f);
	GameObjectManager::IniAll();

	objInScene[PLAYER]->rb->setIgnoreCollisionCheck(objInScene[RING]->rb, true);
	objInScene[PLAYER]->rb->setIgnoreCollisionCheck(objInScene[RING2]->rb, true);
	objInScene[PLAYER]->rb->setIgnoreCollisionCheck(objInScene[RING3]->rb, true);

	objInScene[RING]->rb->setIgnoreCollisionCheck(objInScene[RING2]->rb, true);
	objInScene[RING]->rb->setIgnoreCollisionCheck(objInScene[RING3]->rb, true);
	objInScene[RING2]->rb->setIgnoreCollisionCheck(objInScene[RING3]->rb, true);

	objInScene[RING]->rb->setGravity(btVector3(0.0f, -7.0f, 0.0f));
	objInScene[RING2]->rb->setGravity(btVector3(0.0f, -7.0f, 0.0f));
	objInScene[RING3]->rb->setGravity(btVector3(0.0f, -7.0f, 0.0f));

}

void SceneRingToss::Update()
{
	HandleKeyPress();
	mainCamera.Update();
	glm::vec3 inputMovementDir{};
	if (KeyboardController::GetInstance()->IsKeyDown('W'))
		inputMovementDir = mainCamera.forward;
	if (KeyboardController::GetInstance()->IsKeyDown('S'))
		inputMovementDir = -mainCamera.forward;
	if (KeyboardController::GetInstance()->IsKeyDown('D'))
		inputMovementDir = mainCamera.right;
	if (KeyboardController::GetInstance()->IsKeyDown('A'))
		inputMovementDir = -mainCamera.right;
	glm::vec3 finalForce = inputMovementDir * 10.0f;
	objInScene[PLAYER]->rb->setLinearVelocity(btVector3(finalForce.x, 0.0f, finalForce.z));

	if (isPickable)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (isObjectInteractable(objInScene[RING + i]->GetRigidbodyPosition(), mainCamera, 25.0f, 4.f))
			{
				isInView = true;
				currentIndexRing = RING + i;
				if (KeyboardController::GetInstance()->IsKeyPressed('F'))
				{
					isPickable = false;
					isShoot = false;
					isInView = false;
				}
				break;
			}
			else isInView = false;
		}
	}
	waitingTimer += Time::deltaTime; 
	
	lightTimer += Time::deltaTime;
	if (isTimeReach(lightTimer, 1.f, 1.05f))
	{
		for (int i = 1; i <= 12; ++i)
		{
			MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_LIGHTPOLE]->materials[i - 1].kEmission = glm::vec3(0.5f, 0.5f, 0.5f); 

			glm::vec3 randColor = glm::vec3{
				Math::RandFloatMinMax(0.0f, 1.0f),
				Math::RandFloatMinMax(0.0f, 1.0f),
				Math::RandFloatMinMax(0.0f, 1.0f)
			};

			MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_LIGHTPOLE]->materials[i - 1].kAmbient = glm::vec3(randColor);
			lights[i].color = randColor;
			glUniform3fv(m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * i], 1, &lights[i].color.r);

			lights[12 + i].color = randColor;
			glUniform3fv(m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * (12 + i)], 1, &lights[12 + i].color.r); 
		}
	}
	else if (lightTimer >= 1.1f) lightTimer = 0.0f;

	for (size_t i = 0; i < ((TossBoard*)objInScene[BOARD])->bottles.size(); ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			if (!objInScene[RING+ j]->rb->isActive() || !((Ring*)objInScene[RING + j])->trigger) continue;

			if (CheckCollisionWith(((TossBoard*)objInScene[BOARD])->bottles[i]->trigger->getObject(),
				((Ring*)objInScene[RING + j])->trigger->getObject()))
			{
				points += 500;
				btCollisionShape* collider = ((Ring*)objInScene[RING + j])->trigger->rb->getCollisionShape();
				GameObjectManager::removeItem(((Ring*)objInScene[RING + j])->trigger);
				ColliderManager::GetInstance()->RemoveCollider(collider);
				((Ring*)objInScene[RING + j])->trigger = NULL;
			}
		}
	}

	GameObjectManager::UpdateAll();
}

void SceneRingToss::LateUpdate()
{
	static bool isMouseLeftPressed = false;

	glm::vec3 playerRBPosition = objInScene[PLAYER]->GetRigidbodyPosition();
	mainCamera.m_transform.m_position = glm::vec3(playerRBPosition.x, playerRBPosition.y + 10, playerRBPosition.z);
	mainCamera.UpdateCameraRotation();

	if (!isShoot && !isPickable)
	{
		glm::vec3 throwablePos = mainCamera.m_transform.m_position
			+ mainCamera.right * 1.0f  // Move right
			- mainCamera.up * 0.5f    // Move down
			+ mainCamera.view * 1.0f; // Optional depth
		objInScene[currentIndexRing]->SetRigidbodyPosition(throwablePos);
		objInScene[currentIndexRing]->rb->clearGravity();

		if (MouseController::GetInstance()->IsButtonPressed(0) && !isMouseLeftPressed)
		{
			float power = 12.5f;
			btVector3 finalVel = btVector3(mainCamera.view.x, mainCamera.view.y, mainCamera.view.z) * power;
			objInScene[currentIndexRing]->rb->setLinearVelocity(btVector3(finalVel));
			objInScene[currentIndexRing]->rb->setAngularVelocity(btVector3(0.f, power, 0.0f));
			objInScene[currentIndexRing]->rb->activate();
			isShoot = true;
			isPickable = true;
			numAttempt++;
		}
	}
	if (numAttempt == 3 and waitingTimer >= 5.0f)
	{
		SceneManager::GetInstance()->ChangeState(new carnivalroaming);
		return;
	}
	if (MouseController::GetInstance()->IsButtonReleased(0) && isMouseLeftPressed)
		isMouseLeftPressed = false;

	GameObjectManager::LateUpdateAll();
}

void SceneRingToss::Render()
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

	modelStack.PushMatrix();
	modelStack.Translate(6.0f, 0.0f, 0.0f);
	modelStack.Rotate(-90.0f, .0f, 1.0f, 0.0f);
	modelStack.Scale(7.f, 7.f, 7.f);
	RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_LIGHTPOLE], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-6.0f, 0.0f, 0.0f);
	modelStack.Rotate(-90.0f, .0f, 1.0f, 0.0f);
	modelStack.Scale(7.f, 7.f, 7.f);
	RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_LIGHTPOLE], enableLight);
	modelStack.PopMatrix();

	MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_COUNTER]->material = Material::Wood(WHITE);
	RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RT_COUNTER], enableLight, objInScene[COUNTER]->m_transform);
	GameObjectManager::RenderAll(*this);

	if (isInView)
	{
		modelStack.PushMatrix();
		modelStack.LoadIdentity();
		modelStack.Translate(objInScene[currentIndexRing]->m_transform.m_position.x - 1.0f,
							objInScene[currentIndexRing]->m_transform.m_position.y + 1.0f,
							objInScene[currentIndexRing]->m_transform.m_position.z);
		modelStack.Scale(0.15f, 0.15f, 0.15f);
		RenderText(meshList[GEO_TEXT], "Press 'F' to pickup", BLUE);
		modelStack.PopMatrix();
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "Score: " + std::to_string(points), RED, 25.0f, Application::m_consoleWidth - 200.0f, Application::m_consoleHeight - 50.0f);

#ifdef DRAW_HITBOX

	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	RenderMesh(meshList[GEO_AXIS]);
	modelStack.PopMatrix();

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
			modelStack.Scale(shape->getLocalScaling().x(), shape->getLocalScaling().y(), shape->getLocalScaling().z());
			RenderMesh(hitboxMeshList[HITBOX_SPHERE]);
		}
		else if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE)
		{
			BoxCollider* boxCollider = static_cast<BoxCollider*>(shape);
			float width, height, depth;
			boxCollider->GetDimension(width, height, depth);
			modelStack.Scale(width, height, depth);
			modelStack.Scale(shape->getLocalScaling().x(), shape->getLocalScaling().y(), shape->getLocalScaling().z());
			RenderMesh(hitboxMeshList[HITBOX_BOX]);
		}
		else if (shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
		{
			CylinderCollider* cylinderCollider = static_cast<CylinderCollider*>(shape);
			float rad, height;
			cylinderCollider->GetDimension(rad, height);
			modelStack.Scale(rad / 2.0f, height, rad / 2.0f);
			modelStack.Scale(shape->getLocalScaling().x(), shape->getLocalScaling().y(), shape->getLocalScaling().z());
			RenderMesh(hitboxMeshList[HITBOX_CYLINDER]);
		}
		else if (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
		{
			btCompoundShape* compoundShape = static_cast<btCompoundShape*>(shape);
			for (unsigned i = 0; i < (unsigned)compoundShape->getNumChildShapes(); ++i)
			{
				btCollisionShape* childShape = compoundShape->getChildShape(i);
				btTransform childTransform = compoundShape->getChildTransform(i);
				float mat[16];
				childTransform.getOpenGLMatrix(mat);

				RenderChildCollider(childShape, mat);
			}
		}
		else if (shape->getShapeType() == STATIC_PLANE_PROXYTYPE)
		{
			modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
			RenderMesh(hitboxMeshList[HITBOX_GROUND]);
		}
		modelStack.PopMatrix();
	}

	if(isFillMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

void SceneRingToss::Exit()
{
	Scene::Exit();
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneRingToss::RenderSkybox(void)
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

void SceneRingToss::RenderGround(int size)
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
