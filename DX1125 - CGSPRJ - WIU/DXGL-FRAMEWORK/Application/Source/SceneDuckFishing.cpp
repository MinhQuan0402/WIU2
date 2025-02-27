#define _USE_MATH_DEFINES
#include "SceneDuckFishing.h"
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

#include "Ducks.h"
#include "PoolWall.h"
#include "PoolFloor.h"
#include "SpinyWater.h"


SceneDuckFishing::SceneDuckFishing() : numLight{ 4 }
{
	meshList.resize(NUM_GEOMETRY);
	lights.resize(numLight);
}

SceneDuckFishing::~SceneDuckFishing()
{
}

void SceneDuckFishing::Init()
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

	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", YELLOW, 1.0f, 100, 100);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", GREY, 1.0f);
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("Cylinder", BLUE, 180, 1.0f, 1.0f);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", RED, 75.0f);
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

	meshList[GEO_FD_DUCKY] = MeshBuilder::GenerateOBJMTL("Duck", "Models//FD_Duckie.obj", "Models//FD_Duckie.mtl");

	meshList[GEO_FD_TENTFRAME] = MeshBuilder::GenerateOBJMTL("TENT FRAME", "Models//FD_CK_BoothGuards.obj", "Models//FD_CK_BoothGuards.mtl");
	meshList[GEO_FD_TENTFRAME]->material = Material::Metal(GREY);
	meshList[GEO_FD_TENTROOF] = MeshBuilder::GenerateOBJMTL("TENT Roof", "Models//FD_CK_BoothRoof.obj", "Models//FD_CK_BoothRoof.mtl");
	meshList[GEO_FD_TENTROOF]->textureID = LoadPNG("Images//FD_CK_BoothRoof.png");
	meshList[GEO_FD_TENTROOF]->material = Material::Wood(WHITE);
	meshList[GEO_FD_POOL] = MeshBuilder::GenerateOBJ("Pool", "Models//FD_pool.obj");
	meshList[GEO_FD_POOL]->textureID = LoadPNG("Images//FD_pool.png");
	meshList[GEO_FD_POOL]->material = Material::Metal(BLUE);

	meshList[GEO_FD_WATER] = MeshBuilder::GenerateCircle("Water", WHITE, 5);
	meshList[GEO_FD_WATER]->textureID = LoadPNG("Images//FD_water.png");
	meshList[GEO_FD_WATER]->material = Material::Plastic(BRIGHTBLUE);

	meshList[GEO_FD_DUCKBOTTOM] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_FD_DUCKBOTTOM]->textureID = LoadPNG("Images//FD_duckNumberBottom.png");

	/*meshList[GEO_FD_STEP_STOOL] = MeshBuilder::GenerateOBJ("StepStool", "Models//FD_step_stool.obj");
	meshList[GEO_FD_STEP_STOOL]->textureID = LoadPNG("Images//FD_step_stool.png");*/
	spotLightDuckPosition = glm::vec3(0, 0, 0);
	mainCamera.Init(glm::vec3(0, 10, 15), glm::vec3(0, 10, 0), VECTOR3_UP);
	mainCamera.sensitivity = 100;
	glUniform1i(m_parameters[U_NUMLIGHTS], numLight);

	lightPower = 0;

	lights[0].m_transform.m_position = glm::vec3{};
	lights[0].m_transform.m_position = glm::vec3(0, 23, 0);
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_POINT;
	lights[0].power = 1.5;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = 45.f;
	lights[0].cosInner = 30.f;
	lights[0].exponent = 3.f;
	lights[0].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	lights[1].m_transform.m_position = glm::vec3{};
	lights[1].m_transform.m_position = glm::vec3(spotLightDuckPosition.x, spotLightDuckPosition.y + 5, spotLightDuckPosition.z);
	lights[1].color = glm::vec3(1, 1, 1);
	lights[1].type = Light::LIGHT_SPOT;
	lights[1].power = lightPower; 
	lights[1].kC = 1.f;
	lights[1].kL = 0.01f;
	lights[1].kQ = 0.001f;
	lights[1].cosCutoff = 25.f;
	lights[1].cosInner = 10.f;
	lights[1].exponent = 3.f;
	lights[1].spotDirection = glm::vec3(0.f, 1.f, 0.f);
	for (int i = 2; i < numLight; ++i)
	{
		lights[i].color = glm::vec3(1, 1, 1);
		lights[i].type = Light::LIGHT_POINT;
		lights[i].power = 0.f;
		lights[i].kC = 0.5f;
		lights[i].kL = 0.01f;
		lights[i].kQ = 0.001f;
		lights[i].cosCutoff = 45.f;
		lights[i].cosInner = 30.f;
		lights[i].exponent = 3.f;
		lights[i].spotDirection = glm::vec3(0.f, 1.f, 0.f);
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
	//Application::showCursor = true;
	//Application::enableCursor = true;
	spinnerRotation = 0.0f;
	std::srand(std::time(0));
	float maxRadius = 12.0f;
	
	for (int i = 0; i < 20; i++)
	{
		float theta = (std::rand() / (float)RAND_MAX) * 2.0f * M_PI;

		float r = std::sqrt((std::rand() / (float)RAND_MAX)) * maxRadius;
		float rotationAngle = (std::rand() / (float)RAND_MAX) * 360.0f; 
		float x = r * std::cos(theta);
		float z = r * std::sin(theta);

		duckX[i] = x;
		duckZ[i] = z;
		duckRotation[i] = rotationAngle;
		
	}
	isPickable = true;
	isInView = false;
	PhysicsMaterial groundMat;
	groundMat.m_bounciness = 0.5f;
	groundMat.m_friction = 0.5f;
	objInScene[GROUND] = new GameObject();
	objInScene[GROUND]->rb = addStaticPlane(objInScene[GROUND], VECTOR3_UP, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[GROUND]);

	{
		objInScene[DUCK] = new Ducks();
		objInScene[DUCK]->m_transform.Translate(duckX[0], 7.f, duckZ[0]);
		objInScene[DUCK]->m_transform.Rotate(0.0f, duckRotation[0], 0.0f);

		objInScene[DUCK2] = new Ducks();
		objInScene[DUCK2]->m_transform.Translate(duckX[1], 7.f, duckZ[1]);
		objInScene[DUCK2]->m_transform.Rotate(0.0f, duckRotation[1], 0.0f);

		objInScene[DUCK3] = new Ducks();
		objInScene[DUCK3]->m_transform.Translate(duckX[2], 7.f, duckZ[2]);
		objInScene[DUCK3]->m_transform.Rotate(0.0f, duckRotation[2], 0.0f);

		objInScene[DUCK4] = new Ducks();
		objInScene[DUCK4]->m_transform.Translate(duckX[3], 7.f, duckZ[3]);
		objInScene[DUCK4]->m_transform.Rotate(0.0f, duckRotation[3], 0.0f);

		objInScene[DUCK5] = new Ducks();
		objInScene[DUCK5]->m_transform.Translate(duckX[4], 7.f, duckZ[4]);
		objInScene[DUCK5]->m_transform.Rotate(0.0f, duckRotation[4], 0.0f);

		objInScene[DUCK6] = new Ducks();
		objInScene[DUCK6]->m_transform.Translate(duckX[5], 7.f, duckZ[5]);
		objInScene[DUCK6]->m_transform.Rotate(0.0f, duckRotation[5], 0.0f);

		objInScene[DUCK7] = new Ducks();
		objInScene[DUCK7]->m_transform.Translate(duckX[6], 7.f, duckZ[6]);
		objInScene[DUCK7]->m_transform.Rotate(0.0f, duckRotation[6], 0.0f);

		objInScene[DUCK8] = new Ducks();
		objInScene[DUCK8]->m_transform.Translate(duckX[7], 7.f, duckZ[7]);
		objInScene[DUCK8]->m_transform.Rotate(0.0f, duckRotation[7], 0.0f);

		objInScene[DUCK9] = new Ducks();
		objInScene[DUCK9]->m_transform.Translate(duckX[8], 7.f, duckZ[8]);
		objInScene[DUCK9]->m_transform.Rotate(0.0f, duckRotation[8], 0.0f);

		objInScene[DUCK10] = new Ducks();
		objInScene[DUCK10]->m_transform.Translate(duckX[9], 7.f, duckZ[9]);
		objInScene[DUCK10]->m_transform.Rotate(0.0f, duckRotation[9], 0.0f);

		objInScene[DUCK11] = new Ducks();
		objInScene[DUCK11]->m_transform.Translate(duckX[10], 7.f, duckZ[10]);
		objInScene[DUCK11]->m_transform.Rotate(0.0f, duckRotation[10], 0.0f);

		objInScene[DUCK12] = new Ducks();
		objInScene[DUCK12]->m_transform.Translate(duckX[11], 7.f, duckZ[11]);
		objInScene[DUCK12]->m_transform.Rotate(0.0f, duckRotation[11], 0.0f);

		objInScene[DUCK13] = new Ducks();
		objInScene[DUCK13]->m_transform.Translate(duckX[12], 7.f, duckZ[12]);
		objInScene[DUCK13]->m_transform.Rotate(0.0f, duckRotation[12], 0.0f);

		objInScene[DUCK14] = new Ducks();
		objInScene[DUCK14]->m_transform.Translate(duckX[13], 7.f, duckZ[13]);
		objInScene[DUCK14]->m_transform.Rotate(0.0f, duckRotation[13], 0.0f);

		objInScene[DUCK15] = new Ducks();
		objInScene[DUCK15]->m_transform.Translate(duckX[14], 7.f, duckZ[14]);
		objInScene[DUCK15]->m_transform.Rotate(0.0f, duckRotation[14], 0.0f);

		objInScene[DUCK16] = new Ducks();
		objInScene[DUCK16]->m_transform.Translate(duckX[15], 7.f, duckZ[15]);
		objInScene[DUCK16]->m_transform.Rotate(0.0f, duckRotation[15], 0.0f);

		objInScene[DUCK17] = new Ducks();
		objInScene[DUCK17]->m_transform.Translate(duckX[16], 7.f, duckZ[16]);
		objInScene[DUCK17]->m_transform.Rotate(0.0f, duckRotation[16], 0.0f);

		objInScene[DUCK18] = new Ducks();
		objInScene[DUCK18]->m_transform.Translate(duckX[17], 7.f, duckZ[17]);
		objInScene[DUCK18]->m_transform.Rotate(0.0f, duckRotation[17], 0.0f);

		objInScene[DUCK19] = new Ducks();
		objInScene[DUCK19]->m_transform.Translate(duckX[18], 7.f, duckZ[18]);
		objInScene[DUCK19]->m_transform.Rotate(0.0f, duckRotation[18], 0.0f);

		objInScene[DUCK20] = new Ducks();
		objInScene[DUCK20]->m_transform.Translate(duckX[19], 7.f, duckZ[19]);
		objInScene[DUCK20]->m_transform.Rotate(0.0f, duckRotation[19], 0.0f);
	}

	objInScene[POOLWALL] = new PoolWall();
	objInScene[POOLWALL]->m_transform.Translate(0.0f, 2.f, 0.0f);
	objInScene[POOLWALL2] = new PoolWall();
	objInScene[POOLWALL2]->m_transform.Translate(0.0f, 5.f, 0.0f);
	objInScene[POOLWALL3] = new PoolWall();
	objInScene[POOLWALL3]->m_transform.Translate(0.0f, 7.f, 0.0f);

	{
		objInScene[POOL_SPINNER] = new SpinyWater();
		objInScene[POOL_SPINNER]->m_transform.Translate(0, 3.5, 0);
		objInScene[POOL_SPINNER]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);

		objInScene[POOL_SPINNER2] = new SpinyWater();
		objInScene[POOL_SPINNER2]->m_transform.Translate(9, 3.5, 0);
		objInScene[POOL_SPINNER2]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);

		objInScene[POOL_SPINNER3] = new SpinyWater();
		objInScene[POOL_SPINNER3]->m_transform.Translate(-9, 3.5, 0);
		objInScene[POOL_SPINNER3]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);

		objInScene[POOL_SPINNER4] = new SpinyWater();
		objInScene[POOL_SPINNER4]->m_transform.Translate(0, 3.5, 9);
		objInScene[POOL_SPINNER4]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);

		objInScene[POOL_SPINNER5] = new SpinyWater();
		objInScene[POOL_SPINNER5]->m_transform.Translate(0, 3.5, -9);
		objInScene[POOL_SPINNER5]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);

		objInScene[POOL_SPINNER6] = new SpinyWater();
		objInScene[POOL_SPINNER6]->m_transform.Translate(-9, 3.5, -9);
		objInScene[POOL_SPINNER6]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);

		objInScene[POOL_SPINNER7] = new SpinyWater();
		objInScene[POOL_SPINNER7]->m_transform.Translate(9, 3.5, 9);
		objInScene[POOL_SPINNER7]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);

		objInScene[POOL_SPINNER8] = new SpinyWater();
		objInScene[POOL_SPINNER8]->m_transform.Translate(-9, 3.5, 9);
		objInScene[POOL_SPINNER8]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);

		objInScene[POOL_SPINNER9] = new SpinyWater();
		objInScene[POOL_SPINNER9]->m_transform.Translate(9, 3.5, -9);
		objInScene[POOL_SPINNER9]->m_transform.Rotate(90.0f, spinnerRotation, 0.0f);
	}

	objInScene[POOLFLOOR] = new PoolFloor();
	//objInScene[POOLWALL3]->m_transform.Translate(0.0f, 7.f, 0.0f);

	/*PhysicsMaterial mat;
	mat.m_bounciness = 0.5f;
	mat.m_friction = 0.5f;
	objInScene[BOX] = new GameObject();
	objInScene[BOX]->m_transform.Translate(0.0f, 50.0f, 0.0f);
	objInScene[BOX]->m_transform.ScaleBy(4.f, 2.5f, 4.0f);
	objInScene[BOX]->rb = addBoxCollider(objInScene[BOX], 4.0f, 2.5f, 4.0f, mat);
	GameObjectManager::GetInstance()->addItem(objInScene[BOX]);

	objInScene[SPHERE] = new GameObject();
	objInScene[SPHERE]->m_transform.Translate(0.0f, 20.0f, 0.0f);
	objInScene[SPHERE]->m_transform.ScaleBy(5.0f, 5.0f, 5.0f);
	objInScene[SPHERE]->rb = addSphereCollider(objInScene[SPHERE], 5.0f, mat);
	GameObjectManager::GetInstance()->addItem(objInScene[SPHERE]);

	objInScene[CYLINDER] = new GameObject();
	objInScene[CYLINDER]->m_transform.Translate(0.0f, 80.0f, 0.0f);
	objInScene[CYLINDER]->m_transform.ScaleBy(0.5f, 2.0f, 0.5f);
	objInScene[CYLINDER]->rb = addCylinderCollider(objInScene[CYLINDER], 2.0f, 1.0f, mat);
	GameObjectManager::GetInstance()->addItem(objInScene[CYLINDER]);*/

	GameObjectManager::GetInstance()->IniAll();
	std::srand(std::time(nullptr)); // Seed with time
	time = 0;
	fov = 45.0f;

	} 

void SceneDuckFishing::Update()
{
	HandleKeyPress();
	mainCamera.Update();
	/*glm::vec3 inputMovementDir{};
	if (KeyboardController::GetInstance()->IsKeyDown('W'))
		inputMovementDir = mainCamera.view;
	if (KeyboardController::GetInstance()->IsKeyDown('S'))
		inputMovementDir = -mainCamera.view;
	if (KeyboardController::GetInstance()->IsKeyDown('D'))
		inputMovementDir = mainCamera.right;
	if (KeyboardController::GetInstance()->IsKeyDown('A'))
		inputMovementDir = -mainCamera.right;
	glm::vec3 finalForce = inputMovementDir * 10.0f * Time::deltaTime;
	mainCamera.m_transform.Translate(finalForce);*/

	spinnerRotation += 100* Time::deltaTime;
	
	objInScene[POOL_SPINNER]->SetRigidbodyRotation(0.0f, spinnerRotation, 0.0f);
	objInScene[POOL_SPINNER2]->SetRigidbodyRotation(0.0f, -spinnerRotation, 0.0f);
	objInScene[POOL_SPINNER3]->SetRigidbodyRotation(0.0f, -spinnerRotation, 0.0f);
	objInScene[POOL_SPINNER4]->SetRigidbodyRotation(0.0f, -spinnerRotation, 0.0f);
	objInScene[POOL_SPINNER5]->SetRigidbodyRotation(0.0f, spinnerRotation, 0.0f);
	objInScene[POOL_SPINNER6]->SetRigidbodyRotation(0.0f, -spinnerRotation, 0.0f);
	objInScene[POOL_SPINNER7]->SetRigidbodyRotation(0.0f, -spinnerRotation, 0.0f);
	objInScene[POOL_SPINNER8]->SetRigidbodyRotation(0.0f, spinnerRotation, 0.0f);
	objInScene[POOL_SPINNER9]->SetRigidbodyRotation(0.0f, -spinnerRotation, 0.0f);
	

	if(isSelected == false)
	{
		for (int i = 0; i < 20; ++i)
		{
			if (isObjectInteractable(objInScene[DUCK + i]->GetRigidbodyPosition(), mainCamera, 25.0f, 20.f))
			{
				isInView = true;
				//currentIndexRing = DUCK + i; 
				if (KeyboardController::GetInstance()->IsKeyPressed('F'))
				{

					//isShoot = false;
					isInView = false;
					if (std::rand() % 10 < 7) { // 70% chance (values 0-6 out of 0-9) 
						random_number = std::rand() % 49 + 1; // Range 1-49 
					}
					else { // 30% chance
						random_number = std::rand() % 51 + 50; // Range 50-100 
					}
					isSelected = true;
					break;
				}

				lightPower = 1;
				lights[1].power = lightPower;
				glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * 1], lights[1].power);
				spotLightDuckPosition = objInScene[DUCK + i]->GetRigidbodyPosition();
				lights[1].m_transform.m_position = glm::vec3(spotLightDuckPosition.x, spotLightDuckPosition.y + 5, spotLightDuckPosition.z);
				break;
			}
			else
			{
				isInView = false;
				lightPower = 0;
				lights[1].power = lightPower;
				glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * 1], lights[1].power);
			}

		}
	}
	else
	{
		time += Time::deltaTime;
	}

	lightTimer1 += Time::deltaTime; 
	if (isTimeReach(lightTimer1, 2.f, 2.05f))
	{
		glm::vec3 randColor = glm::vec3{
			Math::RandFloatMinMax(0.0f, 1.0f),
			Math::RandFloatMinMax(0.0f, 1.0f),
			Math::RandFloatMinMax(0.0f, 1.0f)
		};

		
		lights[0].color = randColor;
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	}
	else if (lightTimer1 >= 2.1f)
		lightTimer1 = 0.0f;

	

	
	glm::vec3 prevTarget = mainCamera.target;
	mainCamera.UpdateCameraRotation();
	// stop player rotating too far:
	{
		glm::vec3 toObject = glm::normalize(glm::vec3(0, 3, 0) - mainCamera.m_transform.m_position);

		glm::vec3 lookVector = glm::normalize(mainCamera.target - mainCamera.m_transform.m_position);

		float dotProduct = glm::dot(lookVector, toObject);
		float threshold = glm::cos(glm::radians(fov * 0.9));

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

void SceneDuckFishing::LateUpdate()
{
	
	
}

void SceneDuckFishing::Render()
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

	//for (int i = 0; i < numLight; ++i)
	//{
	//	modelStack.PushMatrix();
	//	modelStack.Translate(lights[i].m_transform.m_position.x, lights[i].m_transform.m_position.y, lights[i].m_transform.m_position.z);
	//	RenderMesh(meshList[GEO_LIGHT], false);
	//	modelStack.PopMatrix();
	//}

	RenderSkybox();
	RenderGround(7);


	modelStack.PushMatrix();
	modelStack.Scale(4.0f, 2.0f,4.0f);
	RenderMesh(meshList[GEO_FD_TENTFRAME],true);
	modelStack.PushMatrix();
	
	RenderMesh(meshList[GEO_FD_TENTROOF],true);
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 3.5, 0);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(2.5f, 2.5f, 1.0f);
	RenderMesh(meshList[GEO_FD_WATER], true); 
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(0, 4.5, 0);
	modelStack.Scale(0.7f, 0.7f, 0.7f);
	RenderMesh(meshList[GEO_FD_POOL],true);
	modelStack.PopMatrix();

	
	//modelStack.PushMatrix();
	//modelStack.Translate(0, 0.0, 15);
	//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
	//modelStack.Scale(1.5f, 1.5f, 1.5f);
	//RenderMesh(meshList[GEO_FD_STEP_STOOL]);
	//modelStack.PopMatrix();

	/*modelStack.PushMatrix();
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_PLANE]);
	modelStack.PopMatrix();*/

	/*RenderRigidMesh(meshList[GEO_SPHERE], false, objInScene[SPHERE]->m_transform, objInScene[SPHERE]->rb);
	RenderRigidMesh(meshList[GEO_CUBE], false, objInScene[BOX]->m_transform, objInScene[BOX]->rb);
	RenderRigidMesh(meshList[GEO_CYLINDER], false, objInScene[CYLINDER]->m_transform, objInScene[CYLINDER]->rb);*/

	
	GameObjectManager::GetInstance()->RenderAll(*this);
	if (isInView == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],"[F] to check duck", YELLOW, 50, 400, 350);
	}

	if (isSelected == true and time <=3)
		{	
		RenderMeshOnScreen(meshList[GEO_FD_DUCKBOTTOM], 570, 340, 50, 50);
		RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(random_number), WHITE, 100, 600, 300);
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
//			modelStack.Scale(shape->getLocalScaling().x(), shape->getLocalScaling().y(), shape->getLocalScaling().z());
//			RenderMesh(hitboxMeshList[HITBOX_SPHERE]);
//		}
//		else if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE)
//		{
//			BoxCollider* boxCollider = static_cast<BoxCollider*>(shape);
//			float width, height, depth;
//			boxCollider->GetDimension(width, height, depth);
//			modelStack.Scale(width, height, depth);
//			modelStack.Scale(shape->getLocalScaling().x(), shape->getLocalScaling().y(), shape->getLocalScaling().z());
//			RenderMesh(hitboxMeshList[HITBOX_BOX]);
//		}
//		else if (shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
//		{
//			CylinderCollider* cylinderCollider = static_cast<CylinderCollider*>(shape);
//			float rad, height;
//			cylinderCollider->GetDimension(rad, height);
//			modelStack.Scale(rad / 2.0f, height, rad / 2.0f);
//			modelStack.Scale(shape->getLocalScaling().x(), shape->getLocalScaling().y(), shape->getLocalScaling().z());
//			RenderMesh(hitboxMeshList[HITBOX_CYLINDER]);
//		}
//		else if (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
//		{
//			btCompoundShape* compoundShape = static_cast<btCompoundShape*>(shape);
//			for (unsigned i = 0; i < (unsigned)compoundShape->getNumChildShapes(); ++i)
//			{
//				btCollisionShape* childShape = compoundShape->getChildShape(i);
//				btTransform childTransform = compoundShape->getChildTransform(i);
//				float mat[16];
//				childTransform.getOpenGLMatrix(mat);
//
//				RenderChildCollider(childShape, mat);
//			}
//		}
//		else if (shape->getShapeType() == STATIC_PLANE_PROXYTYPE)
//		{
//			modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
//			RenderMesh(hitboxMeshList[HITBOX_GROUND]);
//		}
//		modelStack.PopMatrix();
//	}
//
//	if (isFillMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//
//#endif
}

void SceneDuckFishing::Exit()
{
	/*GameObjectManager::removeItem(objInScene[SPHERE]);*/
	Scene::Exit();
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneDuckFishing::RenderSkybox(void)
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
void SceneDuckFishing::RenderGround(int size)
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