#include "GameObjectManager.h"
#include "SceneGame.h"

GameObjectManager* GameObjectManager::m_instance = NULL;
GameObjectManager::~GameObjectManager(void)
{
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

size_t GameObjectManager::ListSize(void) { return GameObjectManager::GetInstance()->m_data.size(); }

std::vector<GameObject*>& GameObjectManager::GetData(void) { return GameObjectManager::GetInstance()->m_data; }

void GameObjectManager::addItem(GameObject* obj)
{
    GameObjectManager::GetInstance()->m_data.push_back(obj);
}

void GameObjectManager::removeItem(GameObject* obj)
{
	int index;
	if (findObjInList(*obj, index))
	{
		delete m_data[index];
		m_data.erase(GameObjectManager::GetInstance()->m_data.begin() + index);
	}
}

bool GameObjectManager::findObjInList(const GameObject& value, int& index)
{
	bool isFound = false;
	
	for (size_t i = 0; i < GameObjectManager::GetInstance()->m_data.size(); ++i)
	{
		if (*GameObjectManager::GetInstance()->m_data[i] == value)
		{
			isFound = true;
			index = i;
			break;
		}
		else
			index = -1;
	}
	return isFound;
}

void GameObjectManager::IniAll(void)
{
	for (GameObject*& value : GameObjectManager::GetInstance()->m_data)
	{
		if(value)
			value->Start();
	}
}
void GameObjectManager::UpdateAll(void)
{
	for (GameObject*& value : GameObjectManager::GetInstance()->m_data)
	{
		if(value)
			value->Update();
	}
}

void GameObjectManager::RenderAll(SceneGame& scene)
{
	for (GameObject*& value : GameObjectManager::GetInstance()->m_data)
	{
		if (value)
			value->Render(scene);
	}
}

void GameObjectManager::clearAll(void)
{
	if (m_data.size() > 0)
	{
		for (GameObject*& obj : m_data)
			delete obj;
		m_data.clear();
	}
}

GameObject* GameObjectManager::operator[](const int& index) const { return m_data[index]; }
GameObject*& GameObjectManager::operator[](const int& index) { return m_data[index]; }
