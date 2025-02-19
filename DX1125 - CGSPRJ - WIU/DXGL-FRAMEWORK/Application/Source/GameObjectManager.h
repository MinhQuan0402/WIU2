#pragma once
#include <UnorderedVector.h>
#include "GameObject.h"
#include "CQuad.h"

class SceneGame;
class GameObjectManager
{
private:
	CQuad* m_root;
	UnorderedVector<GameObject*> m_allGOs;
	UnorderedVector<GameObject*> m_staticGOs;
	UnorderedVector<GameObject*> m_kinematicGOs;
	UnorderedVector<GameObject*> m_dynamicGOs;

	static GameObjectManager* m_instance;

	void preOrder(CQuad* p, unsigned level);
	void quadDivision(CQuad* p);
	CQuad* findQuad(CQuad* p, GameObject* go);
	void postorderDeleteGO(CQuad* p, GameObject* obj2Remove);

	GameObjectManager(void);
public:
	~GameObjectManager(void);

	static GameObjectManager* GetInstance(void);
	static void DestroyInstance(void);
	static size_t ListSize(void);
	static UnorderedVector<GameObject*>& GetData(void);
	static CQuad* GetRoot(void);

	static void addItem(GameObject* obj);
	static void removeItem(GameObject* obj);
	static bool findObjInList(const GameObject& value, int& index);
	static void DivideQuad(void);

	static void IniAll(void);
	static void UpdateAll(void);
	static void RenderAll(Scene& scene);

	void clearQuad(CQuad* p);
	void clearGOs(void);

	static void PrintTree(void);

	GameObject* operator[](const int& index) const;
	GameObject*& operator[](const int& index);
};

