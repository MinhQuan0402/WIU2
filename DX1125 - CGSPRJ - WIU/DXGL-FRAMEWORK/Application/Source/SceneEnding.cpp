#define _USE_MATH_DEFINES
#include "SceneEnding.h"
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

SceneEnding::SceneEnding()
	: numLight{ 6 }, sceneTimer{ 0.0f },
	sceneTimerN{ 0.0f }, currentCameraTarget{},
	timerOffset{}, currentClownPose{ GEO_CLOWN_POSE_1 },
	ballonPos{ 0.0f, 15.0f, 0.0f }, lightSpeed{ 1.0f }, pennywisePos{0.0f, 0.8f, 0.0f},
	rotationXAngle{90.0f}
{
	meshList.resize(NUM_GEOMETRY);
	lights.resize(numLight);
}

SceneEnding::~SceneEnding()
{
}

void SceneEnding::Init()
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

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
		m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);

	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", WHITE, 10.0f);
	meshList[GEO_PLANE]->textureID = LoadPNG("Images//ground.png");
	meshList[GEO_PLANE]->material = Material::Wood(WHITE);
	meshList[GEO_PLANE]->material.kAmbient = glm::vec3{};

	meshList[GEO_SPHERE] = MeshBuilder::GenerateHemisphere("Sphere", BLACK, 50.0f, 50.0f, 1.0f);

	meshList[GEO_TENT] = MeshBuilder::GenerateOBJ("Dust tent", "Models//tent.obj");
	meshList[GEO_TENT]->textureID = LoadPNG("Images//tent.png");
	meshList[GEO_TENT]->material = Material::Wood(WHITE);
	meshList[GEO_TENT]->material.kAmbient = glm::vec3{};

	meshList[GEO_BENCH] = MeshBuilder::GenerateOBJ("Bench", "Models//bench_seats.obj");
	meshList[GEO_BENCH]->textureID = LoadPNG("Images//wood_bench.png");
	meshList[GEO_BENCH]->material = Material::Wood(WHITE);
	meshList[GEO_BENCH]->material.kAmbient = glm::vec3{};

	meshList[GEO_BENCH_POLE] = MeshBuilder::GenerateOBJ("Bench pole", "Models//bench_pole.obj");
	meshList[GEO_BENCH_POLE]->textureID = LoadPNG("Images//wood_bench.png");
	meshList[GEO_BENCH_POLE]->material = Material::Wood(WHITE);
	meshList[GEO_BENCH_POLE]->material.kAmbient = glm::vec3{};

	meshList[GEO_UNDERSEATS] = MeshBuilder::GenerateOBJ("Bench underseats", "Models//bench_underseats.obj");
	meshList[GEO_UNDERSEATS]->textureID = LoadPNG("Images//rusted_metal_bench_underseats.png");
	meshList[GEO_UNDERSEATS]->material = Material::Metal(WHITE);
	meshList[GEO_UNDERSEATS]->material.kAmbient = glm::vec3{};

	meshList[GEO_BENCH_STAIRS] = MeshBuilder::GenerateOBJ("stairs", "Models//bench_stairs.obj");
	meshList[GEO_BENCH_STAIRS]->textureID = LoadPNG("Images//wood_bench_stairs.png");
	meshList[GEO_BENCH_STAIRS]->material = Material::Wood(WHITE);
	meshList[GEO_BENCH_STAIRS]->material.kAmbient = glm::vec3{};

	meshList[GEO_CIRCLE_STAGE] = MeshBuilder::GenerateOBJ("circle stage", "Models//tent_circle_stage.obj");
	meshList[GEO_CIRCLE_STAGE]->textureID = LoadPNG("Images//wood_circle_stage.png");
	meshList[GEO_CIRCLE_STAGE]->material = Material::Wood(WHITE);
	meshList[GEO_CIRCLE_STAGE]->material.kAmbient = glm::vec3{};

	meshList[GEO_STAGE_RING_LOWER] = MeshBuilder::GenerateOBJ("ring", "Models//tent_stage_ring_lower.obj");
	meshList[GEO_STAGE_RING_LOWER]->textureID = LoadPNG("Images//stage_ring_lower.png");
	meshList[GEO_STAGE_RING_LOWER]->material = Material::Plastic(WHITE);
	meshList[GEO_STAGE_RING_LOWER]->material.kAmbient = glm::vec3{};

	meshList[GEO_STAGE_RING_UPPER] = MeshBuilder::GenerateOBJMTL("ring", "Models//tent_stage_ring_upper.obj", "Models//tent_stage_ring_upper.mtl");

	meshList[GEO_STAGE_POLE] = MeshBuilder::GenerateOBJ("pole", "Models//stage_pole.obj");
	meshList[GEO_STAGE_POLE]->textureID = LoadPNG("Images//rusted_metal_bench_underseats.png");
	meshList[GEO_STAGE_POLE]->material = Material::Metal(WHITE);
	meshList[GEO_STAGE_POLE]->material.kAmbient = glm::vec3{};

	meshList[GEO_TENT_POLE] = MeshBuilder::GenerateOBJ("Pole", "Models//tent_pole.obj");
	meshList[GEO_TENT_POLE]->textureID = LoadPNG("Images//wood_bench.png");
	meshList[GEO_TENT_POLE]->material = Material::Wood(WHITE);
	meshList[GEO_TENT_POLE]->material.kAmbient = glm::vec3{};

	meshList[GEO_STAGE_HANGING_POLE] = MeshBuilder::GenerateOBJMTL("hanging pole", "Models//stage_hanging_pole.obj", "Models//stage_hanging_pole.mtl");

	meshList[GEO_STAGE_STRING] = MeshBuilder::GenerateOBJ("String", "Models//stage_string.obj");
	meshList[GEO_STAGE_STRING]->textureID = LoadPNG("Images//stage_string.png");
	meshList[GEO_STAGE_STRING]->material = Material::Wood(WHITE);
	meshList[GEO_STAGE_STRING]->material.kAmbient = glm::vec3{};

	meshList[GEO_STAGE_HOOK] = MeshBuilder::GenerateOBJ("String", "Models//stage_hook.obj");
	meshList[GEO_STAGE_HOOK]->textureID = LoadPNG("Images//rusted_metal_bench_underseats.png");
	meshList[GEO_STAGE_HOOK]->material = Material::Plastic(WHITE);
	meshList[GEO_STAGE_HOOK]->material.kAmbient = glm::vec3{};

	meshList[GEO_STAGE_HULA_HOOP] = MeshBuilder::GenerateOBJMTL("Hula hoop", "Models//stage_hula_hoop.obj", "Models//stage_hula_hoop.mtl");

	meshList[GEO_DEC_SPHERE] = MeshBuilder::GenerateOBJMTL("Hula hoop", "Models//tent_dec_sphere.obj", "Models//tent_dec_sphere.mtl");

	meshList[GEO_DEC_STRIP] = MeshBuilder::GenerateOBJ("String", "Models//tent_dec_strip.obj");
	meshList[GEO_DEC_STRIP]->textureID = LoadPNG("Images//stage_dec_strip.png");
	meshList[GEO_DEC_STRIP]->material = Material::Wood(WHITE);
	meshList[GEO_DEC_STRIP]->material.kAmbient = glm::vec3{};

	meshList[GEO_CEILING_RING] = MeshBuilder::GenerateOBJ("Ceiling ring", "Models//tent_ceiling_ring.obj");
	meshList[GEO_CEILING_RING]->textureID = LoadPNG("Images//wood_bench.png");
	meshList[GEO_CEILING_RING]->material = Material::Wood(WHITE);
	meshList[GEO_CEILING_RING]->material.kAmbient = glm::vec3{};

	meshList[GEO_DEC_RING] = MeshBuilder::GenerateOBJ("Outer ring", "Models//tent_dec_ring.obj");
	meshList[GEO_DEC_RING]->textureID = LoadPNG("Images//stage_dec_strip.png");
	meshList[GEO_DEC_RING]->material = Material::Wood(WHITE);
	meshList[GEO_DEC_RING]->material.kAmbient = glm::vec3{};

	meshList[GEO_CLOWN_POSE_1] = MeshBuilder::GenerateOBJ("Outer ring", "Models//clown_pose_1.obj");
	meshList[GEO_CLOWN_POSE_1]->textureID = LoadPNG("Images//clown_pose_1.png");
	meshList[GEO_CLOWN_POSE_1]->material = Material::Wood(WHITE);
	meshList[GEO_CLOWN_POSE_1]->material.kAmbient = glm::vec3{};

	meshList[GEO_CLOWN_POSE_2] = MeshBuilder::GenerateOBJ("Outer ring", "Models//clown_pose_2.obj");
	meshList[GEO_CLOWN_POSE_2]->textureID = LoadPNG("Images//clown_pose_2.png");
	meshList[GEO_CLOWN_POSE_2]->material = Material::Wood(WHITE);
	meshList[GEO_CLOWN_POSE_2]->material.kAmbient = glm::vec3{};

	meshList[GEO_CLOWN_POSE_3] = MeshBuilder::GenerateOBJ("Outer ring", "Models//clown_pose_3.obj");
	meshList[GEO_CLOWN_POSE_3]->textureID = LoadPNG("Images//clown_pose_3.png");
	meshList[GEO_CLOWN_POSE_3]->material = Material::Wood(WHITE);
	meshList[GEO_CLOWN_POSE_3]->material.kAmbient = glm::vec3{};

	meshList[GEO_BALLON_STRING] = MeshBuilder::GenerateOBJMTL("Ballon string", "Models//ballon_string.obj", "Models//ballon_string.mtl");
	meshList[GEO_BALLON] = MeshBuilder::GenerateOBJ("Ballon", "Models//ballon.obj");
	meshList[GEO_BALLON]->textureID = LoadPNG("Images//ballon.png");
	meshList[GEO_BALLON]->material = Material::Wood(WHITE);
	meshList[GEO_BALLON]->material.kAmbient = glm::vec3{};

	meshList[GEO_PENNYWISE_BODY] = MeshBuilder::GenerateOBJ("Body", "Models//pennywise_body.obj");
	meshList[GEO_PENNYWISE_BODY]->textureID = LoadPNG("Images//pennywise_body.png");
	meshList[GEO_PENNYWISE_BODY]->material = Material::Wood(WHITE);
	meshList[GEO_PENNYWISE_BODY]->material.kAmbient = glm::vec3{};

	meshList[GEO_PENNYWISE_FACE] = MeshBuilder::GenerateOBJ("Ballon", "Models//pennywise_face.obj");
	meshList[GEO_PENNYWISE_FACE]->textureID = LoadPNG("Images//pennywise_face.png");
	meshList[GEO_PENNYWISE_FACE]->material = Material::Wood(WHITE);
	meshList[GEO_PENNYWISE_FACE]->material.kAmbient = glm::vec3{};

	meshList[GEO_PENNYWISE_TEETH] = MeshBuilder::GenerateOBJ("Ballon", "Models//pennywise_teeth.obj");
	meshList[GEO_PENNYWISE_TEETH]->textureID = LoadPNG("Images//pennywise_teeth.png");
	meshList[GEO_PENNYWISE_TEETH]->material = Material::Wood(WHITE);
	meshList[GEO_PENNYWISE_TEETH]->material.kAmbient = glm::vec3{};

	meshList[GEO_PENNYWISE_HAIR] = MeshBuilder::GenerateOBJ("Ballon", "Models//pennywise_hair.obj");
	meshList[GEO_PENNYWISE_HAIR]->textureID = LoadPNG("Images//pennywise_hair.png");
	meshList[GEO_PENNYWISE_HAIR]->material = Material::Wood(WHITE);
	meshList[GEO_PENNYWISE_HAIR]->material.kAmbient = glm::vec3{};

	meshList[GEO_PENNYWISE_EYES] = MeshBuilder::GenerateOBJ("Ballon", "Models//pennywise_eyes.obj");
	meshList[GEO_PENNYWISE_EYES]->textureID = LoadPNG("Images//pennywise_eyes.png");
	meshList[GEO_PENNYWISE_EYES]->material = Material::Wood(WHITE);
	meshList[GEO_PENNYWISE_EYES]->material.kAmbient = glm::vec3{};

	mainCamera.Init(glm::vec3(12.1513f, 2.23612f, -0.0855163), glm::vec3(10.1522f, 2.29354f, -0.0610635f), VECTOR3_UP);
	
	glUniform1i(m_parameters[U_NUMLIGHTS], numLight);

	lights[0].m_transform.m_position = glm::vec3{0.f, 7.0f, 0.0f};
	lights[2].m_transform.m_position = glm::vec3{0.0f, 20.0f, 0.0f};
	lights[4].m_transform.m_position = glm::vec3{ 0.0f, 20.0f, 0.0f };

	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_SPOT;
	lights[0].kC = 1.0f;
	lights[0].kL = 0.0005f;
	lights[0].kQ = 0.00005f;
	lights[0].power = 0.0f;
	lights[0].cosCutoff = 35.0f;
	lights[0].cosInner = 30.f;
	lights[0].exponent = 3.0f;
	lights[0].spotDirection = glm::vec3(0.f, 1.f, 0.f);
	lights[0].isOff = true;
	
	for (int i = 1; i < numLight; ++i)
	{
		lights[i].color = glm::vec3(1, 1, 1);
		lights[i].type = Light::LIGHT_SPOT;
		lights[i].kC = 1.0f;
		lights[i].kL = 0.0005f;
		lights[i].kQ = 0.00005f;
		lights[i].power = 3.0f;
		lights[i].cosCutoff = 75.0f;
		lights[i].cosInner = 60.f;
		lights[i].exponent = 3.0f;
	}

	lights[1].color = glm::vec3(1, 1, 0);
	lights[2].color = glm::vec3(1, 1, 0);

	lights[3].color = glm::vec3(0.8f, 0, 0);
	lights[4].color = glm::vec3(0.8f, 0, 0);

	lights[5].color = glm::vec3(1, 1, 1);
	lights[5].type = Light::LIGHT_SPOT;
	lights[5].kC = 1.0f;
	lights[5].kL = 0.0005f;
	lights[5].kQ = 0.00005f;
	lights[5].power = 0.0f;
	lights[5].cosCutoff = 35.0f;
	lights[5].cosInner = 10.f;
	lights[5].exponent = 1.0f;
	lights[5].spotDirection = glm::vec3(0.f, 1.f, 0.f);
	lights[5].isOff = true;

	for (int i = 0; i < numLight; ++i)
	{
		glUniform3fv(m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * i], 1, &lights[i].color.r);
		glUniform1i(m_parameters[U_LIGHT0_TYPE + U_LIGHT0_EXPONENT * i], lights[i].type);
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
	groundMat.m_bounciness = 0.5f;
	groundMat.m_friction = 0.5f;
	objInScene[GROUND] = new GameObject;
	objInScene[GROUND]->rb = addStaticPlane(objInScene[GROUND], VECTOR3_UP, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[GROUND]);
}

void SceneEnding::Update()
{
	sceneTimer += Time::deltaTime;
	sceneTimerN -= Time::deltaTime;
	HandleKeyPress();

	if (isTimeReach(sceneTimer, 0.0f, 9.0f) || isTimeReach(sceneTimer, 11.0f, 20.0f) 
		|| isTimeReach(sceneTimer, 22.0f, 31.0f) || isTimeReach(sceneTimer, 33.0f, 48.0f)
		|| isTimeReach(sceneTimer, 50.0f, 55.0f))
	{
		float RADIUS = 9.0f;
		glm::vec3 lightTarget1 = glm::vec3(std::sin(sceneTimer * lightSpeed) * RADIUS, 1.0f, std::cos(sceneTimer * lightSpeed) * RADIUS);

		RADIUS = 5.0f;
		lights[1].m_transform.m_position = glm::vec3(std::sin(sceneTimer * lightSpeed) * RADIUS, 5.0f, std::cos(sceneTimer * lightSpeed) * RADIUS);
		glm::vec3 lightDir = glm::normalize(lightTarget1 - lights[1].m_transform.m_position);
		lights[1].spotDirection = -lightDir;

		RADIUS = 20.0f;
		lightTarget1 = glm::vec3(std::sin(sceneTimer * lightSpeed) * RADIUS, 20.0f, std::cos(sceneTimer * lightSpeed) * RADIUS);
		lightDir = glm::normalize(lightTarget1 - lights[2].m_transform.m_position);
		lights[2].spotDirection = -lightDir;

		RADIUS = 9.0f;
		lightTarget1 = glm::vec3(std::sin(sceneTimerN * lightSpeed) * RADIUS, 1.0f, std::cos(sceneTimerN * lightSpeed) * RADIUS);
		RADIUS = 5.0f;
		lights[3].m_transform.m_position = glm::vec3(std::sin(sceneTimerN * lightSpeed) * RADIUS, 5.0f, std::cos(sceneTimerN * lightSpeed) * RADIUS);
		lightDir = glm::normalize(lightTarget1 - lights[3].m_transform.m_position);
		lights[3].spotDirection = -lightDir;

		lightTarget1 = glm::vec3(std::sin(sceneTimerN * lightSpeed) * RADIUS, 20.0f, std::cos(sceneTimerN * lightSpeed) * RADIUS);
		lightDir = glm::normalize(lightTarget1 - lights[4].m_transform.m_position);
		lights[4].spotDirection = -lightDir;
	}

	if (isTimeReach(sceneTimer, 0.0f, 6.0f)) //Move cam view
	{
		glm::vec3 endTarget{};

		if (isTimeReach(sceneTimer, 0.0f, 1.0f))
		{
			currentCameraTarget = glm::vec3(10.15f, 2.3f, -0.06f);
			endTarget = glm::vec3(10.4f, 3.04f, 0.4f);

			if (sceneTimer >= 0.95f)
			{
				timerOffset = 1.0f;
				currentCameraTarget = endTarget;
			}
		}
		else if (isTimeReach(sceneTimer, 1.0f, 2.0f))
		{
			endTarget = glm::vec3(10.5f, 3.26f, -0.55f);

			if (sceneTimer >= 1.95f)
			{
				timerOffset = 2.0f;
				currentCameraTarget = endTarget;
			}
		}
		else if (isTimeReach(sceneTimer, 2.0f, 3.0f))
		{
			endTarget = glm::vec3(10.15f, 2.5f, -0.06f);

			if (sceneTimer >= 2.95f)
			{
				timerOffset = 3.0f;
				currentCameraTarget = endTarget;
			}
		}
		glm::vec3 newCameraTarget = Lerp(sceneTimer - timerOffset, currentCameraTarget, endTarget, 1.0f);
		mainCamera.target = newCameraTarget;
	}
	else if (isTimeReach(sceneTimer, 8.0f, 10.0f) && lights[0].isOff) //Turn middle light on
	{
		lights[0].power = 3.0f;
		glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
		lights[0].isOff = false;
	}
	else if (isTimeReach(sceneTimer, 10.0f, 10.05f)) //off all light
	{
		for (int i = 0; i < numLight; ++i)
		{
			lights[i].power = 0.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = true;
		}
	}
	else if (isTimeReach(sceneTimer, 11.0f, 11.05f)) //turn on other light
	{
		for (int i = 1; i < numLight; ++i)
		{
			lights[i].power = 3.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = false;
		}
	}
	else if (isTimeReach(sceneTimer, 19.f, 21.0f) && lights[0].isOff)
	{
		currentClownPose = GEO_CLOWN_POSE_2;
		lights[0].power = 3.0f;
		glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
		lights[0].isOff = false;
	}
	else if (isTimeReach(sceneTimer, 21.0f, 21.1f))
	{
		for (int i = 0; i < numLight; ++i)
		{
			lights[i].power = 0.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = true;
		}

	}
	else if (isTimeReach(sceneTimer, 22.0f, 22.1f)) //turn on other light
	{
		for (int i = 1; i < numLight; ++i)
		{
			lights[i].power = 3.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = false;
		}
	}
	else if (isTimeReach(sceneTimer, 31.0f, 33.0f) && lights[0].isOff)
	{
		currentClownPose = GEO_CLOWN_POSE_3;
		lights[0].power = 3.0f;
		glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
		lights[0].isOff = false;
	}
	else if (isTimeReach(sceneTimer, 33.0f, 33.1f))
	{
		for (int i = 0; i < numLight; ++i)
		{
			lights[i].power = 0.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = true;
		}

	}
	else if (isTimeReach(sceneTimer, 34.0f, 34.1f)) //turn on other light
	{
		for (int i = 1; i < numLight; ++i)
		{
			lights[i].power = 3.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = false;
		}
	}
	else if (isTimeReach(sceneTimer, 44.0f, 48.0f))
	{
		lightSpeed = 3.0f;
		if (lights[5].isOff)
		{
			lights[5].power = 1.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER] + U_LIGHT0_EXPONENT * 5, lights[5].power);
			lights[5].isOff = false;
		}

		glm::vec3 endTarget{};
		if (isTimeReach(sceneTimer, 44.0f, 45.0f))
		{
			endTarget = ballonPos;
			timerOffset = 44.0f;
			glm::vec3 newCameraTarget = Lerp(sceneTimer - timerOffset, currentCameraTarget, endTarget, 1.0f);
			mainCamera.target = newCameraTarget;

			if (sceneTimer >= 44.9f)
			{
				timerOffset = 45.0f;
				currentCameraTarget = endTarget;
			}
		}
		else if (isTimeReach(sceneTimer, 45.0f, 48.0f))
		{
			ballonPos = Lerp(sceneTimer - timerOffset, glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 3.0f);

			endTarget = glm::vec3(10.2f, 2.5f, -0.11f);
			mainCamera.target = Lerp(sceneTimer - timerOffset, currentCameraTarget, endTarget, 3.0f);

		}
		lights[5].m_transform.m_position = glm::vec3{ 0.0f, ballonPos.y + 10.0f, 0.0f };
	}
	else if (isTimeReach(sceneTimer, 48.0f, 48.1f) && !lights[5].isOff)
	{
		for (int i = 0; i < numLight; ++i)
		{
			lights[i].power = 0.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = true;
		}
	}
	else if (isTimeReach(sceneTimer, 50.0f, 50.5f) && lights[0].isOff)
	{
		lightSpeed = 5.0f;
		for (int i = 0; i < numLight; ++i)
		{
			lights[i].power = 3.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = false;
		}

		ballonPos.y = 2.0f;
		ballonPos.z = -1.0f;
		ballonPos.x = -1.0f;

		lightTarget1 = glm::vec3(pennywisePos.x, 13.0f, pennywisePos.z);
		lights[5].m_transform.m_position = glm::vec3{ pennywisePos.x + 2.0f, 0.0f, pennywisePos.z };
		glm::vec3 lightDir = glm::normalize(lightTarget1 - lights[5].m_transform.m_position);
		lights[5].spotDirection = -lightDir;
	}
	else if (isTimeReach(sceneTimer, 55.0f, 56.0f) && !lights[0].isOff)
	{
		for (int i = 0; i < numLight; ++i)
		{
			lights[i].power = 0.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[i].power);
			lights[i].isOff = true;
		}
	}
	else if (isTimeReach(sceneTimer, 57.0f, 58.0f))
	{
		if (lights[5].isOff)
		{
			lights[5].power = 3.0f;
			glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * 5], lights[5].power);
			lights[5].isOff = false;
		}

		pennywisePos = Lerp(sceneTimer - 57.0f, glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(mainCamera.m_transform.m_position.x - 1.0f, -1.4f, 0.0f), 0.15f);

		lightTarget1 = glm::vec3(pennywisePos.x, 13.0f, pennywisePos.z);
		lights[5].m_transform.m_position = glm::vec3{ pennywisePos.x + 2.0f, 0.0f, pennywisePos.z };
		glm::vec3 lightDir = glm::normalize(lightTarget1 - lights[5].m_transform.m_position);
		lights[5].spotDirection = -lightDir;
		rotationXAngle = 100.0f;
	}
	else if (isTimeReach(sceneTimer, 58.0f, 59.0f) && !lights[5].isOff)
	{
		lights[5].power = 0.0f;
		glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * 5], lights[5].power);
		lights[5].isOff = true;
	}

	if (sceneTimer >= 62.0f)
	{
		//SceneManager::GetInstance().ChangeState();
	}
}

void SceneEnding::LateUpdate()
{
}

void SceneEnding::Render()
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
			glm::vec3 lightDir(lights[i].m_transform.m_position);
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
	RenderMesh(meshList[GEO_TENT], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_BENCH], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_BENCH_POLE], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_UNDERSEATS], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_BENCH_STAIRS], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_CIRCLE_STAGE], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_STAGE_RING_LOWER], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_STAGE_RING_UPPER], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_STAGE_POLE], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_TENT_POLE], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_STAGE_HANGING_POLE], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_STAGE_STRING], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_STAGE_HOOK], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_STAGE_HULA_HOOP], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_DEC_SPHERE], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_DEC_STRIP], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_CEILING_RING], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_DEC_RING], enableLight);
	modelStack.PopMatrix();

	float textSize = 20.0f;
	std::string text{};
	float textOffset{};
	if (isTimeReach(sceneTimer, 0.0f, 6.0f))
	{
		textOffset = 200.0f;
		if(isTimeReach(sceneTimer, 0.0f, 3.0f))
			text = "WOOW! The show is so gorgeous :)";
		else if (isTimeReach(sceneTimer, 3.0f, 6.0f))
		{
			text = "Can't wait to see all the clowns";
			RenderTextOnScreen(meshList[GEO_TEXT], "in action", WHITE, textSize, Application::m_consoleWidth / 2.0f - 50.0f, Application::m_consoleHeight / 2.0f - 300.0f);
		}

		RenderTextOnScreen(meshList[GEO_TEXT], text, WHITE, textSize, Application::m_consoleWidth / 2.0f - textOffset, Application::m_consoleHeight / 2.0f - 270.0f);
	}
	else if (isTimeReach(sceneTimer, 11.0f, 19.0f))
	{
		if (isTimeReach(sceneTimer, 11.0f, 13.0f))
		{
			textOffset = 300.0f;
			text = "That's was a spetacular performance by Mr Clown Jonhy";
		}
		else if (isTimeReach(sceneTimer, 13.0f, 16.0f))
		{
			textOffset = 200.0f;
			text = "Oh Mr Clown Tommy is next!";
		}
		else if (isTimeReach(sceneTimer, 16.0f, 19.0f))
		{
			textOffset = 230.0f;
			text = "I'm so excited to see him on stage.";
		}
		RenderTextOnScreen(meshList[GEO_TEXT], text, WHITE, textSize, Application::m_consoleWidth / 2.0f - textOffset, Application::m_consoleHeight / 2.0f - 270.0f);
	}
	else if (isTimeReach(sceneTimer, 22.0f, 31.0f))
	{
		if (isTimeReach(sceneTimer, 22.5f, 23.5f))
		{
			textOffset = 30.0f;
			text = "???";
		}
		else if (isTimeReach(sceneTimer, 23.5f, 26.0f))
		{
			textOffset = 350.0f;
			text = "Why does Mr Clown Tommy's face seems to be scary today.";
		}
		else if (isTimeReach(sceneTimer, 26.5f, 28.0f))
		{
			textOffset = 200.0f;
			text = "Nevermind. I must be hallucinating .";
		}
		else if (isTimeReach(sceneTimer, 28.5f, 31.0f))
		{
			textOffset = 200.0f;
			text = "Yes! Mrs Clown Jessie is upcoming.";
		}
		RenderTextOnScreen(meshList[GEO_TEXT], text, WHITE, textSize, Application::m_consoleWidth / 2.0f - textOffset, Application::m_consoleHeight / 2.0f - 270.0f);
	}
	else if (isTimeReach(sceneTimer, 33.0f, 44.0f))
	{
		if (isTimeReach(sceneTimer, 35.0f, 36.0f))
		{
			textOffset = 30.0f;
			text = "...";
		}
		else if (isTimeReach(sceneTimer, 36.5f, 38.0f))
		{
			textOffset = 100.0f;
			text = "What is going!?";
		}
		else if (isTimeReach(sceneTimer, 38.0f, 41.0f))
		{
			textOffset = 200.0f;
			text = "Why Mrs Jessie has only one eye?.";
		}
		else if (isTimeReach(sceneTimer, 41.5f, 44.0f))
		{
			textOffset = 250.0f;
			text = "What's wrong with the clowns? I'm scared.";
		}
		RenderTextOnScreen(meshList[GEO_TEXT], text, WHITE, textSize, Application::m_consoleWidth / 2.0f - textOffset, Application::m_consoleHeight / 2.0f - 270.0f);
	}
	else if (isTimeReach(sceneTimer, 44.0f, 58.0f))
	{
		glm::vec3 color = WHITE;
		if (isTimeReach(sceneTimer, 44.0f, 47.0f))
		{
			textOffset = 50.0f;
			text = "Ballon???";
		}
		else if (isTimeReach(sceneTimer, 47.0f, 49.0f))
		{
			textOffset = 200.0f;
			text = "What is happening? Somebody help!";
		}

		RenderTextOnScreen(meshList[GEO_TEXT], text, color, textSize, Application::m_consoleWidth / 2.0f - textOffset, Application::m_consoleHeight / 2.0f - 270.0f);
	}
	else if (isTimeReach(sceneTimer, 58.0f, 59.0f))
	{
		textSize = 50.0;
		textOffset = 100.0f;
		text = "THE END";

		RenderTextOnScreen(meshList[GEO_TEXT], text, RED, textSize, Application::m_consoleWidth / 2.0f - textOffset, Application::m_consoleHeight / 2.0f);
	}
	
	if (isTimeReach(sceneTimer, 44.0f, 57.0f))
	{
		modelStack.PushMatrix();
		modelStack.LoadIdentity();
		modelStack.Translate(ballonPos.x, ballonPos.y, ballonPos.z);
		RenderMesh(meshList[GEO_BALLON_STRING], enableLight);
		RenderMesh(meshList[GEO_BALLON], enableLight);
		modelStack.Scale(5, 5, 5);
		modelStack.PopMatrix();
	}

	if (!lights[0].isOff)
	{
		if (sceneTimer < 50.0f)
		{
			modelStack.PushMatrix();
			modelStack.Translate(0.0f, 0.8f, 0.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(5, 5, 5);
			RenderMesh(meshList[currentClownPose], enableLight);
			modelStack.PopMatrix();
		}
	}
	
	if (sceneTimer > 50.0f && !lights[5].isOff)
		{
			modelStack.PushMatrix();
			modelStack.Translate(pennywisePos.x, pennywisePos.y, pennywisePos.z);
			modelStack.Rotate(rotationXAngle, 0, 1, 0);
			modelStack.Scale(1.15f, 1.15f, 1.15f);
			RenderPennywise();
			modelStack.PopMatrix();
		}
	
#ifdef DRAW_HITBOX

	modelStack.PushMatrix();
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

void SceneEnding::Exit()
{
	Scene::Exit();
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneEnding::RenderSkybox(void)
{
	float size = 20.0f;
	modelStack.PushMatrix();
	modelStack.Translate(0.0f, -0.01f, 0.0f);
	modelStack.Scale(size, size + 7.0f, size);
	RenderMesh(meshList[GEO_SPHERE], false);
	modelStack.PopMatrix();
}

void SceneEnding::RenderGround(int size)
{
	if (size % 2 == 0) size += 1;

	int valueToMul = (size - 1) / 2;
	glm::vec3 originPos = glm::vec3{ 10.0f * (float)valueToMul, 0.0f, 10.0f * (float)valueToMul };
	float orignalX = originPos.x;
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

			originPos.x -= 10.0f;
		}

		originPos.z -= 10.0f;
	}
}

void SceneEnding::RenderPennywise(void)
{
	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PENNYWISE_BODY], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PENNYWISE_FACE], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PENNYWISE_EYES], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PENNYWISE_TEETH], enableLight);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PENNYWISE_HAIR], enableLight);
	modelStack.PopMatrix();
}
