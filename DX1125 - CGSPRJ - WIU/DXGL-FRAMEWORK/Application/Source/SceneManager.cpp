#include "SceneManager.h"

SceneManager* SceneManager::m_instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (!m_instance) m_instance = new SceneManager();
	return m_instance;
}

void SceneManager::DestroyInstance()
{
	if (m_instance) delete m_instance;
}

SceneManager::~SceneManager()
{
	for (Scene* pScene : m_scenes)
		delete pScene;
}

void SceneManager::PushState(Scene* pScene)
{
	pScene->Init();
	m_scenes.push_back(pScene);
}

void SceneManager::ChangeState(Scene* pScene)
{
	size_t size = m_scenes.size();
	if (size > 0)
	{
		m_scenes[size - 1]->Exit();
		delete m_scenes[size - 1];
		m_scenes.pop_back();
	}

	pScene->Init();
	m_scenes.push_back(pScene);
}

void SceneManager::PopState()
{
	if (m_scenes.size() > 0)
	{
		m_scenes[m_scenes.size() - 1]->Exit();
		delete m_scenes[m_scenes.size() - 1];
		m_scenes.pop_back();
	}
}

void SceneManager::Update(void)
{
	size_t size = m_scenes.size();
	if (size == 0)
		return; //no scenes to update

	m_scenes[size - 1]->Update();
}

void SceneManager::Render()
{
	//draw all scenes from bottom to top
	for (Scene* pScene : m_scenes)
		pScene->Render();
}
