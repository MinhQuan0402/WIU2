#include "CQuad.h"
#include "btBulletDynamicsCommon.h"

CQuad::CQuad(void)
	: m_origin{}, m_width{}, m_depth{}, UpL{ nullptr },
	UpR{ nullptr }, BotL{ nullptr }, BotR{ nullptr }, m_parent{ nullptr }, m_quadType{ ROOT }
{
}

CQuad::CQuad(glm::vec3 origin, float width, float depth, CQuad* parent)
	: m_origin{ origin }, m_width{ width }, m_depth{ depth }, UpL{ nullptr },
	UpR{ nullptr }, BotL{ nullptr }, BotR{ nullptr }, m_parent{ parent }, m_quadType{ ROOT }
{
}

CQuad::~CQuad(void)
{
}

unsigned CQuad::goCount(void) const { return GOList.size(); }

void CQuad::UpLSetter(CQuad* newQuad) { UpL = newQuad; }
CQuad* CQuad::UpLGetter(void) { return UpL; }

void CQuad::UpRSetter(CQuad* newQuad) { UpR = newQuad; }
CQuad* CQuad::UpRGetter(void) { return UpR; }

void CQuad::BotLSetter(CQuad* newQuad) { BotL = newQuad; }
CQuad* CQuad::BotLGetter(void) { return BotL; }

void CQuad::BotRSetter(CQuad* newQuad) { BotR = newQuad; }
CQuad* CQuad::BotRGetter(void) { return BotR; }

UnorderedVector<GameObject*> CQuad::GOListGetter(void) { return GOList; }

void CQuad::insertGO(GameObject* newObj) 
{
	GOList.push_back(newObj); 

	switch (newObj->rbGOType)
	{
	case GameObject::DYNAMIC: m_dynamicGOs.push_back(newObj);  break;
	case GameObject::KINEMATIC: m_kinematicGOs.push_back(newObj);  break;
	case GameObject::STATIC: m_staticGOs.push_back(newObj);  break;
	}
}

void CQuad::removeGO(GameObject* obj2Remove) { GOList.remove(obj2Remove); }

glm::vec3 CQuad::OriginGetter(void) const { return m_origin; }

void CQuad::DimensionGetter(int& width, int& depth) const
{
	width = m_width;
	depth = m_depth;
}

bool CQuad::isGOWithin(GameObject* obj) const
{
	for (GameObject* go : GOList) { if (go == obj) return true; }
	return false;
}

void CQuad::TypeSetter(ENUM_QUADTYPE type) { m_quadType = type; }

CQuad::ENUM_QUADTYPE CQuad::TypeGetter(void) const { return m_quadType; }

CQuad* CQuad::GetParent(void) { return m_parent; }
