#define _USE_MATH_DEFINES
#include "SceneDuckShooting.h"
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
#include "LoadPNG.h"
#include <btBulletDynamicsCommon.h>
#include "ColliderManager.h"
#include <MyMath.h>
#include "Utility.h"

#include "DS_Floor.h"
#include "DS_BoothWall.h"
#include "DS_BoothRoof.h"
#include "DS_Conveyor.h"
#include "DS_Pillar.h"
#include "DS_Duck.h"
#include "DS_Bullet.h"
#include "SceneManager.h"

SceneDuckShooting::SceneDuckShooting() : numLight{ 2 }
{
	meshList.resize(NUM_GEOMETRY);
	lights.resize(numLight);
}

SceneDuckShooting::~SceneDuckShooting()
{
}

void SceneDuckShooting::Init()
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


		m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
		m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
		m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
		m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
		m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
		m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
		m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
		m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
		m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
		m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
		m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

		m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
		m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
		m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
		m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
		m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
		m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	}

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);

	meshList[GEO_HITBOX] = MeshBuilder::GenerateCube("HitBox", GREEN, 1.0f);

	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_MODEL1] = MeshBuilder::GenerateOBJ("Doorman", "Models//doorman.obj");
	meshList[GEO_MODEL1]->textureID = LoadPNG("Images//doorman.png");

	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", WHITE, 1.0f, 100, 100);
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("Cylinder", WHITE, 100, 1, 1);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", WHITE, 1.0f);

	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", glm::vec3(0.1, 0.1, 0.1), 1000.0f);
	meshList[GEO_PLANE]->material = Material::Concrete(GREY);

	meshList[GEO_BOOTHROOF] = MeshBuilder::GenerateOBJMTL("BoothRoof", "Models//DS_BoothRoof.obj", "Models//DS_BoothRoof.mtl");
	meshList[GEO_BOOTHROOF]->textureID = LoadPNG("Images//DS_BoothRoof.png");
	meshList[GEO_BOOTHROOF]->material = Material::Wood(WHITE);

	meshList[GEO_BOOTHGUARDS] = MeshBuilder::GenerateOBJMTL("BoothGuards", "Models//DS_BoothGuards.obj", "Models//DS_BoothGuards.mtl");
	meshList[GEO_BOOTHGUARDS]->material = Material::Metal(GREY);

	meshList[GEO_COUNTER] = MeshBuilder::GenerateCube("Counter", glm::vec3(0.459, 0.302, 0), 1);
	meshList[GEO_COUNTER]->textureID = LoadPNG("Images//DS_Wood.png");
	meshList[GEO_COUNTER]->material = Material::Wood(glm::vec3(0.459, 0.302, 0));

	meshList[GEO_LIGHTBULB] = MeshBuilder::GenerateOBJMTL("Lamp", "Models//DS_LightBulb.obj", "Models//DS_LightBulb.mtl");
	meshList[GEO_LIGHTBULB]->material = Material::Wood(WHITE);

	meshList[GEO_DUCK] = MeshBuilder::GenerateOBJMTL("Duck", "Models//DS_Duck.obj", "Models//DS_Duck.mtl");
	meshList[GEO_DUCK]->material = Material::Wood(YELLOW);

	meshList[GEO_GUN] = MeshBuilder::GenerateOBJ("Gun", "Models//DS_Gun.obj");
	meshList[GEO_GUN]->material = Material::Wood(WHITE);
	meshList[GEO_GUN]->textureID = LoadPNG("Images//DS_Gun.png");

	meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", BLUE, 1);
	meshList[GEO_BULLET]->material = Material::Plastic(BLUE);


	mainCamera.Init(glm::vec3(0, 3, 7.5), glm::vec3(0, 3, 0), VECTOR3_UP);
	mainCamera.sensitivity = 50.0f;

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	lights[0].m_transform.m_position = glm::vec3(3, 100, 2);
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].power = 0;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = 90.f;
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

	lights[1].m_transform.m_position = glm::vec3(0.2866777, 10.987, 0.0397956);
	lights[1].color = glm::vec3(1, 1, 1);
	lights[1].type = Light::LIGHT_POINT;
	lights[1].power = 1;
	lights[1].kC = 1.f;
	lights[1].kL = 0.01f;
	lights[1].kQ = 0.001f;
	lights[1].cosCutoff = 60.f;
	lights[1].cosInner = 30.f;
	lights[1].exponent = 3.f;
	lights[1].spotDirection = glm::vec3(0.f, -1.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &lights[1].color.r);
	glUniform1i(m_parameters[U_LIGHT1_TYPE], lights[1].type);
	glUniform1f(m_parameters[U_LIGHT1_POWER], lights[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], lights[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], lights[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], lights[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], cosf(glm::radians<float>(lights[1].cosCutoff)));
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], cosf(glm::radians<float>(lights[1].cosInner)));
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], lights[1].exponent);

	enableLight = true;

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0.f, 0.f, 0.f));
	btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0.f, 1.0f, 0.f), 0);
	btMotionState* motion = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo info(0.f, motion, plane);
	info.m_restitution = 0.8f;
	info.m_friction = 0.5f;

	fov = 45.0f;

	// Create objects in scene:
	{
		objInScene[FLOOR] = new DS_Floor;
		objInScene[BOOTH_WALL] = new DS_BoothWall;
		objInScene[BOOTH_ROOF] = new DS_BoothRoof;
		objInScene[CONVEYOR] = new DS_Conveyor;
		objInScene[PILLAR] = new DS_Pillar;
		objInScene[PILLAR2] = new DS_Pillar;
		objInScene[DUCK] = new DS_Duck;
		objInScene[BULLET] = new DS_Bullet;
	}

	// Modify objects in scene:
	{
		// Pillars:
		objInScene[PILLAR]->m_transform.Translate(24.3538 * 0.5 + 2.5 * 0.5, 5, -4.78361);
		objInScene[PILLAR2]->m_transform.Translate(-24.3538 * 0.5 - 2.5 * 0.5, 5, -4.78361);

		

		//// Counters:
		//{
		//	objInScene[COUNTER]->m_transform.Translate(0, 2.65 * 0.5, 4.84);
		//	objInScene[COUNTER]->m_transform.ScaleBy(11.4612, 2.65, 1);

		//	objInScene[COUNTER2]->m_transform.Translate(5.25, 2.65 * 0.5, -0.4);
		//	objInScene[COUNTER2]->m_transform.ScaleBy(1, 2.65, 11);

		//	objInScene[COUNTER3]->m_transform.Translate(-5.25, 2.65 * 0.5, -0.4);
		//	objInScene[COUNTER3]->m_transform.ScaleBy(1, 2.65, 11);
		//}
	}

	GameObjectManager::GetInstance()->IniAll();

	// Ducks Collision Ignore:
	{
		// Ducks:
		objInScene[DUCK]->rb->setIgnoreCollisionCheck(objInScene[PILLAR]->rb, true);
		objInScene[DUCK]->rb->setIgnoreCollisionCheck(objInScene[PILLAR2]->rb, true);
		objInScene[DUCK]->rb->setIgnoreCollisionCheck(objInScene[CONVEYOR]->rb, true);
	}

	bulletSpeed = 20;
	cooldownTimer = 0;
	gameComplete = false;
}

void SceneDuckShooting::Update()
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
	//{
	//	glm::vec3 toObject = glm::normalize(glm::vec3(0, 3, 0) - mainCamera.m_transform.m_position);

	//	glm::vec3 lookVector = glm::normalize(mainCamera.target - mainCamera.m_transform.m_position);

	//	float dotProduct = glm::dot(lookVector, toObject);
	//	float threshold = glm::cos(glm::radians(fov * 0.5));

	//	if (dotProduct <= threshold) // Rotated too much
	//	{
	//		mainCamera.target = prevTarget;
	//	}
	//	else {
	//		float closeness = (dotProduct - threshold) / (1.0f - threshold);
	//		mainCamera.sensitivity = 10 + closeness * (50 - 10);
	//	}
	//}

	float speed = 20 * Time::deltaTime;
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
		devVec.y -= speed;

	//lights[0].m_transform.m_position = devVec;
	std::cout << devVec.x << ", " << devVec.y << ", " << devVec.z << std::endl;




	GameObjectManager::GetInstance()->UpdateAll();
}

void SceneDuckShooting::LateUpdate()
{
	// View Model:
	{
		glm::vec3 cameraPos = mainCamera.m_transform.m_position;
		glm::vec3 forward = mainCamera.view;
		glm::vec3 right = mainCamera.right;
		glm::vec3 up = mainCamera.up;

		glm::vec3 gunOffset = right * 0.2f + up * -0.15f + forward * 0.45f;
		gunTransform.m_position = cameraPos + gunOffset;

		glm::vec3 direction = glm::normalize(mainCamera.target - cameraPos);

		glm::vec3 gunRotation;
		gunRotation.y = atan2(direction.x, direction.z);
		gunRotation.x = asin(-direction.y);
		gunTransform.m_rotation = gunRotation;
	}


	if (cooldownTimer > 0) {
		cooldownTimer -= Time::deltaTime;
	}
	else {
		cooldownTimer = 0;
	}

	if (gameComplete == false) 
	{
		if (cooldownTimer <= 0) {
			if (MouseController::GetInstance()->IsButtonReleased(0))
			{
				cooldownTimer = 3;

				// Shoot:
				std::cout << "Pew" << std::endl;

			//	if (objInScene[BULLET] == nullptr)
			//		objInScene[BULLET] = new DS_Bullet;

				glm::vec3 bulletOffset = mainCamera.right * 0.165f + mainCamera.up * -0.075f + mainCamera.view * 1.f;
				glm::vec3 origin = mainCamera.m_transform.m_position + bulletOffset;
				glm::vec3 direction = glm::normalize(mainCamera.target - origin);

				objInScene[BULLET]->rb->setSleepingThresholds(0.8, 1);
				objInScene[BULLET]->SetRigidbodyPosition(origin);
				objInScene[BULLET]->rb->setLinearVelocity(btVector3(direction.x, direction.y, direction.z) * bulletSpeed);
				objInScene[BULLET]->rb->activate();

			}
		}
	}

	GameObjectManager::GetInstance()->LateUpdateAll();
}

void SceneDuckShooting::Render()
{
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

		if (lights[1].type == Light::LIGHT_DIRECTIONAL)
		{
			glm::vec3 lightDir(lights[1].m_transform.m_position.x, lights[1].m_transform.m_position.y, lights[1].m_transform.m_position.z);
			glm::vec3 lightDirection_cameraspace = viewStack.Top() * glm::vec4(lightDir, 0);
			glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, glm::value_ptr(lightDirection_cameraspace));
		}
		else if (lights[1].type == Light::LIGHT_SPOT)
		{
			glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(lights[1].m_transform.m_position, 1);
			glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
			glm::vec3 spotDirection_cameraspace = viewStack.Top() * glm::vec4(lights[1].spotDirection, 0);
			glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, glm::value_ptr(spotDirection_cameraspace));
		}
		else {
			// Calculate the light position in camera space
			glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(lights[1].m_transform.m_position, 1);
			glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
		}

		modelStack.PushMatrix();
		RenderMesh(meshList[GEO_AXIS]);
		modelStack.PopMatrix();
	}
	
	// Light:
	{
		modelStack.PushMatrix();
		modelStack.Translate(lights[0].m_transform.m_position.x, lights[0].m_transform.m_position.y, lights[0].m_transform.m_position.z);
		//RenderMesh(meshList[GEO_LIGHT], !enableLight);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(lights[1].m_transform.m_position.x, lights[1].m_transform.m_position.y, lights[1].m_transform.m_position.z);
		//RenderMesh(meshList[GEO_LIGHT], !enableLight);
		modelStack.PopMatrix();
	}
	
	// Tent:
	{
		modelStack.PushMatrix();
		modelStack.Translate(0,0,0);
		modelStack.Scale(objInScene[BOOTH_ROOF]->m_transform.m_scale.x, objInScene[BOOTH_ROOF]->m_transform.m_scale.y, objInScene[BOOTH_ROOF]->m_transform.m_scale.z);
		RenderMesh(meshList[GEO_BOOTHROOF], enableLight);
		RenderMesh(meshList[GEO_BOOTHGUARDS], enableLight);
		modelStack.PopMatrix();
	}

	// Render lamp:
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.302714, 11.9211, 0.0587656);
		modelStack.Rotate(180, 1, 0, 0);
		modelStack.Scale(7, 7, 7);
		RenderMesh(meshList[GEO_LIGHTBULB], false);
		modelStack.PopMatrix();
	}

	// Gun:
	{
		modelStack.PushMatrix();
		modelStack.Translate(gunTransform.m_position.x, gunTransform.m_position.y, gunTransform.m_position.z);
		modelStack.Rotate(glm::degrees(gunTransform.m_rotation.y), 0, 1, 0); // Yaw
		modelStack.Rotate(glm::degrees(gunTransform.m_rotation.x), 1, 0, 0); // Pitch
		RenderMesh(meshList[GEO_GUN], enableLight);
		modelStack.PopMatrix();
	}

	// placeholder:
	{
		//modelStack.PushMatrix();

		//glm::vec3 bulletOffset = mainCamera.right * 0.165f + mainCamera.up * -0.075f + mainCamera.view * 1.f;
		//glm::vec3 origin = mainCamera.m_transform.m_position + bulletOffset;
		//glm::vec3 direction = glm::normalize(mainCamera.target - origin);

		//glm::vec3 rotation;
		//rotation.y = glm::degrees(atan2(direction.x, direction.z)); // Yaw
		//rotation.x = glm::degrees(asin(-direction.y)) - 90;              // Pitch

		//modelStack.Translate(origin.x, origin.y, origin.z);
		//modelStack.Scale(0.05, 0.05, 0.05);
		//RenderMesh(meshList[GEO_BULLET], true);
		//modelStack.PopMatrix();
	}

	// Render hitboxes:
	GameObjectManager::GetInstance()->RenderAll(*this);

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

void SceneDuckShooting::Exit()
{
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();
	ColliderManager::GetInstance()->ClearAll();

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneDuckShooting::RenderSkybox(void)
{
	float size = 50.0f;
	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.f, 0.f, -5.f * size);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.f, 0.f, 5.f * size);
	modelStack.Rotate(180.0f, 0, 1, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(5.0f * size, 0.f, 0.0f);
	modelStack.Rotate(-90.0f, 0, 1, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(-5.0f * size, 0.f, 0.0f);
	modelStack.Rotate(90.0f, 0, 1, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.0f, 5.0f * size, 0.0f);
	modelStack.Rotate(90.0f, 1, 0, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.0f, -5.0f * size, 0.0f);
	modelStack.Rotate(-90.0f, 1, 0, 0);
	modelStack.Scale(size, size, 1.0f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();
}

void SceneDuckShooting::RenderMeshOnScreen(Mesh* mesh, float x, float y, float width, float height, glm::vec2 anchorPoint)
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
