#pragma once
#include "Scene.h"
#include "Mesh.h"
#include <Transform.h>
#include <UnorderedVector.h>

class MeshManager
{
private:
	static MeshManager* m_instance; 
	MeshManager();
public:
	UnorderedVector<Mesh*>  meshList;
	~MeshManager();
	static MeshManager* GetInstance();
	static void DestroyInstance();
	enum Meshes
	{
		//carnival roaming
		GEO_CIRCUSTENT,
		GEO_CIRCUSNAME,
		GEO_CANTENT,
		GEO_CANTENTROOF,
		GEO_STRIPEWALL,
		GEO_GRASS,
		GEO_TABLECAN,
		GEO_CAN,
		GEO_TOPCOTTONCANDY,
		GEO_MIDDLECOTTONCANDY,
		GEO_INNERMIDDLECOTTONCANDY,
		GEO_COTTONCANDYS,
		GEO_COTTONCANDYMACHINE,
		GEO_CARAMELAPPLE,
		GEO_CARAMELAPPLESTAND,
		GEO_BOTTOMCOTTONCANDY,
		GEO_CR_PLINKO,
		GEO_POOL,
		GEO_POOLWATER,
		GEO_DUCKY,
		GEO_SHOOTINGDUCK,
		GEO_TARGET,
		GEO_CONVEYER,
		GEO_GUN,
		GEO_BOARD,
		GEO_BALLOON,
		GEO_BOTTLES,
		GEO_RACK,
		GEO_COUNTER,
		GEO_RING,

		//plinko
		GEO_PLINKO,
		GEO_PLINKO_BOOTHROOF,
		GEO_PLINKO_BOOTHGUARDS,

		//Can Game
		GEO_POWERUI_FRAME,
		GEO_POWERUI_BAR,

		GEO_BALL,
		GEO_CK_CAN,
		GEO_CK_TABLE,
		GEO_CK_BOOTHROOF,
		GEO_CK_BOOTHGUARDS,
		GEO_CK_COUNTER,
		GEO_LIGHTBULB,

		NUM_MESHES
	};

	virtual void Init(); 
};