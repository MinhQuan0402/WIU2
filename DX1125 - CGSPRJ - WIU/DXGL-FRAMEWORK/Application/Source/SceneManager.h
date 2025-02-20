//chinykian
//last updated: 26 Oct 2024
//use this singleton to change current scenes or stack scenes atop another
#pragma once

#include <vector>
#include "Scene.h"

class SceneManager
{
public:
	static SceneManager* GetInstance();
	static void DestroyInstance();

	void PushState(Scene* pScene);
	void ChangeState(Scene* pScene);
	void PopState();

	void Update(void);
	void LateUpdate(void);
	void Render();

private:
	std::vector<Scene*> m_scenes;
	static SceneManager* m_instance;
	//ensure an instance of this class can't be created outside of this class
	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
};
