#include "GameManager.h"

GameManager* GameManager::m_instance = nullptr;
GameManager::GameManager()
	: points{}, playerPosition{}, cameraTarget{}, energy{ 0 }, maxEnergy{ 100 }
{
}

GameManager::~GameManager()
{
}

GameManager* GameManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new GameManager();
	}
	return m_instance;
}

void GameManager::DestroyInstance()
{
	if (m_instance)
		delete m_instance;
}

glm::vec3 GameManager::GetPlayerPosition()
{
	return playerPosition;

}

void GameManager::SetPlayerPosition(float x, float y, float z)
{
	playerPosition.x = x;
	playerPosition.y = y;
	playerPosition.z = z;
}

glm::vec3 GameManager::GetCameraTarget()
{
	return cameraTarget;
}

void GameManager::SetCameraTarget(int x, int y, int z)
{
	cameraTarget.x = x;
	cameraTarget.y = y;
	cameraTarget.z = z;
}

int GameManager::GetPoints()
{
	return points;
}

void GameManager::SetPoints(int point)
{
	points = point;
}

float GameManager::GetEnergy()
{
	return energy;
}

float GameManager::GetMaxEnergy()
{
	return maxEnergy;
}

void GameManager::SetEnergy(int e)
{
	energy = e;

	if (energy > maxEnergy) {
		energy = maxEnergy;
	}
	else if (energy <= 0) {
		energy = 0;
	}
}
