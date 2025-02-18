#define _USE_MATH_DEFINES
#include "carnivalroaming.h"
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
#include "CarnivalHitBoxes.h"
carnivalroaming::carnivalroaming() : numLight{ 2 }
{
	meshList.resize(NUM_GEOMETRY);
	lights.resize(numLight);
}

carnivalroaming::~carnivalroaming()
{
}




void carnivalroaming::Init()
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

	meshList[GEO_HITBOX] = MeshBuilder::GenerateCube("HitBox", glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_MODEL1] = MeshBuilder::GenerateOBJ("Doorman", "Models//doorman.obj");
	meshList[GEO_MODEL1]->textureID = LoadPNG("Images//doorman.png");
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", WHITE, 1.0f, 100, 100);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", GREEN, 1.0f);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", RED, 1.0f);

	meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateCircle("Circle", WHITE, 200);
	meshList[GEO_SHOOTINGGAME] = MeshBuilder::GenerateQuad("Quad", WHITE, 1);

	mainCamera.Init(glm::vec3(8, 6, 6), glm::vec3(0, 6, 0), VECTOR3_UP);
	mainCamera.sensitivity = 20;

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

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

	objInscene[CIRCLE] = new CircusTent();
	objInscene[BOX] = new CarnivalHitBoxes();
	objInscene[BOX2] = new CarnivalHitBoxes();
	objInscene[BOX3] = new CarnivalHitBoxes();
	objInscene[BOX4] = new CarnivalHitBoxes();
	objInscene[BOX5] = new CarnivalHitBoxes();
	objInscene[BOX6] = new CarnivalHitBoxes();
	objInscene[BOX7] = new CarnivalHitBoxes();
	objInscene[BOX8] = new CarnivalHitBoxes();


	
	objInscene[CIRCLE]->m_transform.ScaleBy(200.0f, 200.f, 200.0f);


	objInscene[BOX]->m_transform.Translate(0.0f, 0.5f, 400.0f);
	objInscene[BOX]->m_transform.ScaleBy(350.0f, 100.f, 100.0f);

	objInscene[BOX2]->m_transform.Translate(512.5f, 0.5f, 400.0f);
	objInscene[BOX2]->m_transform.ScaleBy(275.0f, 100.f, 100.0f);

	objInscene[BOX3]->m_transform.Translate(-512.5f, 0.5f, 400.0f);
	objInscene[BOX3]->m_transform.ScaleBy(275.0f, 100.f, 100.0f);

	objInscene[BOX4]->m_transform.Translate(650.5f, 0.5f, 0.0f);
	objInscene[BOX4]->m_transform.ScaleBy(100.0f, 100.f, 300.0f);

	objInscene[BOX5]->m_transform.Translate(-650.5f, 0.5f, 0.0f);
	objInscene[BOX5]->m_transform.ScaleBy(100.0f, 100.f, 300.0f);

	objInscene[BOX6]->m_transform.Translate(-650.5f, 0.5f, 250.0f);
	objInscene[BOX6]->m_transform.ScaleBy(50.0f, 100.f, 100.0f);


	objInscene[BOX7]->m_transform.Translate(-650.5f, 0.5f, -300.0f);
	objInscene[BOX7]->m_transform.ScaleBy(100.0f, 100.f, 200.0f);

	objInscene[BOX8]->m_transform.Translate(650.5f, 0.5f, -300.0f);
	objInscene[BOX8]->m_transform.ScaleBy(100.0f, 100.f, 200.0f);

	GameObjectManager::GetInstance()->IniAll();

}

void carnivalroaming::Update()
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
	glm::vec3 finalForce = inputMovementDir * 100.0f * Time::deltaTime;
	mainCamera.m_transform.Translate(finalForce);
	mainCamera.UpdateCameraRotation();


	GameObject::MoveObj(objInscene[CIRCLE]->m_transform);
	GameObject::MoveObj(objInscene[BOX]->m_transform);
	GameObject::MoveObj(objInscene[BOX2]->m_transform);
	GameObject::MoveObj(objInscene[BOX3]->m_transform);
	GameObject::MoveObj(objInscene[BOX4]->m_transform);
	GameObject::MoveObj(objInscene[BOX5]->m_transform);
	GameObject::MoveObj(objInscene[BOX6]->m_transform);
	GameObject::MoveObj(objInscene[BOX7]->m_transform);

	
	GameObjectManager::GetInstance()->UpdateAll();
}

void carnivalroaming::Render()
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

	modelStack.PushMatrix();
	
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(1500.0f, 1000.0f, 0.0f);
	RenderMesh(meshList[GEO_PLANE]);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 0.5f, 0.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_CIRCUSTENT]);
	modelStack.PopMatrix();

	//shooting
	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 0.5f, 400.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(350.0f, 100.f, 0.0f);
	RenderMesh(meshList[GEO_HITBOX]);
	modelStack.PopMatrix();

	//balloon 
	modelStack.PushMatrix();
	modelStack.Translate(512.5f, 0.5f, 400.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(275.0f, 100.f, 0.0f);
	RenderMesh(meshList[GEO_HITBOX]);
	modelStack.PopMatrix();

	//can
	modelStack.PushMatrix();
	modelStack.Translate(-512.5f, 0.5f, 400.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(275.0f, 100.f, 0.0f);
	RenderMesh(meshList[GEO_HITBOX]);
	modelStack.PopMatrix();

	//ringtoss
	modelStack.PushMatrix();
	modelStack.Translate(650.5f, 0.5f, 0.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(100.0f, 300.f, 0.0f);
	RenderMesh(meshList[GEO_HITBOX]);
	modelStack.PopMatrix();

	//duck fishing
	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 0.5f, 0.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(100.0f, 300.f, 0.0f);
	RenderMesh(meshList[GEO_HITBOX]);
	modelStack.PopMatrix();

	//plinko
	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 0.5f, 250.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(50.0f, 100.f, 0.0f);
	RenderMesh(meshList[GEO_HITBOX]);
	modelStack.PopMatrix();

	//food 1 
	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 0.5f, -300.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(100.0f, 200.f, 0.0f);
	RenderMesh(meshList[GEO_HITBOX]);
	modelStack.PopMatrix();

	//food 2
	modelStack.PushMatrix();
	modelStack.Translate(650.5f, 0.5f, -300.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(100.0f, 200.f, 0.0f);
	RenderMesh(meshList[GEO_HITBOX]);
	modelStack.PopMatrix();

	GameObjectManager::GetInstance()->RenderAll(*this);
}

void carnivalroaming::Exit()
{
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void carnivalroaming::RenderSkybox(void)
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
