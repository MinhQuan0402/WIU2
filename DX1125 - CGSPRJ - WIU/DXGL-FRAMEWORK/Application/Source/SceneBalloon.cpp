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

SceneBalloon::SceneBalloon() : numLight{ 2 }
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

	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", YELLOW, 1.0f, 100, 100);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", GREY, 1.0f);
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("Cylinder", BLUE, 180, 1.0f, 1.0f);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", GREY, 1000.0f);

	meshList[GEO_BALLOON] = MeshBuilder::GenerateOBJMTL("Balloon", "Models//balloon.obj", "Models//balloon.mtl");
	meshList[GEO_BPBOOTHGUARDS] = MeshBuilder::GenerateOBJMTL("BoothGuards", "Models//BP_BoothGuards.obj", "Models//BP_BoothGuards.mtl");
	meshList[GEO_BPBOOTHROOF] = MeshBuilder::GenerateOBJMTL("BoothRoof", "Models//BP_BoothRoof.obj", "Models//BP_BoothRoof.mtl");
	meshList[GEO_BPBOOTHROOF]->textureID = LoadPNG("Images//BP_BoothRoof.png");
	meshList[GEO_BALLOONBOARD] = MeshBuilder::GenerateOBJ("BalloonBoard", "Models//noticeboard.obj");
	meshList[GEO_BALLOONBOARD]->textureID = LoadPNG("Images//boardimage.png");
	meshList[GEO_DART] = MeshBuilder::GenerateOBJMTL("Dart", "Models//dart.obj", "Models//dart.mtl");
	meshList[GEO_POWERUI_FRAME] = MeshBuilder::GenerateQuad("PowerUi_Frame", glm::vec3(1, 1, 1), 1);
	meshList[GEO_POWERUI_FRAME]->textureID = LoadPNG("Images//CK_PowerUi_Frame.png");
	meshList[GEO_POWERUI_BAR] = MeshBuilder::GenerateQuad("PowerUi_Bar", glm::vec3(1, 1, 0), 1);

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

	mainCamera.Init(glm::vec3(10, 4, 0), glm::vec3(0, 4, 0), VECTOR3_UP);
	mainCamera.sensitivity = 15.f;
	fov = 45.f;
	
	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	lights[0].m_transform.m_position = glm::vec3{};
	lights[0].m_transform.m_position = glm::vec3(0, 5, 0);
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_POINT;
	lights[0].power = 1;
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

	enableLight = true;

	objInScene[BALLOONBOARD] = new BalloonBoard();
	objInScene[DART] = new Dart();

	objInScene[BALLOONBOARD]->m_transform.Translate(-5.2, 3.6, -0.3);

	GameObjectManager::GetInstance()->IniAll();

	power = 5.f;
	cooldownTimer = 0.f;
	isShooting = false;
	gameComplete = false;
	attemptsLeft = 9;
}

void SceneBalloon::Update()
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
	glm::vec3 prevTarget = mainCamera.target;
	mainCamera.UpdateCameraRotation();

	// stop player rotating too far:
	{
		glm::vec3 toObject = glm::normalize(glm::vec3(0, 3, 0) - mainCamera.m_transform.m_position);

		glm::vec3 lookVector = glm::normalize(mainCamera.target - mainCamera.m_transform.m_position);

		float dotProduct = glm::dot(lookVector, toObject);
		float threshold = glm::cos(glm::radians(fov * 0.5));

		if (dotProduct <= threshold) // Rotated too much
		{
			mainCamera.target = prevTarget;
		}
		else {
			float closeness = (dotProduct - threshold) / (1.0f - threshold);
			mainCamera.sensitivity = 10 + closeness * (50 - 10);
		}
	}

	GameObjectManager::GetInstance()->UpdateAll();
}

void SceneBalloon::LateUpdate()
{
	if (cooldownTimer > 0) {
		cooldownTimer -= Time::deltaTime;
	}
	else if (isShooting == true) {
		isShooting = false;
		cooldownTimer = 0;
		attemptsLeft--;

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
		glm::vec3 newPos = cameraPos + (forward * 2.f) + (right * 1.f) + (up * -1.f);

		objInScene[DART]->m_transform.m_rotation = glm::vec3(mainCamera.view.x, mainCamera.view.y, mainCamera.view.z);

		if (objInScene[DART] != nullptr && attemptsLeft >= 0) {
			objInScene[DART]->SetRigidbodyPosition(newPos);
			objInScene[DART]->rb->clearGravity();
			objInScene[DART]->rb->setSleepingThresholds(0, 0);
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

			if (MouseController::GetInstance()->IsButtonReleased(0) && objInScene[DART] != nullptr)
			{
				isShooting = true;
				cooldownTimer = 3;
				glm::vec3 look = mainCamera.view * power;
				objInScene[DART]->rb->setSleepingThresholds(0.8, 1);
				objInScene[DART]->rb->setLinearVelocity(btVector3(look.x - 3, look.y + 3, look.z));
				objInScene[DART]->rb->activate();
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
	RenderMesh(meshList[GEO_AXIS]);
	modelStack.PopMatrix();

	RenderSkybox();

	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1.5, 1, 1);

	meshList[GEO_BPBOOTHGUARDS]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BPBOOTHGUARDS]->material.kDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	meshList[GEO_BPBOOTHGUARDS]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BPBOOTHGUARDS]->material.kShininess = 2.0f;
	RenderMesh(meshList[GEO_BPBOOTHGUARDS], true);

	meshList[GEO_BPBOOTHROOF]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BPBOOTHROOF]->material.kDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	meshList[GEO_BPBOOTHROOF]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BPBOOTHROOF]->material.kShininess = 2.0f;
	RenderMesh(meshList[GEO_BPBOOTHROOF], true);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//RenderMesh(meshList[GEO_PLANE]);
	//modelStack.PopMatrix();

	GameObjectManager::GetInstance()->RenderAll(*this);

	// Render ui:
	{
		if (power > 5) {
			RenderMeshOnScreen(meshList[GEO_POWERUI_FRAME], 600, 200, 100 * 1.25, 25 * 1.25, glm::vec3(0, 0, 0));
			RenderMeshOnScreen(meshList[GEO_POWERUI_BAR], 600, 200, power / maxPower * 120, 25, glm::vec3(-0.5, 0, 0));
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
