#pragma once
#include <vector>
#include "GameObject.h"
class SceneGame;
class GameObjectManager
{
private:
	std::vector<GameObject*> m_data;

	static GameObjectManager* m_instance;

	GameObjectManager(void) {};
	~GameObjectManager(void);
public:
	static GameObjectManager* GetInstance(void);
	static void DestroyInstance(void);
	static size_t ListSize(void);
	static std::vector<GameObject*>& GetData(void);

	static void addItem(GameObject* obj);

	void removeItem(GameObject* obj);
	static bool findObjInList(const GameObject& value, int& index);

	static void IniAll(void);
	static void UpdateAll(void);
	static void RenderAll(SceneGame& scene);

	void clearAll(void);

	GameObject* operator[](const int& index) const;
	GameObject*& operator[](const int& index);
};

