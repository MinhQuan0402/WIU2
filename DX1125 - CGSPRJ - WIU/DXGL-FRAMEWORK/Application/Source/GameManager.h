#pragma once
#include <Transform.h>
class GameManager
{
private:
	static GameManager* m_instance;

	glm::vec3 playerPosition;
	glm::vec3 cameraTarget;
	
	int points;

	float energy;
	const float maxEnergy;

	GameManager();

public:
	~GameManager();

	static GameManager* GetInstance();
	static void DestroyInstance();

	glm::vec3 GetPlayerPosition();
	void SetPlayerPosition(float x, float y, float z);

	glm::vec3 GetCameraTarget();
	void SetCameraTarget(int x, int y, int z);

	int GetPoints();
	void SetPoints(int point);

	float GetEnergy();
	float GetMaxEnergy();
	void SetEnergy(int e);
};