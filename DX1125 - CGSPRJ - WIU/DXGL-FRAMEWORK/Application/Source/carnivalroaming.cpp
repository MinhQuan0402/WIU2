#define _USE_MATH_DEFINES
#include "carnivalroaming.h"
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
#include "PlayerHitBox.h"
#include "ScenePlinko.h"
#include "SceneManager.h"
#include "GameManager.h"
#include "SceneCanKnockdown.h"
#include "CircusTent.h"
#include "MeshManager.h"
#include "SceneRingToss.h"


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



	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);

	meshList[GEO_HITBOX] = MeshBuilder::GenerateCube("HitBox", glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
	meshList[GEO_AXIS] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("Sphere", WHITE, .05f, 180, 180);
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_MODEL1] = MeshBuilder::GenerateOBJ("Doorman", "Models//doorman.obj");
	meshList[GEO_MODEL1]->textureID = LoadPNG("Images//doorman.png");
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", WHITE, 1.0f, 100, 100);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", YELLOW, 1.0f);

	meshList[GEO_TICKET] = MeshBuilder::GenerateQuad("Quad", YELLOW, 1.0f);
	meshList[GEO_TICKET]->textureID = LoadPNG("Images//ticket.png");

	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", WHITE, 1.0f);
	meshList[GEO_PLANE]->textureID = LoadPNG("Images//CR_grass.png");
	meshList[GEO_PLANE]->material = Material::Wood(WHITE); 

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

	glm::vec3 savedPositionn = GameManager::GetInstance()->GetPlayerPosition();
	if (savedPositionn.x &&
		savedPositionn.y &&
		savedPositionn.z)
	{
		mainCamera.Init(savedPositionn, glm::vec3(0.0f, GameManager::GetInstance()->GetCameraTarget().y, 0.0f), VECTOR3_UP);
	}
	else mainCamera.Init(glm::vec3(0,35,-400), glm::vec3(0,35,0), VECTOR3_UP);

	mainCamera.sensitivity = 20;

	glUniform1i(m_parameters[U_NUMLIGHTS], 1);

	lights[0].m_transform.m_position = devVec;
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].power = 2.0f;
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

	PhysicsMaterial mat;
	mat.m_mass = 0.0f;

	objInscene[CIRCLE] = new CircusTent();
	objInscene[BOX] = new CarnivalHitBoxes();
	objInscene[BOX2] = new CarnivalHitBoxes();
	objInscene[BOX3] = new CarnivalHitBoxes();
	objInscene[BOX4] = new CarnivalHitBoxes();
	objInscene[BOX5] = new CarnivalHitBoxes();
	objInscene[BOX6] = new CarnivalHitBoxes();
	objInscene[BOX7] = new CarnivalHitBoxes();
	objInscene[BOX8] = new CarnivalHitBoxes();
	objInscene[BOX9] = new CarnivalHitBoxes();
	objInscene[BOX10] = new CarnivalHitBoxes();
	objInscene[BOX11] = new CarnivalHitBoxes();
	objInscene[BOX12] = new CarnivalHitBoxes();
	objInscene[BOX13] = new CarnivalHitBoxes();


	objInscene[PLAYERBOX] = new PlayerHitBox();
	

	//objInscene[CIRCLE]->m_transform.ScaleBy(201.0f, 201.f, 201.0f);


	objInscene[BOX]->m_transform.Translate(0.0f, 0.5f, 400.0f);
	objInscene[BOX]->m_transform.ScaleBy(350.0f, 100.f, 100.0f);
	objInscene[BOX]->rb = addBoxCollider(objInscene[BOX], 350.0f, 100.f, 100.0f, mat);

	

	objInscene[BOX2]->m_transform.Translate(512.5f, 0.5f, 400.0f);
	objInscene[BOX2]->m_transform.ScaleBy(275.0f, 100.f, 100.0f);
	objInscene[BOX2]->rb = addBoxCollider(objInscene[BOX2], 275.0f, 100.f, 100.0f, mat);

	objInscene[BOX3]->m_transform.Translate(-512.5f, 0.5f, 400.0f);
	objInscene[BOX3]->m_transform.ScaleBy(275.0f, 100.f, 100.0f);
	objInscene[BOX3]->rb = addBoxCollider(objInscene[BOX3], 275.0f, 100.f, 100.0f, mat);


	objInscene[BOX4]->m_transform.Translate(650.5f, 0.5f, 0.0f);
	objInscene[BOX4]->m_transform.ScaleBy(100.0f, 100.f, 300.0f);
	objInscene[BOX4]->rb = addBoxCollider(objInscene[BOX4], 100.0f, 100.f, 300.0f, mat);

	objInscene[BOX5]->m_transform.Translate(-650.5f, 0.5f, 0.0f);
	objInscene[BOX5]->m_transform.ScaleBy(100.0f, 100.f, 300.0f);
	objInscene[BOX5]->rb = addBoxCollider(objInscene[BOX5], 100.0f, 100.f, 300.0f, mat);


	objInscene[BOX6]->m_transform.Translate(-650.5f, 0.5f, 250.0f);
	objInscene[BOX6]->m_transform.ScaleBy(50.0f, 100.f, 100.0f);
	objInscene[BOX6]->rb = addBoxCollider(objInscene[BOX6], 50.0f, 100.f, 100.0f, mat);


	objInscene[BOX7]->m_transform.Translate(-650.5f, 0.5f, -300.0f);
	objInscene[BOX7]->m_transform.ScaleBy(100.0f, 100.f, 200.0f);
	objInscene[BOX7]->rb = addBoxCollider(objInscene[BOX7], 100.0f, 100.f, 200.0f, mat);

	objInscene[BOX8]->m_transform.Translate(650.5f, 0.5f, -300.0f);
	objInscene[BOX8]->m_transform.ScaleBy(100.0f, 100.f, 200.0f);
	objInscene[BOX8]->rb = addBoxCollider(objInscene[BOX8], 100.0f, 100.f, 200.0f, mat);

	objInscene[BOX9]->m_transform.Translate(750.f, 0.5f, 0.0f);
	objInscene[BOX9]->m_transform.ScaleBy(1.0f, 20.f, 1000.0f);
	objInscene[BOX9]->rb = addBoxCollider(objInscene[BOX9], 1.0f, 20.f, 1000.0f, mat);

	objInscene[BOX10]->m_transform.Translate(-750.f, 0.5f, 0.0f);
	objInscene[BOX10]->m_transform.ScaleBy(1.0f, 20.f, 1000.0f);
	objInscene[BOX10]->rb = addBoxCollider(objInscene[BOX10], 1.0f, 20.f, 1000.0f, mat);

	objInscene[BOX11]->m_transform.Translate(0.f, 0.5f, 500.0f);
	objInscene[BOX11]->m_transform.ScaleBy(1500.0f, 20.f, 1.0f);
	objInscene[BOX11]->rb = addBoxCollider(objInscene[BOX11], 1500.0f, 20.f, 1.0f, mat);

	objInscene[BOX12]->m_transform.Translate(0.f, 0.5f, -500.0f);
	objInscene[BOX12]->m_transform.ScaleBy(1500.0f, 20.f, 1.0f);
	objInscene[BOX12]->rb = addBoxCollider(objInscene[BOX12], 1500.0f, 20.f, 1.0f, mat);

	objInscene[BOX13]->m_transform.Translate(-150.0f, 0.5f, -200.0f);
	objInscene[BOX13]->m_transform.ScaleBy(40.0f, 30.f, 40.0f);
	objInscene[BOX13]->rb = addBoxCollider(objInscene[BOX13], 40.0f, 30.f, 40.0f, mat);

	mat.m_mass = 1.0f;
	float playerColliderHeight = 35.5f;
	objInscene[PLAYERBOX]->m_transform.Translate(mainCamera.m_transform.m_position.x, 0.0f, mainCamera.m_transform.m_position.z);
	objInscene[PLAYERBOX]->rb = addBoxCollider(objInscene[PLAYERBOX], 3.0f, playerColliderHeight, 3.0f, mat, glm::vec3(0.0f, playerColliderHeight / 2.f, 0.0f));
	objInscene[PLAYERBOX]->rb->setSleepingThresholds(0.0f, 0.0f);
	objInscene[PLAYERBOX]->rb->setAngularFactor(btVector3(0, 0, 0));

	PhysicsMaterial groundMat;
	groundMat.m_friction = 0.5f;
	objInscene[GROUND] = new GameObject();
	objInscene[GROUND]->rb = addStaticPlane(objInscene[GROUND], VECTOR3_UP, groundMat);
	GameObjectManager::GetInstance()->addItem(objInscene[GROUND]);

	GameObjectManager::GetInstance()->IniAll();

	std::srand(std::time(0));
	
	float maxRadius = 30.0f;
	for (int i = 0; i < 10; i++)
	{
		float theta = (std::rand() / (float)RAND_MAX) * 2 * M_PI;

		float r = std::sqrt((std::rand() / (float)RAND_MAX)) * maxRadius;
		float x = r * std::cos(theta);
		float y = r * std::sin(theta);

		ducksX[i] = x;
		ducksY[i] = y;

	}
	energy = maxEnergy; 
	std::cout << GameManager::GetInstance()->GetPoints() << std::endl;

}

void carnivalroaming::Update()
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
	glm::vec3 finalForce = inputMovementDir * 100.0f;
	
	objInscene[PLAYERBOX]->rb->setLinearVelocity(btVector3(finalForce.x, 0.0f, finalForce.z));
	GameObjectManager::GetInstance()->UpdateAll();
}

void carnivalroaming::LateUpdate()
{
	glm::vec3 playerRBPosition = objInscene[PLAYERBOX]->GetRigidbodyPosition();
	mainCamera.m_transform.m_position = glm::vec3(playerRBPosition.x, playerRBPosition.y + 35, playerRBPosition.z);
	mainCamera.UpdateCameraRotation();

	// Developer Controls:
	{
		float speed = 20;
		if (KeyboardController::GetInstance()->IsKeyDown('Y')) {
			devVec.y += speed * Time::deltaTime;
		}
		if (KeyboardController::GetInstance()->IsKeyDown('U')) {
			devVec.y -= speed * Time::deltaTime;
		}
		if (KeyboardController::GetInstance()->IsKeyDown('T')) {
			devVec.z += speed * Time::deltaTime;
		}
		if (KeyboardController::GetInstance()->IsKeyDown('G')) {
			devVec.z -= speed * Time::deltaTime;
		}
		if (KeyboardController::GetInstance()->IsKeyDown('F')) {
			devVec.x += speed * Time::deltaTime;
		}
		if (KeyboardController::GetInstance()->IsKeyDown('H')) {
			devVec.x -= speed * Time::deltaTime;
		}

		lights[0].m_transform.m_position = devVec;

		GameObjectManager::GetInstance()->UpdateAll(); 
	}

	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX]->getObject())  and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		std::cout << "Colliding with duck shooting" << std::endl;
		
		
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX2]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		std::cout << "Colliding with ring toss" << std::endl;
		SceneManager::GetInstance()->ChangeState(new SceneRingToss);
		return;
	
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX3]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		std::cout << "Colliding with can game" << std::endl;
		SceneManager::GetInstance()->ChangeState(new SceneCanKnockdown); 
		return;
		
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX4]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		std::cout << "Colliding with balloon pop" << std::endl;
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX5]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		std::cout << "Colliding with duck fishing" << std::endl;
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX6]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		std::cout << "Colliding with plinko" << std::endl;
		SceneManager::GetInstance()->ChangeState(new ScenePlinko);
		return;
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX7]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		std::cout << "Colliding with food truck" << std::endl;
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX8]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		std::cout << "Colliding with food truck" << std::endl;
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX13]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E')) 
	{
		std::cout << "Colliding with Ticket booth" << std::endl;
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[CIRCLE]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E') and CanEnterCircus == true)
	{
		std::cout << "Colliding with CircusTent" << std::endl;
	}
	if (CanEnterCircus == true)
	{
		timer += Time::deltaTime; 
	}
}

void carnivalroaming::Render()
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
		RenderMesh(meshList[GEO_AXIS]);
		modelStack.PopMatrix();
	}

	RenderSkybox();

	//meshList[GEO_PLANE]->material = Material::Wood(RED);
	modelStack.PushMatrix();
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(1500.0f, 1000.0f, 0.0f);
	RenderMesh(meshList[GEO_PLANE], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 10.5f, 0.0f);
	modelStack.Scale(45, 45, 45);
	RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CIRCUSTENT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 0.5f, 0.0f);
	modelStack.Scale(45, 45, 45);
	RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CIRCUSNAME],true); 
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-150.0f, 0.5f, -200.0f);
	modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
	RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TICKETBOOTHFRAME], true);

	{
		modelStack.PushMatrix();
		//modelStack.Translate(0.0f, 0.5f, -400.0f);
		//modelStack.Scale(45, 45, 45);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TICKETBOOTHBOX], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		//modelStack.Translate(0.0f, 0.5f, -400.0f);
		//modelStack.Scale(45, 45, 45);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TICKETBOOTHLIGHTS], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		//modelStack.Translate(0.0f, 0.5f, -400.0f);
		//modelStack.Scale(45, 45, 45);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TICKETBOOTHWINDOWS], true);
		modelStack.PopMatrix();
	}

	modelStack.PopMatrix();

	//can game
	{
		modelStack.PushMatrix();
		modelStack.Translate(-512.5f, 0.5f, 405.0f);
		modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(20, 7, 6);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENT], true);
		modelStack.PushMatrix();
		//modelStack.Translate(-512.5f, 0.5f, 405.0f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Scale(20, 7, 6);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENTROOF], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-512.0f, 0.5f, 378.0f);
		modelStack.Scale(200, 70, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TABLECAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-400.5f, 5.5f, 405.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-634.5f, 5.5f, 405.0f);
		modelStack.Rotate(-90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-512.5f, 5.5f, 370.0f);
		//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-434.5f, 5.5f, 370.0f);
		//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-590.5f, 5.5f, 370.0f);
		//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-512.5f, 0.5f, 430.0f);
		modelStack.Scale(200, 70, 20);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TABLECAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-512.5f, 24.0f, 430.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);

		modelStack.PushMatrix();
		modelStack.Translate(-6.5f, .0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(6.5f, .0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(3.5f, 7.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-3.5f, 7.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 14.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PopMatrix();



		modelStack.PushMatrix();
		modelStack.Translate(-450.5f, 24.0f, 430.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);

		modelStack.PushMatrix();
		modelStack.Translate(-6.5f, .0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(6.5f, .0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(3.5f, 7.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-3.5f, 7.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 14.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PopMatrix();


		modelStack.PushMatrix();
		modelStack.Translate(-570.5f, 24.0f, 430.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);

		modelStack.PushMatrix();
		modelStack.Translate(-6.5f, .0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(6.5f, .0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(3.5f, 7.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-3.5f, 7.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 14.0f, 0.0f);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CAN], true);
		modelStack.PopMatrix();

		modelStack.PopMatrix();
	}


	//duck shooting
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 0.5f, 400.0f);
		modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(20, 7, 6);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENT], true);

		modelStack.PushMatrix();
		//modelStack.Translate(-512.5f, 0.5f, 405.0f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Scale(20, 7, 6);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENTROOF], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-122.f, 5.5f, 402.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(112.f, 5.5f, 402.0f);
		modelStack.Rotate(-90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();


		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 7.5f, 370.0f);
		//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(65.f, 7.5f, 370.0f);
		//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-72.f, 7.5f, 370.0f);
		//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();
}

	

	//food1
	{
		modelStack.PushMatrix();
		modelStack.Translate(-660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TOPCOTTONCANDY], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_MIDDLECOTTONCANDY], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_INNERMIDDLECOTTONCANDY], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_COTTONCANDYS], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_COTTONCANDYMACHINE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CARAMELAPPLE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CARAMELAPPLESTAND], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BOTTOMCOTTONCANDY], true);
		modelStack.PopMatrix();

		//food2
		modelStack.PushMatrix();
		modelStack.Translate(660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TOPCOTTONCANDY], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_MIDDLECOTTONCANDY], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_INNERMIDDLECOTTONCANDY], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_COTTONCANDYS], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_COTTONCANDYMACHINE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CARAMELAPPLE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CARAMELAPPLESTAND], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(660.5f, 30.f, -300.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(6, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BOTTOMCOTTONCANDY], true);
		modelStack.PopMatrix();

	}



	//plinko
	{
		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 0.5f, 250.0f);
		modelStack.Scale(10, 10, 10);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CR_PLINKO], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 0.5f, 250.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(7, 7, 3);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENT], true);
		modelStack.PushMatrix();
		//modelStack.Translate(-512.5f, 0.5f, 405.0f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Scale(20, 7, 6);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENTROOF], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 3.5f, 207.0f);
		//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(1, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 3.5f, 290.0f);
		//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
		//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(1, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();
}

	//duckfishing

	{
		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 0.5f, 0.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(20, 7, 7);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENT], true);
		modelStack.PushMatrix();
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENTROOF], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 3.5f, 115.0f);
		modelStack.Scale(2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 3.5f, -120.0f);
		modelStack.Scale(2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();


		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 10.5f, 0.0f);
		modelStack.Scale(2, 2, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_POOL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-650.5f, 10.5f, 0.0f);
		modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(40, 90, 40);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_POOLWATER], true);
		modelStack.PopMatrix();

		for (int i = 0; i < 10; i++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(-650.5f + ducksX[i], 13.5f, -45.0f + ducksY[i]);
			//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
			modelStack.Scale(2, 2, 2);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
			modelStack.PopMatrix();
		}

		for (int i = 0; i < 10; i++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(-650.5f + ducksX[i], 13.5f, 45.0f + ducksY[i]);
			modelStack.Rotate(60.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(2, 2, 2);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
			modelStack.PopMatrix();
		}

		for (int i = 0; i < 10; i++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(-650.5f + ducksX[i], 13.5f, 0.0f + ducksY[i]);
			modelStack.Rotate(60.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(2, 2, 2);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
			modelStack.PopMatrix();
		}
	}

	//duckshooting

	{
		modelStack.PushMatrix();
		modelStack.Translate(-0.0f, 0.5f, 381.0f);
		modelStack.Scale(200, 70, 10);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TABLECAN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-0.0f, 24.5f, 380.0f);
		modelStack.Rotate(60.0f, 0, 1, 0);
		modelStack.Rotate(90.0f, 0, 0, 1);
		modelStack.Scale(40, 40, 40);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_GUN], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 26.5f, 415.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.3, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 26.5f, 414.5f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(3, 3, 3);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TARGET], true);
		modelStack.PopMatrix();


		modelStack.PushMatrix();
		modelStack.Translate(30.0f, 26.5f, 415.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.3, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(30.0f, 26.5f, 414.5f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(3, 3, 3);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TARGET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(60.0f, 26.5f, 415.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.3, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(60.0f, 26.5f, 414.5f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(3, 3, 3);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TARGET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(90.0f, 26.5f, 415.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.3, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(90.0f, 26.5f, 414.5f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(3, 3, 3);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TARGET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-30.0f, 26.5f, 415.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.3, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-30.0f, 26.5f, 414.5f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(3, 3, 3);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TARGET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-60.0f, 26.5f, 415.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.3, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-60.0f, 26.5f, 414.5f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(3, 3, 3);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TARGET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-90.0f, 26.5f, 415.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.3, 5, 5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DUCKY], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-90.0f, 26.5f, 414.5f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(3, 3, 3);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_TARGET], true);
		modelStack.PopMatrix();


		modelStack.PushMatrix();
		modelStack.Translate(-8.0f, 0.5f, 420.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.5, 0.7, 0.7);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CONVEYER], true);
		modelStack.PopMatrix();
	}

	//balloon

	{
		modelStack.PushMatrix();
		modelStack.Translate(650.5f, 0.5f, 0.0f);
		modelStack.Rotate(-90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(20, 7, 7);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENT], true);
		modelStack.PushMatrix();
		//modelStack.Translate(-512.5f, 0.5f, 405.0f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Scale(20, 7, 6);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENTROOF], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(690.5f, 30.5f, 0.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.5, 0.4, 0.2);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BOARD], true);
		modelStack.PopMatrix();

		for (int i = 0; i < 3; i++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 0.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(RED);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 10.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(YELLOW);

			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 20.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(BLUE);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 30.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(GREEN);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 40.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(GREY);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 50.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(BRIGHTBLUE);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();


			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -10.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(YELLOW);

			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -20.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(BLUE);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -30.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(GREEN);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -40.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(GREY);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -50.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(BRIGHTBLUE);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

		}

		for (int i = 0; i > -3; i--)
		{
			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 0.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(RED);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 10.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(YELLOW);

			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 20.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(BLUE);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 30.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(GREEN);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 40.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(GREY);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), 50.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(BRIGHTBLUE);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();


			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -10.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(YELLOW);

			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -20.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(BLUE);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -30.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(GREEN);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -40.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(GREY);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(685.5f, 30.5f + (10 * i), -50.0f);
			modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
			modelStack.Scale(20, 20, 20);
			MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON]->material = Material::Sillicon(BRIGHTBLUE);
			RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BALLOON], true);
			modelStack.PopMatrix();

		}

		modelStack.PushMatrix();
		modelStack.Translate(650.5f, 3.5f, 120.0f);
		modelStack.Scale(2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(650.5f, 3.5f, -115.0f);
		modelStack.Scale(2.2, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(610.5f, 3.5f, 0.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(610.5f, 3.5f, 70.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(610.5f, 3.5f, -70.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();
	}

	//ring toss

	{
		modelStack.PushMatrix();
		modelStack.Translate(512.5f, 0.5f, 400.0f);
		modelStack.Rotate(-90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(7, 7, 20);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENT], true);
		modelStack.PushMatrix();
		//modelStack.Translate(-512.5f, 0.5f, 405.0f);
		//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Scale(20, 7, 6);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CANTENTROOF], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(602.5f, 0.5f, 400.0f);
		modelStack.Rotate(-90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BOTTLES], true);

		modelStack.PushMatrix();
		//modelStack.Translate(610.5f, 3.5f, -70.0f);
		//modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RACK], true);
		modelStack.PopMatrix();

		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(602.5f, 0.5f, 375.0f);
		modelStack.Rotate(-90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BOTTLES], true);
		modelStack.PushMatrix();
		//modelStack.Translate(610.5f, 3.5f, -70.0f);
		//modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RACK], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(602.5f, 0.5f, 425.0f);
		modelStack.Rotate(-90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BOTTLES], true);
		modelStack.PushMatrix();
		//modelStack.Translate(610.5f, 3.5f, -70.0f);
		//modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Scale(2.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RACK], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(512.5f, 0.5f, 400.0f);
		modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(2., 7, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_STRIPEWALL], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(402.5f, 0.5f, 402.0f);
		modelStack.Rotate(-90.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(3.5, 1, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_COUNTER], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(402.5f, 20.5f, 430.0f);
		//modelStack.Rotate(-130.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		//modelStack.Scale(2., 7, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RING], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(402.5f, 20.5f, 400.0f);
		//modelStack.Rotate(-130.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		//modelStack.Scale(2., 7, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RING], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(402.5f, 20.5f, 370.0f);
		//modelStack.Rotate(-130.0f, 0.0f, 1.0f, 0.0f);
		//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		//modelStack.Scale(2., 7, 1);
		RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_RING], true);
		modelStack.PopMatrix();
}

	//shooting
	//modelStack.PushMatrix();
	//modelStack.Translate(0.0f, 0.5f, 400.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(350.0f, 100.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	////ring toss 
	//modelStack.PushMatrix();
	//modelStack.Translate(512.5f, 0.5f, 400.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(275.0f, 100.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	////can
	//modelStack.PushMatrix();
	//modelStack.Translate(-512.5f, 0.5f, 400.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(275.0f, 100.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	////balloon
	//modelStack.PushMatrix();
	//modelStack.Translate(650.5f, 0.5f, 0.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(100.0f, 300.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	////duck fishing
	//modelStack.PushMatrix();
	//modelStack.Translate(-650.5f, 0.5f, 0.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(100.0f, 300.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	////plinko
	//modelStack.PushMatrix();
	//modelStack.Translate(-650.5f, 0.5f, 250.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(50.0f, 100.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	////food 1 
	//modelStack.PushMatrix();
	//modelStack.Translate(-650.5f, 0.5f, -300.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(100.0f, 200.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	////food 2
	//modelStack.PushMatrix();
	//modelStack.Translate(650.5f, 0.5f, -300.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(100.0f, 200.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(devVec.x, devVec.y, devVec.z);
	modelStack.Scale(1, 1, 1);
	RenderMesh(meshList[GEO_CUBE]);
	modelStack.PopMatrix();

	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX13]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		//std::cout << "Colliding with Ticket booth" << std::endl;
		RenderTextOnScreen(meshList[GEO_TEXT], "2984 points for a ticket! (N to exchange)", YELLOW, 40, 100, 300);
		RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(GameManager::GetInstance()->GetPoints()) + " / 2984", YELLOW, 40, 550, 150);
	}
	if (CheckCollisionWith(objInscene[PLAYERBOX]->getObject(), objInscene[BOX13]->getObject()) and KeyboardController::GetInstance()->IsKeyDown('N') and (GameManager::GetInstance()->GetPoints() >= 2984))
	{	
		GameManager::GetInstance()->SetPoints(GameManager::GetInstance()->GetPoints() - 2984);
		CanEnterCircus = true;
	}
	if (timer <= 5.0f and CanEnterCircus == true)
		{
			RenderMeshOnScreen(meshList[GEO_TICKET], 600, 350, 650, 400, glm::vec3(0, 1, 0));
			std::cout << timer << std::endl;
		}
	RenderTextOnScreen(meshList[GEO_TEXT], "Points:" + std::to_string(GameManager::GetInstance()->GetPoints()), YELLOW, 40, 900, 600);

	GameObjectManager::GetInstance()->RenderAll(*this);

	// Render Energy UI:
	{
		RenderMeshOnScreen(MeshManager::GetInstance()->meshList[MeshManager::GEO_ENERGYFRAME], 175, 53, 288, 53);

		float maxWidth = 275;
		float barWidth = energy / maxEnergy * maxWidth;
		float posX = 175 - (maxWidth / 2) + (barWidth / 2);

		RenderMeshOnScreen(MeshManager::GetInstance()->meshList[MeshManager::GEO_ENERGYBAR], posX, 53, barWidth, 39); 
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

void carnivalroaming::Exit()
{
	GameManager::GetInstance()->SetPlayerPosition(mainCamera.m_transform.m_position.x, mainCamera.m_transform.m_position.y, mainCamera.m_transform.m_position.z);
	GameManager::GetInstance()->SetCameraTarget(mainCamera.target.x, mainCamera.target.y, mainCamera.target.z);
	GameManager::GetInstance()->SetPoints(3000);
	Scene::Exit(); 
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i) { if (meshList[i]) delete meshList[i]; }
	meshList.clear();
	GameObjectManager::GetInstance()->clearGOs();
	ColliderManager::GetInstance()->ClearAll();

	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void carnivalroaming::RenderSkybox(void)
{
	float size = 200.0f;
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
