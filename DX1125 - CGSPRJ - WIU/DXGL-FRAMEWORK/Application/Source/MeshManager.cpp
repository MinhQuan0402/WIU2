#define _USE_MATH_DEFINES
#include "MeshManager.h"
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
#include <MyMath.h>
#include "Utility.h"
#include "ColliderManager.h"
#include "RigidBody.h"
#include "SceneManager.h"
#include "GameManager.h"

MeshManager* MeshManager::m_instance = nullptr;
MeshManager::MeshManager()
{
	meshList.resize(NUM_MESHES);
}

MeshManager::~MeshManager()
{
}

MeshManager* MeshManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new MeshManager();
	}
	return m_instance;
}

void MeshManager::DestroyInstance()
{
	if (m_instance)
		delete m_instance;
}


void MeshManager::Init()
{
	//carnival roaming 
	meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateCircle("Circle", WHITE, 200);
	meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateOBJ("CircusTent", "Models//CR_circusTentTest.obj");
	meshList[GEO_CIRCUSTENT]->textureID = LoadPNG("Images//CR_CircusTentTest.png");
	meshList[GEO_CIRCUSTENT]->material = Material::Wood(WHITE);
	meshList[GEO_CIRCUSTENT]->material.kDiffuse = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_CIRCUSTENT]->material.kAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_CIRCUSTENT]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);

	//meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateCircle("Circle", WHITE, 200);
	meshList[GEO_CIRCUSNAME] = MeshBuilder::GenerateOBJMTL("CircusName", "Models//CR_circusName.obj", "Models//CR_circusName.mtl");
	meshList[GEO_CIRCUSNAME]->textureID = LoadPNG("Images//CR_CircusNameTestpng.png");
	meshList[GEO_CIRCUSNAME]->material = Material::Wood(WHITE);


	meshList[GEO_CANTENT] = MeshBuilder::GenerateOBJMTL("CanTent", "Models//CR_CK_BoothGuards.obj", "Models//CR_CK_BoothGuards.mtl");
	meshList[GEO_CANTENT]->material = Material::Wood(WHITE);
	meshList[GEO_CANTENT]->material.kAmbient = glm::vec3(0.4, 0.4, 0.4);

	meshList[GEO_CANTENTROOF] = MeshBuilder::GenerateOBJMTL("CanTent", "Models//CR_CK_BoothRoof.obj", "Models//CR_CK_BoothRoof.mtl");
	meshList[GEO_CANTENTROOF]->textureID = LoadPNG("Images//CR_CK_BoothRoof.png");
	meshList[GEO_CANTENTROOF]->material = Material::Wood(WHITE);

	meshList[GEO_TABLECAN] = MeshBuilder::GenerateOBJ("Table", "Models//CR_simpletable.obj");
	meshList[GEO_TABLECAN]->textureID = LoadPNG("Images//CR_tableCan.png");
	meshList[GEO_TABLECAN]->material = Material::Wood(WHITE);

	meshList[GEO_CAN] = MeshBuilder::GenerateOBJMTL("Can", "Models//CR_Can.obj", "Models//CR_Can.mtl");
	meshList[GEO_CAN]->textureID = LoadPNG("Images//CR_Can.png");
	meshList[GEO_CAN]->material = Material::Wood(WHITE);

	meshList[GEO_STRIPEWALL] = MeshBuilder::GenerateOBJ("Stripewall", "Models//CR_redwhitewalls.obj");
	meshList[GEO_STRIPEWALL]->textureID = LoadPNG("Images//CR_redwhitestripes.png");
	meshList[GEO_STRIPEWALL]->material = Material::Wood(WHITE);

	meshList[GEO_TOPCOTTONCANDY] = MeshBuilder::GenerateOBJ("CottonCandy", "Models//CR_top_cottoncandy.obj");
	meshList[GEO_TOPCOTTONCANDY]->textureID = LoadPNG("Images//CR_top_cotton_candy.png");
	meshList[GEO_TOPCOTTONCANDY]->material = Material::Wood(WHITE);

	meshList[GEO_MIDDLECOTTONCANDY] = MeshBuilder::GenerateOBJ("Middle CottonCandy", "Models//CR_middle_cottoncandy.obj");
	meshList[GEO_MIDDLECOTTONCANDY]->textureID = LoadPNG("Images//CR_middle_cottoncandy.png");
	meshList[GEO_MIDDLECOTTONCANDY]->material = Material::Wood(WHITE);

	meshList[GEO_INNERMIDDLECOTTONCANDY] = MeshBuilder::GenerateOBJ("Inner Middle CottonCandy", "Models//CR_innermiddle_cottoncandy.obj");
	meshList[GEO_INNERMIDDLECOTTONCANDY]->material = Material::Wood(WHITE);

	meshList[GEO_COTTONCANDYS] = MeshBuilder::GenerateOBJ("CottonCandy", "Models//CR_cottoncandys.obj");
	meshList[GEO_COTTONCANDYS]->textureID = LoadPNG("Images//CR_cottoncandys.png");
	meshList[GEO_COTTONCANDYS]->material = Material::Wood(WHITE);

	meshList[GEO_COTTONCANDYMACHINE] = MeshBuilder::GenerateOBJ("CottonCandymachines", "Models//CR_cottoncandymachine.obj");
	meshList[GEO_COTTONCANDYMACHINE]->textureID = LoadPNG("Images//CR_cottoncandymachine.png");
	meshList[GEO_COTTONCANDYMACHINE]->material = Material::Wood(WHITE);

	meshList[GEO_CARAMELAPPLE] = MeshBuilder::GenerateOBJ("CaramelApple", "Models//CR_caramelapple_cottoncandy.obj");
	meshList[GEO_CARAMELAPPLE]->textureID = LoadPNG("Images//CR_caramelapple_cottoncandy.png");
	meshList[GEO_CARAMELAPPLE]->material = Material::Wood(WHITE);

	meshList[GEO_CARAMELAPPLESTAND] = MeshBuilder::GenerateOBJ("CaramelAppleStand", "Models//CR_caramelapplestand_cottoncandy.obj");
	meshList[GEO_CARAMELAPPLESTAND]->textureID = LoadPNG("Images//CR_caramelapplestands_cottoncandy.png");
	meshList[GEO_CARAMELAPPLESTAND]->material = Material::Wood(WHITE);

	meshList[GEO_BOTTOMCOTTONCANDY] = MeshBuilder::GenerateOBJ("BottomCottonCandy", "Models//CR_bottom_cottoncandy.obj");
	meshList[GEO_BOTTOMCOTTONCANDY]->textureID = LoadPNG("Images//CR_bottom_cottoncandy.png");
	meshList[GEO_BOTTOMCOTTONCANDY]->material = Material::Wood(WHITE);

	meshList[GEO_CR_PLINKO] = MeshBuilder::GenerateOBJMTL("Plinko", "Models//CR_plinko.obj", "Models//CR_plinko.mtl");
	meshList[GEO_CR_PLINKO]->textureID = LoadPNG("Images//CR_plinko.png");
	meshList[GEO_CR_PLINKO]->material = Material::Wood(WHITE);

	meshList[GEO_POOL] = MeshBuilder::GenerateOBJ("Table", "Models//CR_InflatablePool.obj");
	meshList[GEO_POOL]->textureID = LoadPNG("Images//CR_pool.png");
	meshList[GEO_POOL]->material = Material::Wood(WHITE);

	meshList[GEO_POOLWATER] = MeshBuilder::GenerateCircle("Circle", WHITE, 1);
	meshList[GEO_POOLWATER]->textureID = LoadPNG("Images//CR_water.png");
	meshList[GEO_POOLWATER]->material = Material::Wood(WHITE);

	meshList[GEO_DUCKY] = MeshBuilder::GenerateOBJMTL("duck", "Models//CR_ducky.obj", "Models//CR_ducky.mtl");
	meshList[GEO_DUCKY]->material = Material::Wood(WHITE);

	meshList[GEO_SHOOTINGDUCK] = MeshBuilder::GenerateOBJMTL("duckpewpew", "Models//CR_duck_w_target.obj", "Models//CR_duck_w_target.mtl");
	meshList[GEO_SHOOTINGDUCK]->material = Material::Wood(WHITE);

	meshList[GEO_TARGET] = MeshBuilder::GenerateOBJ("target", "Models//CR_target2.obj");
	meshList[GEO_TARGET]->textureID = LoadPNG("Images//CR_targetpic.png");
	meshList[GEO_TARGET]->material = Material::Wood(WHITE);

	meshList[GEO_CONVEYER] = MeshBuilder::GenerateOBJ("conveyer", "Models//CR_conveyerbelt.obj");
	meshList[GEO_CONVEYER]->textureID = LoadPNG("Images//CR_conveyerbelt.png");
	meshList[GEO_CONVEYER]->material = Material::Wood(WHITE);

	meshList[GEO_GUN] = MeshBuilder::GenerateOBJ("conveyer", "Models//CR_DS_Gun.obj");
	meshList[GEO_GUN]->textureID = LoadPNG("Images//CR_DS_Gun.png");
	meshList[GEO_GUN]->material = Material::Wood(WHITE);


	meshList[GEO_BOARD] = MeshBuilder::GenerateOBJ("conveyer", "Models//CR_noticeboard.obj");
	meshList[GEO_BOARD]->textureID = LoadPNG("Images//CR_boardimage.png");
	meshList[GEO_BOARD]->material = Material::Wood(WHITE);

	meshList[GEO_BALLOON] = MeshBuilder::GenerateOBJ("balloon", "Models//CR_balloon.obj");

	meshList[GEO_BOTTLES] = MeshBuilder::GenerateOBJ("Bottles", "Models//CR_allbottles.obj");
	meshList[GEO_BOTTLES]->material = Material::Metal(WHITE);

	meshList[GEO_RACK] = MeshBuilder::GenerateOBJ("Rack", "Models//CR_ring_toss_rack.obj");
	meshList[GEO_RACK]->textureID = LoadPNG("Images//CR_wood.png");
	meshList[GEO_RACK]->material = Material::Wood(WHITE);

	meshList[GEO_COUNTER] = MeshBuilder::GenerateOBJ("Counter", "Models//CR_ring_toss_counter.obj");
	meshList[GEO_COUNTER]->textureID = LoadPNG("Images//CR_counter.png");
	meshList[GEO_COUNTER]->material = Material::Wood(WHITE);

	meshList[GEO_RING] = MeshBuilder::GenerateOBJ("Ring", "Models//CR_ring_toss_rope.obj");
	meshList[GEO_RING]->textureID = LoadPNG("Images//CR_rope.png");
	meshList[GEO_RING]->material = Material::Wood(WHITE);


	meshList[GEO_TICKETBOOTHFRAME] = MeshBuilder::GenerateOBJ("Ticket booth frame", "Models//CR_ticketboothframe2.obj");
	meshList[GEO_TICKETBOOTHFRAME]->textureID = LoadPNG("Images//CR_ticketboothframe.png");
	meshList[GEO_TICKETBOOTHFRAME]->material = Material::Wood(WHITE);

	meshList[GEO_TICKETBOOTHBOX] = MeshBuilder::GenerateOBJ("Ticket booth box", "Models//CR_ticketboothbox.obj");
	meshList[GEO_TICKETBOOTHBOX]->textureID = LoadPNG("Images//CR_ticketboothbox.png");
	meshList[GEO_TICKETBOOTHBOX]->material = Material::Wood(WHITE);

	meshList[GEO_TICKETBOOTHLIGHTS] = MeshBuilder::GenerateOBJ("Ticket booth lights", "Models//CR_ticketboothlights2.obj");
	meshList[GEO_TICKETBOOTHLIGHTS]->textureID = LoadPNG("Images//CR_ticketboothlight.png");
	meshList[GEO_TICKETBOOTHLIGHTS]->material = Material::Wood(WHITE);

	meshList[GEO_TICKETBOOTHWINDOWS] = MeshBuilder::GenerateOBJ("Ticket booth windows", "Models//CR_ticketboothwindows2.obj");
	meshList[GEO_TICKETBOOTHWINDOWS]->textureID = LoadPNG("Images//CR_ticketboothwindow.png");
	meshList[GEO_TICKETBOOTHWINDOWS]->material = Material::Wood(WHITE);


	meshList[GEO_GRASS] = MeshBuilder::GenerateCube("Cube", YELLOW, 1.0f);
	meshList[GEO_GRASS]->textureID = LoadPNG("Images//CR_grass.png"); meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateCircle("Circle", WHITE, 200);
	meshList[GEO_CIRCUSTENT] = MeshBuilder::GenerateOBJMTL("CircusTent", "Models//CR_circusTentTest.obj", "Models//CR_circusTentTest.mtl");
	meshList[GEO_CIRCUSTENT]->textureID = LoadPNG("Images//CR_CircusTentTest.png");

	
	meshList[GEO_ENERGYFRAME] = MeshBuilder::GenerateQuad("EnergyFrame", WHITE, 1.0f);
	meshList[GEO_ENERGYFRAME]->textureID = LoadPNG("Images//EnergyFrame.png");
	meshList[GEO_ENERGYBAR] = MeshBuilder::GenerateQuad("EnergyBar", glm::vec3(0.62, 0.208, 0.62), 1.0f); 


	//plinko
	meshList[GEO_PLINKO] = MeshBuilder::GenerateOBJ("Plinko", "Models//plinko.obj");
	meshList[GEO_PLINKO]->material.kDiffuse = WHITE;
	meshList[GEO_PLINKO]->material.kAmbient = GREY;
	meshList[GEO_PLINKO]->material.kSpecular = glm::vec3(0.1, 0.1, 0.1);
	meshList[GEO_PLINKO]->material.kShininess = 1;

	meshList[GEO_PLINKO]->textureID = LoadPNG("Images//Atlas_1_baseColor.png");
	meshList[GEO_PLINKO_BOOTHROOF] = MeshBuilder::GenerateOBJMTL("BoothRoof", "Models//Plinko_BoothRoof.obj", "Models//Plinko_BoothRoof.mtl");
	meshList[GEO_PLINKO_BOOTHROOF]->textureID = LoadPNG("Images//Plinko_BoothRoof.png");
	meshList[GEO_PLINKO_BOOTHGUARDS] = MeshBuilder::GenerateOBJMTL("BoothGuards", "Models//Plinko_BoothGuards.obj", "Models//Plinko_BoothGuards.mtl");

	//can game
	meshList[GEO_POWERUI_FRAME] = MeshBuilder::GenerateQuad("PowerUi_Frame", glm::vec3(1, 1, 1), 1);
	meshList[GEO_POWERUI_FRAME]->textureID = LoadPNG("Images//CK_PowerUi_Frame.png");
	meshList[GEO_POWERUI_BAR] = MeshBuilder::GenerateQuad("PowerUi_Bar", glm::vec3(1, 1, 0), 1);

	meshList[GEO_BALL] = MeshBuilder::GenerateSphere("Ball", WHITE, 1.0f, 100, 100);
	meshList[GEO_BALL]->material = Material::Plastic(BLUE);

	meshList[GEO_CK_CAN] = MeshBuilder::GenerateOBJMTL("Can", "Models//CK_Can.obj", "Models//CK_Can.mtl");
	meshList[GEO_CK_CAN]->textureID = LoadPNG("Images//CK_Can.png");
	meshList[GEO_CK_CAN]->material = Material::Wood(WHITE);

	meshList[GEO_CK_TABLE] = MeshBuilder::GenerateOBJMTL("Table", "Models//CK_Table.obj", "Models//CK_Table.mtl");
	meshList[GEO_CK_TABLE]->textureID = LoadPNG("Images//CK_Table.png");
	meshList[GEO_CK_TABLE]->material = Material::Wood(WHITE);

	meshList[GEO_CK_BOOTHROOF] = MeshBuilder::GenerateOBJMTL("BoothRoof", "Models//CK_BoothRoof.obj", "Models//CK_BoothRoof.mtl");
	meshList[GEO_CK_BOOTHROOF]->textureID = LoadPNG("Images//CK_BoothRoof.png");
	meshList[GEO_CK_BOOTHROOF]->material = Material::Wood(WHITE);

	meshList[GEO_CK_BOOTHGUARDS] = MeshBuilder::GenerateOBJMTL("BoothGuards", "Models//CK_BoothGuards.obj", "Models//CK_BoothGuards.mtl");
	meshList[GEO_CK_BOOTHGUARDS]->material = Material::Metal(GREY);

	meshList[GEO_CK_COUNTER] = MeshBuilder::GenerateCube("Counter", glm::vec3(0.459, 0.302, 0), 1);
	meshList[GEO_CK_COUNTER]->textureID = LoadPNG("Images//CK_Wood.png");
	meshList[GEO_CK_COUNTER]->material = Material::Wood(glm::vec3(0.459, 0.302, 0));

	meshList[GEO_LIGHTBULB] = MeshBuilder::GenerateOBJMTL("Lamp", "Models//CK_LightBulb.obj", "Models//CK_LightBulb.mtl");
	meshList[GEO_LIGHTBULB]->material = Material::Wood(WHITE);

	//ring toss
	meshList[GEO_RT_PLANE] = MeshBuilder::GenerateQuad("Quad", WHITE, 75.f);
	meshList[GEO_RT_PLANE]->textureID = LoadPNG("Images//ground.png");
	meshList[GEO_RT_BOARD] = MeshBuilder::GenerateOBJ("board", "Models//ring_toss_rack.obj");
	meshList[GEO_RT_BOARD]->textureID = LoadPNG("Images//wood.png");
	meshList[GEO_RT_BOTTLE] = MeshBuilder::GenerateOBJ("Bottle", "Models//ring_toss_bottle.obj");
	meshList[GEO_RT_RING] = MeshBuilder::GenerateOBJ("Ring", "Models//ring_toss_rope.obj");
	meshList[GEO_RT_RING]->textureID = LoadPNG("Images//rope.png");
	meshList[GEO_RT_TIE] = MeshBuilder::GenerateOBJ("Tie", "Models//ring_toss_tie.obj");
	meshList[GEO_RT_TIE]->textureID = LoadPNG("Images//tie.png");
	meshList[GEO_RT_TABLE] = MeshBuilder::GenerateOBJ("Table", "Models//ring_toss_table.obj");
	meshList[GEO_RT_TABLE]->textureID = LoadPNG("Images//wood.png");
	meshList[GEO_RT_TABLECLOTH] = MeshBuilder::GenerateOBJ("Table cloth", "Models//ring_toss_tablecloth.obj");
	meshList[GEO_RT_TABLECLOTH]->textureID = LoadPNG("Images//tablecloth.png");
	meshList[GEO_RT_COUNTER] = MeshBuilder::GenerateOBJ("Counter", "Models//ring_toss_counter.obj");
	meshList[GEO_RT_COUNTER]->textureID = LoadPNG("Images//counter.png");
	meshList[GEO_RT_LIGHTPOLE] = MeshBuilder::GenerateOBJMTL("Light pole", "Models//ring_toss_lightpole.obj", "Models//ring_toss_lightpole.mtl");

	meshList[GEO_RT_BOARD]->material = Material::Wood(WHITE);





	// duck shooting:
	meshList[GEO_DS_QUAD] = MeshBuilder::GenerateQuad("Quad", BLACK, 1.0f);
	meshList[GEO_DS_BOOTHROOF] = MeshBuilder::GenerateOBJMTL("BoothRoof", "Models//DS_BoothRoof.obj", "Models//DS_BoothRoof.mtl");
	meshList[GEO_DS_BOOTHROOF]->textureID = LoadPNG("Images//DS_BoothRoof.png");
	meshList[GEO_DS_BOOTHROOF]->material = Material::Wood(WHITE);
	meshList[GEO_DS_BOOTHGUARDS] = MeshBuilder::GenerateOBJMTL("BoothGuards", "Models//DS_BoothGuards.obj", "Models//DS_BoothGuards.mtl");
	meshList[GEO_DS_BOOTHGUARDS]->material = Material::Metal(GREY);
	meshList[GEO_DS_COUNTER] = MeshBuilder::GenerateCube("Counter", glm::vec3(0.459, 0.302, 0), 1);
	meshList[GEO_DS_COUNTER]->textureID = LoadPNG("Images//DS_Wood.png");
	meshList[GEO_DS_COUNTER]->material = Material::Wood(glm::vec3(0.459, 0.302, 0));
	meshList[GEO_DS_LIGHTBULB] = MeshBuilder::GenerateOBJMTL("Lamp", "Models//DS_LightBulb.obj", "Models//DS_LightBulb.mtl");
	meshList[GEO_DS_LIGHTBULB]->material = Material::Wood(WHITE);
	meshList[GEO_DS_DUCK] = MeshBuilder::GenerateOBJMTL("Duck", "Models//DS_Duck.obj", "Models//DS_Duck.mtl");
	meshList[GEO_DS_DUCK]->material = Material::Wood(YELLOW);
	meshList[GEO_DS_GUN] = MeshBuilder::GenerateOBJ("Gun", "Models//DS_Gun.obj");
	meshList[GEO_DS_GUN]->material = Material::Wood(WHITE);
	meshList[GEO_DS_GUN]->textureID = LoadPNG("Images//DS_Gun.png");
	meshList[GEO_DS_BULLET] = MeshBuilder::GenerateSphere("Bullet", WHITE, 1);
	meshList[GEO_DS_BULLET]->material = Material::Plastic(BLUE);
	meshList[GEO_DS_PILLAR] = MeshBuilder::GenerateCube("Pillar", WHITE, 1);
	meshList[GEO_DS_PILLAR]->material = Material::Concrete(glm::vec3(1, 0.2, 0.2));
	meshList[GEO_DS_CONVEYOR] = MeshBuilder::GenerateOBJ("Conveyor", "Models//DS_ConveyorBelt.obj");
	meshList[GEO_DS_CONVEYOR]->textureID = LoadPNG("Images//DS_ConveyorBelt.png");
	meshList[GEO_DS_CONVEYOR]->material = Material::Wood(WHITE);
	meshList[GEO_DS_FAIRYLIGHT] = MeshBuilder::GenerateOBJMTL("FairyLight", "Models//DS_FairyLight.obj", "Models//DS_FairyLight.mtl");
}
