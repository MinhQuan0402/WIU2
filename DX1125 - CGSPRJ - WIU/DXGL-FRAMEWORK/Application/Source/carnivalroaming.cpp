#define _USE_MATH_DEFINES
#include "carnivalroaming.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "cmath"
#include <cstdlib>
#include <ctime>
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
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", WHITE, 1.0f);

	meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateCircle("Circle", WHITE, 200);
	meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateOBJMTL("CircusTent", "Models//circusTentTest.obj", "Models//circusTentTest.mtl");
	meshList[GEO_CIRCUSTENT]->textureID = LoadPNG("Images//CircusTentTest.png");

	//meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateCircle("Circle", WHITE, 200);
	meshList[GEO_CIRCUSNAME] = MeshBuilder::GenerateOBJMTL("CircusName", "Models//circusName.obj", "Models//circusName.mtl");
	meshList[GEO_CIRCUSNAME]->textureID = LoadPNG("Images//CircusNameTestpng.png");
	meshList[GEO_CIRCUSNAME]->material = Material::Wood(WHITE);


	meshList[GEO_CANTENT] = MeshBuilder::GenerateOBJMTL("CanTent", "Models//CK_BoothGuards.obj", "Models//CK_BoothGuards.mtl");
	meshList[GEO_CANTENT]->material = Material::Wood(WHITE);
	meshList[GEO_CANTENTROOF] = MeshBuilder::GenerateOBJMTL("CanTent", "Models//CK_BoothRoof.obj", "Models//CK_BoothRoof.mtl");
	meshList[GEO_CANTENTROOF]->textureID = LoadPNG("Images//CK_BoothRoof.png");
	meshList[GEO_CANTENTROOF]->material = Material::Wood(WHITE);


	meshList[GEO_TABLECAN] = MeshBuilder::GenerateOBJ("Table", "Models//simpletable.obj");
	meshList[GEO_TABLECAN]->textureID = LoadPNG("Images//tableCan.png");
	meshList[GEO_TABLECAN]->material = Material::Wood(WHITE);


	meshList[GEO_CAN] = MeshBuilder::GenerateOBJMTL("Can", "Models//Can.obj", "Models//Can.mtl");
	meshList[GEO_CAN]->textureID = LoadPNG("Images//Can.png");
	meshList[GEO_CAN]->material = Material::Wood(WHITE);


	meshList[GEO_STRIPEWALL] = MeshBuilder::GenerateOBJMTL("Stripewall", "Models//stripedwall2.obj", "Models//stripedwall2.mtl");
	meshList[GEO_STRIPEWALL]->material = Material::Wood(WHITE);

	meshList[GEO_TOPCOTTONCANDY] = MeshBuilder::GenerateOBJ("CottonCandy", "Models//top_cottoncandy.obj");
	meshList[GEO_TOPCOTTONCANDY]->textureID = LoadPNG("Images//top_cotton_candy.png");
	meshList[GEO_TOPCOTTONCANDY]->material = Material::Wood(WHITE);

	meshList[GEO_MIDDLECOTTONCANDY] = MeshBuilder::GenerateOBJ("Middle CottonCandy", "Models//middle_cottoncandy.obj");
	meshList[GEO_MIDDLECOTTONCANDY]->textureID = LoadPNG("Images//middle_cottoncandy.png");
	meshList[GEO_MIDDLECOTTONCANDY]->material = Material::Wood(WHITE);

	meshList[GEO_INNERMIDDLECOTTONCANDY] = MeshBuilder::GenerateOBJ("Inner Middle CottonCandy", "Models//innermiddle_cottoncandy.obj");
	meshList[GEO_INNERMIDDLECOTTONCANDY]->material = Material::Wood(WHITE);

	meshList[GEO_COTTONCANDYS] = MeshBuilder::GenerateOBJ("CottonCandy", "Models//cottoncandys.obj");
	meshList[GEO_COTTONCANDYS]->textureID = LoadPNG("Images//cottoncandys.png");
	meshList[GEO_COTTONCANDYS]->material = Material::Wood(WHITE);

	meshList[GEO_COTTONCANDYMACHINE] = MeshBuilder::GenerateOBJ("CottonCandymachines", "Models//cottoncandymachine.obj");
	meshList[GEO_COTTONCANDYMACHINE]->textureID = LoadPNG("Images//cottoncandymachine.png");
	meshList[GEO_COTTONCANDYMACHINE]->material = Material::Wood(WHITE);

	meshList[GEO_CARAMELAPPLE] = MeshBuilder::GenerateOBJ("CaramelApple", "Models//caramelapple_cottoncandy.obj");
	meshList[GEO_CARAMELAPPLE]->textureID = LoadPNG("Images//caramelapple_cottoncandy.png");
	meshList[GEO_CARAMELAPPLE]->material = Material::Wood(WHITE);

	meshList[GEO_CARAMELAPPLESTAND] = MeshBuilder::GenerateOBJ("CaramelAppleStand", "Models//caramelapplestand_cottoncandy.obj");
	meshList[GEO_CARAMELAPPLESTAND]->textureID = LoadPNG("Images//caramelapplestands_cottoncandy.png");
	meshList[GEO_CARAMELAPPLESTAND]->material = Material::Wood(WHITE);

	meshList[GEO_BOTTOMCOTTONCANDY] = MeshBuilder::GenerateOBJ("BottomCottonCandy", "Models//bottom_cottoncandy.obj");
	meshList[GEO_BOTTOMCOTTONCANDY]->textureID = LoadPNG("Images//bottom_cottoncandy.png");
	meshList[GEO_BOTTOMCOTTONCANDY]->material = Material::Wood(WHITE);


	meshList[GEO_PLINKO] = MeshBuilder::GenerateOBJMTL("Plinko", "Models//plinko.obj", "Models//plinko.mtl");
	meshList[GEO_PLINKO]->textureID = LoadPNG("Images//plinko.png");
	meshList[GEO_PLINKO]->material = Material::Wood(WHITE);


	meshList[GEO_POOL] = MeshBuilder::GenerateOBJ("Table", "Models//InflatablePool.obj");
	meshList[GEO_POOL]->textureID = LoadPNG("Images//pool.png");
	meshList[GEO_POOL]->material = Material::Wood(WHITE);

	meshList[GEO_POOLWATER] = MeshBuilder::GenerateCircle("Circle", WHITE, 1);
	meshList[GEO_POOLWATER]->textureID = LoadPNG("Images//water.png");
	meshList[GEO_POOLWATER]->material = Material::Wood(WHITE);

	meshList[GEO_DUCKY] = MeshBuilder::GenerateOBJMTL("duck", "Models//ducky.obj", "Models//ducky.mtl");
	meshList[GEO_DUCKY]->material = Material::Wood(WHITE);

	meshList[GEO_SHOOTINGDUCK] = MeshBuilder::GenerateOBJMTL("duckpewpew", "Models//duckshooting.obj", "Models//duckshooting.mtl");
	meshList[GEO_SHOOTINGDUCK]->material = Material::Wood(WHITE);

	meshList[GEO_TARGET] = MeshBuilder::GenerateOBJMTL("target", "Models//target.obj", "Models//target.mtl");
	meshList[GEO_TARGET]->material = Material::Wood(WHITE);

	meshList[GEO_CONVEYER] = MeshBuilder::GenerateOBJMTL("conveyer", "Models//conveyer.obj", "Models//conveyer.mtl");
	meshList[GEO_CONVEYER]->material = Material::Wood(WHITE);


	mainCamera.Init(glm::vec3(0, 6, -400), glm::vec3(0, 6, 0), VECTOR3_UP);
	mainCamera.sensitivity = 20;

	glUniform1i(m_parameters[U_NUMLIGHTS], 1);

	lights[0].m_transform.m_position = devVec;
	lights[0].color = glm::vec3(1, 1, 1);
	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].power = 2.0f;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = 90.f;
	lights[0].cosInner = 45.f;
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


	
	objInscene[CIRCLE]->m_transform.ScaleBy(200.0f, 200.f, 200.0f);


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

		//std::cout << "RandomPoint in circle: (" << x << ", " << y << ")\n";
		//std::cout << ducksX[i] << std::endl; 
		//std::cout << ducksY[i] << std::endl;
	}
	

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

void carnivalroaming::LateUpdate()
{
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
		std::cout << devVec.x << ", " << devVec.y << ", " << devVec.z << std::endl;
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
	}

	meshList[GEO_PLANE]->material = Material::Wood(RED);
	modelStack.PushMatrix();
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(1500.0f, 1000.0f, 0.0f);
	RenderMesh(meshList[GEO_PLANE], true);
	modelStack.PopMatrix();

	

	meshList[GEO_CIRCUSTENT]->material = Material::Wood(WHITE);
	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 0.5f, 0.0f);
	modelStack.Scale(45, 45, 45);
	RenderMesh(meshList[GEO_CIRCUSTENT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 0.5f, 0.0f);
	modelStack.Scale(45, 45, 45);
	RenderMesh(meshList[GEO_CIRCUSNAME],true);
	modelStack.PopMatrix();

	//can game
	modelStack.PushMatrix();
	modelStack.Translate(-512.5f, 0.5f, 405.0f);
	modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_CANTENT],true);

	modelStack.PushMatrix();
	//modelStack.Translate(-512.5f, 0.5f, 405.0f);
	//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
	//modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_CANTENTROOF], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(-370.5f, 5.5f, 405.0f);
	modelStack.Rotate(90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-370.5f, 15.5f, 405.0f);
	modelStack.Rotate(90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-654.5f, 11.5f, 405.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-654.5f, 11.5f, 405.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-512.5f, 11.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-470.5f, 11.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-420.5f, 11.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(-570.5f, 11.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-610.5f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(-512.5f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-470.5f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-420.5f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(-570.5f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-610.5f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(-512.5f, 0.5f, 430.0f);
	modelStack.Scale(200, 70, 20);
	RenderMesh(meshList[GEO_TABLECAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-512.5f, 24.0f, 430.0f);
	RenderMesh(meshList[GEO_CAN], true);

	modelStack.PushMatrix();
	modelStack.Translate(-6.5f, .0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6.5f, .0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(3.5f, 7.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-3.5f, 7.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 14.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();



	modelStack.PushMatrix();
	modelStack.Translate(-450.5f, 24.0f, 430.0f);
	RenderMesh(meshList[GEO_CAN], true);

	modelStack.PushMatrix();
	modelStack.Translate(-6.5f, .0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6.5f, .0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(3.5f, 7.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-3.5f, 7.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 14.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(-570.5f, 24.0f, 430.0f);
	RenderMesh(meshList[GEO_CAN], true);

	modelStack.PushMatrix();
	modelStack.Translate(-6.5f, .0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6.5f, .0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(3.5f, 7.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-3.5f, 7.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 14.0f, 0.0f);
	RenderMesh(meshList[GEO_CAN], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();


	//duck shooting
	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 0.5f, 400.0f);
	modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_CANTENT], true);

	modelStack.PushMatrix();
	//modelStack.Translate(-512.5f, 0.5f, 405.0f);
	//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
	//modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_CANTENTROOF], true);
	modelStack.PopMatrix(); 
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-142.f, 5.5f, 405.0f);
	modelStack.Rotate(90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-142.f, 15.5f, 405.0f);
	modelStack.Rotate(90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(142.f, 11.5f, 405.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(142.f, 11.5f, 405.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 11.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-42.f, 11.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-92.f, 11.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(58.f, 11.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(98.f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(0.f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-42.f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-92.f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(58.f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(98.f, 5.5f, 370.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();




	//food1
	modelStack.PushMatrix();
	modelStack.Translate(-660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_TOPCOTTONCANDY], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_MIDDLECOTTONCANDY], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_INNERMIDDLECOTTONCANDY], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_COTTONCANDYS], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_COTTONCANDYMACHINE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_CARAMELAPPLE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_CARAMELAPPLESTAND],true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_BOTTOMCOTTONCANDY], true);
	modelStack.PopMatrix();

	//food2
	modelStack.PushMatrix();
	modelStack.Translate(660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_TOPCOTTONCANDY], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_MIDDLECOTTONCANDY], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_INNERMIDDLECOTTONCANDY], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_COTTONCANDYS], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_COTTONCANDYMACHINE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_CARAMELAPPLE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_CARAMELAPPLESTAND], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(660.5f, 30.f, -300.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(6, 5, 5);
	RenderMesh(meshList[GEO_BOTTOMCOTTONCANDY], true);
	modelStack.PopMatrix();




	//plinko
	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 0.5f, 250.0f);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[GEO_PLINKO], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 0.5f, 250.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(7, 7, 3);
	RenderMesh(meshList[GEO_CANTENT], true);
	modelStack.PushMatrix();
	//modelStack.Translate(-512.5f, 0.5f, 405.0f);
	//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
	//modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_CANTENTROOF], true);
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 3.5f, 210.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(5, 7, 3);
	RenderMesh(meshList[GEO_STRIPEWALL],true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 3.5f, 300.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(5, 7, 3);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	//duckfishing

	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 0.5f, 0.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(20, 7, 7);
	RenderMesh(meshList[GEO_CANTENT], true);
	modelStack.PushMatrix();
	//modelStack.Translate(-512.5f, 0.5f, 405.0f);
	//modelStack.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
	//modelStack.Scale(20, 7, 6);
	RenderMesh(meshList[GEO_CANTENTROOF], true);
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 3.5f, 140.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(7, 7, 7);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 3.5f,-140.0f);
	modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(7, 7, 7);
	RenderMesh(meshList[GEO_STRIPEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 10.5f, 0.0f);
	//modelStack.Rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	//modelStack.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(2, 2, 5);
	RenderMesh(meshList[GEO_POOL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-650.5f, 10.5f, 0.0f);
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelStack.Scale(40, 90, 40);
	RenderMesh(meshList[GEO_POOLWATER], true);
	modelStack.PopMatrix();

	for (int i = 0; i < 10; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-650.5f + ducksX[i], 13.5f, -45.0f + ducksY[i]);
		//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_DUCKY], true);
		modelStack.PopMatrix();
	}
	
	for (int i = 0; i < 10; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-650.5f + ducksX[i], 13.5f, 45.0f + ducksY[i]);
		modelStack.Rotate(60.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_DUCKY], true);
		modelStack.PopMatrix();
	}
	
	for (int i = 0; i < 10; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-650.5f + ducksX[i], 13.5f, 0.0f + ducksY[i]);
		modelStack.Rotate(60.0f, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_DUCKY], true);
		modelStack.PopMatrix();
	}

	//duckshooting

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 27.5f, 415.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(0.1, 0.1, 0.1);
	RenderMesh(meshList[GEO_SHOOTINGDUCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, 25.5f, 414.5f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(0.2, 0.2, 0.2);
	RenderMesh(meshList[GEO_TARGET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-13.0f, 0.5f, 420.0f);
	modelStack.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelStack.Scale(25, 20, 27);
	RenderMesh(meshList[GEO_CONVEYER], true);
	modelStack.PopMatrix();

	//shooting
	//modelStack.PushMatrix();
	//modelStack.Translate(0.0f, 0.5f, 400.0f);
	//modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	//modelStack.Scale(350.0f, 100.f, 0.0f);
	//RenderMesh(meshList[GEO_HITBOX]);
	//modelStack.PopMatrix();

	////balloon 
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

	////ringtoss
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
