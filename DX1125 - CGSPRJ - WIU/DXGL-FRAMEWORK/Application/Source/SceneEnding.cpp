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

SceneEnding::SceneEnding() : numLight{ 1 }, sceneTimer{ 0.0f }
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
		m_parameters[U_LIGHT0_TYPE + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].type");
		m_parameters[U_LIGHT0_POSITION + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
		m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].color");
		m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].power");
		m_parameters[U_LIGHT0_KC + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].kC");
		m_parameters[U_LIGHT0_KL + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].kL");
		m_parameters[U_LIGHT0_KQ + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].kQ");
		m_parameters[U_LIGHT0_SPOTDIRECTION + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
		m_parameters[U_LIGHT0_COSCUTOFF + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
		m_parameters[U_LIGHT0_COSINNER + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].cosInner");
		m_parameters[U_LIGHT0_EXPONENT + U_LIGHT0_EXPONENT * i] = glGetUniformLocation(m_programID, "lights[0].exponent");
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
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", WHITE, 75.0f);
	meshList[GEO_PLANE]->textureID = LoadPNG("Images//ground.png");
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

	mainCamera.Init(glm::vec3(0, 6, 6), glm::vec3(0, 6, 0), VECTOR3_UP);
	
	glUniform1i(m_parameters[U_NUMLIGHTS], numLight);

	lights[0].m_transform.m_position = glm::vec3{};
	lights[0].m_transform.m_position = glm::vec3(0, 5, 0);
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_SPOT;
	lights[0].power = 1;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = 45.f;
	lights[0].cosInner = 30.f;
	lights[0].exponent = 3.f;
	lights[0].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	for (int i = 0; i < numLight; ++i)
	{
		glUniform3fv(m_parameters[U_LIGHT0_COLOR + U_LIGHT0_EXPONENT * i], 1, &lights[0].color.r);
		glUniform1i(m_parameters[U_LIGHT0_TYPE + U_LIGHT0_EXPONENT * i], lights[0].type);
		glUniform1f(m_parameters[U_LIGHT0_POWER + U_LIGHT0_EXPONENT * i], lights[0].power);
		glUniform1f(m_parameters[U_LIGHT0_KC + U_LIGHT0_EXPONENT * i], lights[0].kC);
		glUniform1f(m_parameters[U_LIGHT0_KL + U_LIGHT0_EXPONENT * i], lights[0].kL);
		glUniform1f(m_parameters[U_LIGHT0_KQ + U_LIGHT0_EXPONENT * i], lights[0].kQ);
		glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF + U_LIGHT0_EXPONENT * i], cosf(glm::radians<float>(lights[0].cosCutoff)));
		glUniform1f(m_parameters[U_LIGHT0_COSINNER + U_LIGHT0_EXPONENT * i], cosf(glm::radians<float>(lights[0].cosInner)));
		glUniform1f(m_parameters[U_LIGHT0_EXPONENT + U_LIGHT0_EXPONENT * i], lights[0].exponent);
	}

	enableLight = true;

	PhysicsMaterial groundMat;
	groundMat.m_bounciness = 0.5f;
	groundMat.m_friction = 0.5f;
	objInScene[GROUND] = new GameObject();
	objInScene[GROUND]->rb = addStaticPlane(objInScene[GROUND], VECTOR3_UP, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[GROUND]);
}

void SceneEnding::Update()
{
	sceneTimer += Time::deltaTime;
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

	if (isTimeReach(sceneTimer, 0.0f, 1.0f))
	{

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

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 1.0f, 0.0f);
	modelStack.Scale(5, 5, 5);
	RenderMesh(meshList[GEO_CLOWN_POSE_1], enableLight);
	modelStack.PopMatrix();

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