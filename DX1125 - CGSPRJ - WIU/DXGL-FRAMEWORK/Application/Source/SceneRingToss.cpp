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

SceneRingToss::SceneRingToss() : numLight{ 2 }
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
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Quad", RED, 1000.0f);
	meshList[GEO_BOARD] = MeshBuilder::GenerateOBJ("board", "Models//ring_toss_rack.obj");
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

	mainCamera.Init(glm::vec3(0, 10, 30), glm::vec3(0, 10, 0), VECTOR3_UP);
	
	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	lights[0].m_transform.m_position = glm::vec3{};
	lights[0].m_transform.m_position = glm::vec3(0, 20, 0);
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

	enableLight = false;

	PhysicsMaterial groundMat;
	groundMat.m_bounciness = 0.5f;
	groundMat.m_friction = 0.5f;
	objInScene[GROUND] = new GameObject();
	objInScene[GROUND]->rb = addStaticPlane(objInScene[GROUND], VECTOR3_UP, groundMat);
	GameObjectManager::GetInstance()->addItem(objInScene[GROUND]);

	objInScene[TABLE] = new TossTable();
	objInScene[TABLE]->m_transform.Translate(0.0f, .0f, 4.f);
	objInScene[TABLE]->m_transform.Rotate(0.0f, 90.0f);
	objInScene[TABLE]->m_transform.ScaleBy(2.0f, 2.0f, 2.0f);

	objInScene[BOARD] = new TossBoard();
	objInScene[BOARD]->m_transform.Translate(0.0f, 5.5f, 0.0f);

	float colliderHeight = 7.3f;
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	objInScene[COUNTER] = new GameObject;
	objInScene[COUNTER]->m_transform.Translate(0.0f, .0f, 20.0f);
	objInScene[COUNTER]->m_transform.Rotate(0.0f, 180.0f);
	objInScene[COUNTER]->m_transform.ScaleBy(0.4f, 0.4f, 0.4f);
	objInScene[COUNTER]->rb = addBoxCollider(objInScene[COUNTER], 10.0f, colliderHeight, 6.25f, mat, glm::vec3(0.0f, colliderHeight / 2.f, 0.f));
	GameObjectManager::addItem(objInScene[COUNTER]);

	objInScene[RING] = new Ring(RED);
	GameObjectManager::IniAll();
}

void SceneRingToss::Update()
{
	HandleKeyPress();
	mainCamera.Update();
	glm::vec3 inputMovementDir{};
	glm::vec3 forward = glm::normalize(glm::cross(mainCamera.up, mainCamera.right));
	if (KeyboardController::GetInstance()->IsKeyDown('W'))
		inputMovementDir = forward;
	if (KeyboardController::GetInstance()->IsKeyDown('S'))
		inputMovementDir = -forward;
	if (KeyboardController::GetInstance()->IsKeyDown('D'))
		inputMovementDir = mainCamera.right;
	if (KeyboardController::GetInstance()->IsKeyDown('A'))
		inputMovementDir = -mainCamera.right;
	glm::vec3 finalForce = inputMovementDir * 10.0f * Time::deltaTime;
	mainCamera.m_transform.Translate(finalForce);
	mainCamera.UpdateCameraRotation();

	GameObjectManager::UpdateAll();
}

void SceneRingToss::LateUpdate()
{
	if (!isShoot)
	{
		glm::vec3 forward = glm::normalize(glm::cross(mainCamera.up, mainCamera.right));
		glm::vec3 throwablePos = mainCamera.m_transform.m_position
			+ mainCamera.right * 1.0f  // Move right
			- mainCamera.up * 0.5f    // Move down
			+ mainCamera.view * 1.0f; // Optional depth
		objInScene[RING]->SetRigidbodyPosition(throwablePos);
		objInScene[RING]->rb->clearGravity();
	}
	else
	{
		timer += Time::deltaTime;

		if (timer >= 4.0f)
		{
			isShoot = false;
			timer = 0.0f;
		}
	}

	if (!isShoot && KeyboardController::GetInstance()->IsKeyPressed(VK_SPACE))
	{
		float power = 20.0f;
		btVector3 finalVel = btVector3(mainCamera.view.x, mainCamera.view.y, mainCamera.view.z) * power;
		objInScene[RING]->rb->setLinearVelocity(btVector3(finalVel));
		objInScene[RING]->rb->setAngularVelocity(btVector3(0.f, power, 0.0f));
		objInScene[RING]->rb->activate();
		isShoot = true;
	}
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

	meshList[GEO_PLANE]->material = Material::Concrete(WHITE);
	modelStack.PushMatrix();
	modelStack.Rotate(90.0f, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_PLANE], enableLight);
	modelStack.PopMatrix();

	meshList[GEO_COUNTER]->material = Material::Wood(WHITE);
	RenderMesh(meshList[GEO_COUNTER], enableLight, objInScene[COUNTER]->m_transform);
	GameObjectManager::RenderAll(*this);

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
			for (unsigned i = 0; i < compoundShape->getNumChildShapes(); ++i)
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
