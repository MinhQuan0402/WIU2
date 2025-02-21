#include "GameObjectManager.h"
#include "SceneGame.h"

GameObjectManager* GameObjectManager::m_instance = NULL;

void GameObjectManager::preOrder(CQuad* p, unsigned level)
{
	int tempLevel = level;
	if (p != nullptr)
	{
		std::string toPrint;
		for (unsigned i = 0; i < level; ++i)
			toPrint += '-';
		toPrint += std::to_string(level);

		switch (p->TypeGetter())
		{
		case 1:
			toPrint += ":UpL ";
			break;
		case 2:
			toPrint += ":UpR ";
			break;
		case 3:
			toPrint += ":BotL ";
			break;
		case 4:
			toPrint += ":BotR ";
			break;
		default:
			toPrint += ":ROOT ";
			break;
		}

		glm::vec3 pOrigin = p->OriginGetter();
		int width, height;
		p->DimensionGetter(width, height);

		toPrint += "(" + std::to_string(pOrigin.x) + ", " + std::to_string(pOrigin.z) + ") " +
			'H' + std::to_string(height) + 'W' + std::to_string(width) + " " + "(" + std::to_string(p->goCount()) + ")";

		std::cout << toPrint << std::endl;

		preOrder(p->UpLGetter(), ++level);
		preOrder(p->UpRGetter(), level);
		preOrder(p->BotLGetter(), level);
		preOrder(p->BotRGetter(), level);
	}
}

void GameObjectManager::quadDivision(CQuad* p)
{
	int width, depth;
	p->DimensionGetter(width, depth);

	glm::vec3 pOrigin = p->OriginGetter();
	float halfWidth = width / 2.0f, halfDepth = depth / 2.0f;

	glm::vec3 upLOrigin = pOrigin, upROrigin = glm::vec3{ pOrigin.x + halfWidth, 0.0f, pOrigin.z },
		botLOrigin = glm::vec3{ pOrigin.x, 0.0f, pOrigin.z - halfDepth }, botROrigin = glm::vec3{ pOrigin.x + halfWidth, 0.0f, pOrigin.z - halfDepth };

	for (size_t i = 0; i < p->goCount(); ++i)
	{
		GameObject* currentGO = p->GOListGetter()[i];

		glm::vec3 GOPosition = currentGO->m_transform.m_position;
		if (GOPosition.x < upLOrigin.x + halfWidth && GOPosition.z >= upLOrigin.z - halfDepth)
		{
			if (!p->UpLGetter()) p->UpLSetter(new CQuad{ upLOrigin, halfWidth, halfDepth, p });
			p->UpLGetter()->insertGO(currentGO);
			p->UpLGetter()->TypeSetter(p->E_UPL);
		}
		else if (GOPosition.x < upROrigin.x + width && GOPosition.z >= upROrigin.z - halfDepth)
		{
			if (!p->UpRGetter()) p->UpRSetter(new CQuad{ upROrigin, halfWidth, halfDepth, p });
			p->UpRGetter()->insertGO(currentGO);
			p->UpRGetter()->TypeSetter(p->E_UPR);
		}
		else if (GOPosition.x < botLOrigin.x + halfWidth && GOPosition.z >= pOrigin.z - depth)
		{
			if (!p->BotLGetter()) p->BotLSetter(new CQuad{ botLOrigin, halfWidth, halfDepth, p });
			p->BotLGetter()->insertGO(currentGO);
			p->BotLGetter()->TypeSetter(p->E_BOTL);
		}
		else if (GOPosition.x < pOrigin.x + width && GOPosition.z >= pOrigin.z - depth)
		{
			if (!p->BotRGetter()) p->BotRSetter(new CQuad{ botROrigin, halfWidth, halfDepth, p });
			p->BotRGetter()->insertGO(currentGO);
			p->BotRGetter()->TypeSetter(p->E_BOTR);
		}
	}

	if (p->UpLGetter() && p->UpLGetter()->goCount() > 3) quadDivision(p->UpLGetter());
	if (p->UpRGetter() && p->UpRGetter()->goCount() > 3) quadDivision(p->UpRGetter());
	if (p->BotLGetter() && p->BotLGetter()->goCount() > 3) quadDivision(p->BotLGetter());
	if (p->BotRGetter() && p->BotRGetter()->goCount() > 3) quadDivision(p->BotRGetter());
}
CQuad* GameObjectManager::findQuad(CQuad* p, GameObject* go)
{
	if (p != nullptr)
	{
		if (p->UpLGetter() && p->UpLGetter()->isGOWithin(go))
			return findQuad(p->UpLGetter(), go);

		if (p->UpRGetter() && p->UpRGetter()->isGOWithin(go))
			return findQuad(p->UpRGetter(), go);

		if (p->BotLGetter() && p->BotLGetter()->isGOWithin(go))
			return findQuad(p->BotLGetter(), go);

		if (p->BotRGetter() && p->BotRGetter()->isGOWithin(go))
			return findQuad(p->BotRGetter(), go);
	}

	return p;
}
void GameObjectManager::postorderDeleteGO(CQuad* p, GameObject* obj2Remove)
{
	if (p->GetParent() != nullptr)
	{
		p->removeGO(obj2Remove);
		postorderDeleteGO(p->GetParent(), obj2Remove);
	}
}

GameObjectManager::GameObjectManager(void)
{
	m_root = new CQuad{ glm::vec3{-750.0f, 0.0f, 500.0f}, 1500.0f, 1000.0f };
}

GameObjectManager::~GameObjectManager(void)
{
	clearGOs();
	clearQuad(m_root);
}

GameObjectManager* GameObjectManager::GetInstance(void)
{
    if (m_instance == NULL)
        m_instance = new GameObjectManager();
    return m_instance;
}
void GameObjectManager::DestroyInstance(void)
{
    if (m_instance) delete m_instance;
}

size_t GameObjectManager::ListSize(void) { return GameObjectManager::GetInstance()->m_allGOs.size(); }
UnorderedVector<GameObject*>& GameObjectManager::GetData(void) { return GameObjectManager::GetInstance()->m_allGOs; }

CQuad* GameObjectManager::GetRoot(void) { return GameObjectManager::GetInstance()->m_root; }

void GameObjectManager::addItem(GameObject* obj)
{
	GameObjectManager::GetInstance()->GetRoot()->insertGO(obj);
    GameObjectManager::GetInstance()->m_allGOs.push_back(obj);

	switch (obj->rbGOType)
	{
	case GameObject::DYNAMIC:  GameObjectManager::GetInstance()->m_dynamicGOs.push_back(obj);  break;
	case GameObject::KINEMATIC:  GameObjectManager::GetInstance()->m_kinematicGOs.push_back(obj);  break;
	case GameObject::STATIC:  GameObjectManager::GetInstance()->m_staticGOs.push_back(obj);  break;
	}
}
void GameObjectManager::removeItem(GameObject* obj)
{
	CQuad* quad = GameObjectManager::GetInstance()->findQuad(GameObjectManager::GetInstance()->m_root, obj);
	GameObjectManager::GetInstance()->postorderDeleteGO(quad, obj);
	GameObjectManager::GetInstance()->m_allGOs.remove(obj); 
}
bool GameObjectManager::findObjInList(const GameObject& value, int& index)
{
	bool isFound = false;
	
	for (size_t i = 0; i < GameObjectManager::GetInstance()->m_allGOs.size(); ++i)
	{
		if (*GameObjectManager::GetInstance()->m_allGOs[i] == value)
		{
			isFound = true;
			index = i;
			break;
		}
		else index = -1;
	}
	return isFound;
}

void GameObjectManager::DivideQuad(void) { GameObjectManager::GetInstance()->quadDivision(GameObjectManager::GetInstance()->m_root); }

void GameObjectManager::IniAll(void)
{
	for (GameObject*& go : GameObjectManager::GetInstance()->m_allGOs) { if(go) go->Start(); }
}
void GameObjectManager::UpdateAll(void)
{
	for (GameObject*& value : GameObjectManager::GetInstance()->m_allGOs) { if (value) value->Update(); }
}
void GameObjectManager::LateUpdateAll(void)
{
	for (GameObject*& value : GameObjectManager::GetInstance()->m_allGOs) { if (value) value->LateUpdate(); }
}
void GameObjectManager::RenderAll(Scene& scene)
{
	for (GameObject*& value : GameObjectManager::GetInstance()->m_allGOs) { if (value) value->Render(scene); }
}

void GameObjectManager::clearQuad(CQuad* p)
{
	if (p != nullptr)
	{
		clearQuad(p->UpLGetter());
		clearQuad(p->UpRGetter());
		clearQuad(p->BotLGetter());
		clearQuad(p->BotRGetter());
		delete p;
	}
}
void GameObjectManager::clearGOs(void)
{
	if (m_allGOs.size() > 0)
	{
		for (GameObject*& obj : m_allGOs) delete obj;
		m_allGOs.clear();
	}
}

void GameObjectManager::PrintTree(void)
{
	int level = 0;
	GameObjectManager::GetInstance()->preOrder(GameObjectManager::GetInstance()->m_root, level);
}

GameObject* GameObjectManager::operator[](const int& index) const { return m_allGOs[index]; }
GameObject*& GameObjectManager::operator[](const int& index) { return m_allGOs[index]; }
