#define _USE_MATH_DEFINES
#include "SceneBalloon.h"
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
#include "Balloon.h"
#include "BalloonBoard.h"
#include "Dart.h"
#include "BP_Counter.h"
#include "BP_Box.h"

SceneBalloon::SceneBalloon() : numLight{ 11 }, currentDartIndex{ DART1 }
{
	meshList.resize(NUM_GEOMETRY);
	lights.resize(numLight);
}

SceneBalloon::~SceneBalloon()
{
}

void SceneBalloon::Init()
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

		Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);
	}

	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", YELLOW, 1.0f, 100, 100);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", GREY, 1.0f);
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("Cylinder", BLUE, 180, 1.0f, 1.0f);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", WHITE, 75.0f);
	meshList[GEO_PLANE]->textureID = LoadPNG("Images//ground.png");

	meshList[GEO_BP_BALLOON] = MeshBuilder::GenerateOBJ("Balloon", "Models//balloon.obj");
	meshList[GEO_BP_BOOTHGUARDS] = MeshBuilder::GenerateOBJMTL("BoothGuards", "Models//BP_BoothGuards.obj", "Models//BP_BoothGuards.mtl");
	meshList[GEO_BP_BOOTHROOF] = MeshBuilder::GenerateOBJMTL("BoothRoof", "Models//BP_BoothRoof.obj", "Models//BP_BoothRoof.mtl");
	meshList[GEO_BP_BOOTHROOF]->textureID = LoadPNG("Images//BP_BoothRoof.png");
	meshList[GEO_BP_BALLOONBOARD] = MeshBuilder::GenerateOBJ("BalloonBoard", "Models//noticeboard.obj");
	meshList[GEO_BP_BALLOONBOARD]->textureID = LoadPNG("Images//boardimage.png");
	meshList[GEO_BP_DART] = MeshBuilder::GenerateOBJ("Dart", "Models//dart.obj");
	meshList[GEO_BP_POWERUI_FRAME] = MeshBuilder::GenerateQuad("PowerUi_Frame", glm::vec3(1, 1, 1), 1);
	meshList[GEO_BP_POWERUI_FRAME]->textureID = LoadPNG("Images//CK_PowerUi_Frame.png");
	meshList[GEO_BP_POWERUI_BAR] = MeshBuilder::GenerateQuad("PowerUi_Bar", glm::vec3(1, 1, 0), 1);
	meshList[GEO_BP_COUNTER] = MeshBuilder::GenerateCube("Counter", glm::vec3(0.459, 0.302, 0), 1);
	meshList[GEO_BP_COUNTER]->textureID = LoadPNG("Images//BP_Wood.png");
	meshList[GEO_BP_COUNTER]->material = Material::Wood(glm::vec3(0.459, 0.302, 0));
	meshList[GEO_BP_FAIRYLIGHT] = MeshBuilder::GenerateOBJMTL("FairyLight", "Models//BP_FairyLight.obj", "Models//BP_FairyLight.mtl");

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
	fov = 45.f;
	
	glUniform1i(m_parameters[U_NUMLIGHTS], numLight);

	lights[0].m_transform.m_position = glm::vec3{};
	lights[0].m_transform.m_position = glm::vec3(0, 12, 0);
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_POINT;
	lights[0].power = 0.5;
	lights[0].kC = 0.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = 45.f;
	lights[0].cosInner = 30.f;
	lights[0].exponent = 3.f;
	lights[0].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	lights[1].m_transform.m_position = glm::vec3(4.50083, 7.36148, -9.67616);
	lights[2].m_transform.m_position = glm::vec3(1.9155, 7.23154, -9.66117);
	lights[3].m_transform.m_position = glm::vec3(0, 7.94687, -9.46624);
	lights[4].m_transform.m_position = glm::vec3(-2.18515, 7.04141, -9.75959);
	lights[5].m_transform.m_position = glm::vec3(-4.67714, 7.49898, -9.42656);

	lights[6].m_transform.m_position = glm::vec3(4.50083, 7.36148, 9.07616);
	lights[7].m_transform.m_position = glm::vec3(1.9155, 7.23154, 9.06117);
	lights[8].m_transform.m_position = glm::vec3(0, 7.94687, 9.26624);
	lights[9].m_transform.m_position = glm::vec3(-2.18515, 7.04141, 9.05959);
	lights[10].m_transform.m_position = glm::vec3(-4.67714, 7.49898, 9.42656);

	for (int i = 1; i < numLight; ++i)
	{
		lights[i].color = glm::vec3(1, 1, 1);
		lights[i].type = Light::LIGHT_POINT;
		lights[i].power = 0.3f;
		lights[i].kC = 1.f;
		lights[i].kL = 0.01f;
		lights[i].kQ = 0.001f;
		lights[i].cosCutoff = 45.f;
		lights[i].cosInner = 30.f;
		lights[i].exponent = 3.f;
		lights[i].spotDirection = glm::vec3(0.f, 1.f, 0.f);
	}

	for (int i = 1; i <= 5; ++i)
	{
		meshList[GEO_BP_FAIRYLIGHT]->materials[i - 1].kEmission = glm::vec3(0.5f, 0.5f, 0.5f);

		glm::vec3 randColor = glm::vec3{
			Math::RandFloatMinMax(0.0f, 1.0f),
			Math::RandFloatMinMax(0.0f, 1.0f),
			Math::RandFloatMinMax(0.0f, 1.0f)
		};

		meshList[GEO_BP_FAIRYLIGHT]->materials[i - 1].kAmbient = glm::vec3(randColor);
		lights[i].color = randColor;
		lights[5 + i].color = randColor;
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

	objInScene[BALLOONBOARD] = new BalloonBoard();
	objInScene[COUNTER] = new BP_Counter();
	objInScene[BOXF] = new BP_Box();
	objInScene[BOXB] = new BP_Box();
	objInScene[BOXL] = new BP_Box();
	objInScene[BOXR] = new BP_Box();
	objInScene[DART1] = new Dart();
	objInScene[DART2] = new Dart();
	objInScene[DART3] = new Dart();
	objInScene[DART4] = new Dart();
	objInScene[DART5] = new Dart();
	objInScene[DART6] = new Dart();
	objInScene[DART7] = new Dart();
	objInScene[DART8] = new Dart();
	objInScene[DART9] = new Dart();
	objInScene[DART10] = new Dart();


	objInScene[BALLOONBOARD]->m_transform.Translate(-5.2, 3.6, -0.3);
	objInScene[COUNTER]->m_transform.Translate(5.2, 1.325, 0);
	objInScene[BOXR]->m_transform.Translate(5.2, 2.66, -0.8);
	objInScene[BOXL]->m_transform.Translate(5.2, 2.66, -2.3);
	objInScene[BOXF]->m_transform.Translate(4.55, 2.66, -1.55);
	objInScene[BOXF]->m_transform.Rotate(0, 90, 0);
	objInScene[BOXB]->m_transform.Translate(5.85, 2.66, -1.55);
	objInScene[BOXB]->m_transform.Rotate(0, 90, 0);

	for (int i = 0; i < 10; i++) {
		objInScene[currentDartIndex]->m_transform.Translate(5.2, 2.66, -1.3);
		currentDartIndex++;
	}

	currentDartIndex = DART1;

	GameObjectManager::GetInstance()->IniAll();

	PhysicsMaterial groundMat;
	groundMat.m_friction = 0.5f;
	objInScene[GROUND] = new GameObject();
	objInScene[GROUND]->rb = addStaticPlane(objInScene[GROUND], VECTOR3_UP, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[GROUND]);

	power = 5.f;
	cooldownTimer = 0.f;
	isShooting = false;
	gameComplete = false;
	attemptsLeft = 9;
	lightTimer = 0.f;
}

void SceneBalloon::Update()
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
	glm::vec3 finalForce = inputMovementDir * 10.0f * Time::deltaTime;
	mainCamera.m_transform.Translate(finalForce);
	glm::vec3 prevTarget = mainCamera.target;
	mainCamera.UpdateCameraRotation();

	/*float speed = 5 * Time::deltaTime;
	if (KeyboardController::GetInstance()->IsKeyDown('I'))
		devVec.z += speed;
	if (KeyboardController::GetInstance()->IsKeyDown('K'))
		devVec.z -= speed;
	if (KeyboardController::GetInstance()->IsKeyDown('J'))
		devVec.x += speed;
	if (KeyboardController::GetInstance()->IsKeyDown('L'))
		devVec.x -= speed;
	if (KeyboardController::GetInstance()->IsKeyDown('O'))
		devVec.y += speed;
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
		devVec.y -= speed;*/

	//lights[5].m_transform.m_position = devVec;
	//std::cout << devVec.x << ", " << devVec.y << ", " << devVec.z << std::endl;


	// stop player rotating too far:
	{
		glm::vec3 toObject = glm::normalize(glm::vec3(0, 3, 0) - mainCamera.m_transform.m_position);

		glm::vec3 lookVector = glm::normalize(mainCamera.target - mainCamera.m_transform.m_position);

		float dotProduct = glm::dot(lookVector, toObject);
		float threshold = glm::cos(glm::radians(fov * 0.7));

		if (dotProduct <= threshold) // Rotated too much
		{
			mainCamera.target = prevTarget;
		}
		else {
			float closeness = (dotProduct - threshold) / (1.0f - threshold);
			mainCamera.sensitivity = 10 + closeness * (50 - 10);
		}
	}

	// update light:
	{
		lightTimer += Time::deltaTime;

		if (isTimeReach(lightTimer, 2.f, 2.05f) && !isLightChanged)
		{
			for (int i = 1; i <= 5; ++i)
			{
				meshList[GEO_BP_FAIRYLIGHT]->materials[i - 1].kEmission = glm::vec3(0.5f, 0.5f, 0.5f);

				glm::vec3 randColor = glm::vec3{
					Math::RandFloatMinMax(0.0f, 1.0f),
					Math::RandFloatMinMax(0.0f, 1.0f),
					Math::RandFloatMinMax(0.0f, 1.0f)
				};

				meshList[GEO_BP_FAIRYLIGHT]->materials[i - 1].kAmbient = glm::vec3(randColor);
				lights[i].color = randColor;
				glUniform3fv(m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * i], 1, &lights[i].color.r);

				lights[5 + i].color = randColor;
				glUniform3fv(m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * (5 + i)], 1, &lights[5 + i].color.r);
			}

			isLightChanged = true;
		}
		else if (lightTimer >= 2.1f)
		{
			lightTimer = 0.0f;
			isLightChanged = false;
		}
	}

	UnorderedVector<Balloon*> ballonList = ((BalloonBoard*)objInScene[BALLOONBOARD])->balloons;
	for (size_t i = 0; i < ballonList.size(); ++i)
	{
		if (ballonList[i]->getObject() != nullptr && objInScene[currentDartIndex]->getObject() != nullptr) {
			if (CheckCollisionWith(ballonList[i]->getObject(), objInScene[currentDartIndex]->getObject()))
			{
				GameObjectManager::GetInstance()->removeItem(ballonList[i]);
				((BalloonBoard*)objInScene[BALLOONBOARD])->balloons.remove(ballonList[i]);
				score += 20;
				break;
			}
		}
	}

	if (CheckCollisionWith(objInScene[currentDartIndex]->getObject(), objInScene[BALLOONBOARD]->getObject()))
	{
		objInScene[currentDartIndex]->rb->setCollisionFlags(objInScene[currentDartIndex]->rb->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}

	if (attemptsLeft < 0) {
		gameEnd = true;
	}

	if (gameEnd) {
		// GAME END
	}

	GameObjectManager::GetInstance()->UpdateAll();
}

void SceneBalloon::LateUpdate()
{
	mainCamera.Update();

	if (cooldownTimer > 0) {
		cooldownTimer -= Time::deltaTime;
	}
	else if (isShooting == true) {
		isShooting = false;
		cooldownTimer = 0;
		attemptsLeft--;
		if (currentDartIndex != DART10)
			currentDartIndex++;

		//if (attemptsLeft < 0 && gameComplete == false) {
		//	SceneManager::GetInstance()->PopState();
		//	SceneManager::GetInstance()->PushState(new SceneCanKnockdown);
		//}
	}

	float powerIncreaseSpeed = maxPower;
	if (isShooting == false && attemptsLeft >= 0 && gameComplete == false)
	{
		glm::vec3 cameraPos = mainCamera.m_transform.m_position;
		glm::vec3 forward = mainCamera.view;
		glm::vec3 right = mainCamera.right;
		glm::vec3 up = mainCamera.up;

		// Calculate world position of the ball
		glm::vec3 newPos = cameraPos + (forward * 2.f) + (right * 1.f) + (up * -0.6f);
		glm::vec3 direction = glm::normalize(mainCamera.target - cameraPos);

		glm::vec3 gunRotation;
		gunRotation.y = atan2(direction.x, direction.z);
		gunRotation.x = asin(-direction.y);

		//objInScene[DART]->m_transform.m_rotation = gunRotation;

		if (objInScene[currentDartIndex] != nullptr && attemptsLeft >= 0) {

			objInScene[currentDartIndex]->SetRigidbodyPosition(newPos);
			objInScene[currentDartIndex]->SetRigidbodyRotation(gunRotation);
			objInScene[currentDartIndex]->rb->clearGravity();
			objInScene[currentDartIndex]->rb->setAngularVelocity(btVector3(0, 0, 0));
			objInScene[currentDartIndex]->rb->setIgnoreCollisionCheck(objInScene[BOXF]->getObject(), true);
			objInScene[currentDartIndex]->rb->setIgnoreCollisionCheck(objInScene[BOXB]->getObject(), true);
			objInScene[currentDartIndex]->rb->setIgnoreCollisionCheck(objInScene[BOXL]->getObject(), true);
			objInScene[currentDartIndex]->rb->setIgnoreCollisionCheck(objInScene[BOXR]->getObject(), true);
			objInScene[currentDartIndex]->rb->setIgnoreCollisionCheck(objInScene[COUNTER]->getObject(), true);
		}

		if (cooldownTimer <= 0) {
			if (MouseController::GetInstance()->IsButtonDown(0))
			{
				if (power < maxPower) {
					power += powerIncreaseSpeed * Time::deltaTime;
				}
				else {
					power = maxPower;
				}
			}

			if (MouseController::GetInstance()->IsButtonReleased(0) && objInScene[currentDartIndex] != nullptr)
			{
				isShooting = true;
				cooldownTimer = 3;
				glm::vec3 look = mainCamera.view * power;
				objInScene[currentDartIndex]->rb->setLinearVelocity(btVector3(look.x - 3, look.y + 3, look.z));
				objInScene[currentDartIndex]->rb->activate();
				power = 0;
			}
		}
	}
}

void SceneBalloon::Render()
{
	// Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 model = glm::mat4(1.0f);
	// Setup Model View Projection matrix
	projectionStack.LoadMatrix(glm::perspective(fov,
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

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_AXIS]);
	modelStack.PopMatrix();

	RenderSkybox();
	RenderGround(7);

	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1.5, 1, 1);

	meshList[GEO_BP_BOOTHGUARDS]->material = Material::Metal(GREY);
	RenderMesh(meshList[GEO_BP_BOOTHGUARDS], true);

	meshList[GEO_BP_BOOTHROOF]->material = Material::Wood(WHITE);
	RenderMesh(meshList[GEO_BP_BOOTHROOF], true);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//RenderMesh(meshList[GEO_PLANE]);
	//modelStack.PopMatrix();

	// fairy lights:
	{
		modelStack.PushMatrix();
		modelStack.Translate(-0.00242271, 7.51264, -9.57801);
		modelStack.Scale(6, 6, 6);
		RenderMesh(meshList[GEO_BP_FAIRYLIGHT], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-0.00242271, 7.51264, 9.15998);
		modelStack.Scale(6, 6, 6);
		RenderMesh(meshList[GEO_BP_FAIRYLIGHT], true);
		modelStack.PopMatrix();
	}


	GameObjectManager::GetInstance()->RenderAll(*this);

	// Render ui:
	{
		if (power > 5) {
			RenderMeshOnScreen(meshList[GEO_BP_POWERUI_FRAME], 0.5, 0.3, 100 * 1.25, 25 * 1.25, glm::vec2(0, 0));
			RenderMeshOnScreen(meshList[GEO_BP_POWERUI_BAR], 0.425, 0.3, power / maxPower * 120, 25, glm::vec2(-0.5, 0));
		}
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "Score:" + std::to_string(score), RED, 30, 1000, 600);

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

	if(isFillMode) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

void SceneBalloon::Exit()
{
	Scene::Exit();
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneBalloon::RenderSkybox(void)
{
	float size = 30.0f;
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

void SceneBalloon::RenderMeshOnScreen(Mesh* mesh, float x, float y, float width, float height, glm::vec2 anchorPoint)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -
		1000.f, 1000.f); // dimension of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();

	// To do: Use modelStack to position GUI on screen
	modelStack.Translate(x * 800 - width * anchorPoint.x, y * 600 - height * anchorPoint.y, 0);
	// To do: Use modelStack to scale the GUI
	modelStack.Scale(width, height, 1);
	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBalloon::RenderGround(int size)
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
			modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
			RenderMesh(meshList[GEO_PLANE], enableLight);
			modelStack.PopMatrix();

			originPos.x -= 75.0f;
		}

		originPos.z -= 75.0f;
	}
}