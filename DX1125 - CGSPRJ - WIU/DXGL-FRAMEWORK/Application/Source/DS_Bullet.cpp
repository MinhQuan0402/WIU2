#include "DS_Bullet.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_Bullet::DS_Bullet() 
{	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Bullet::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 1;
	m_transform.ScaleBy(0.213822, 0.213822, 0.213822);
	rb = addCylinderCollider(this, 1.11 * m_transform.m_scale.x, 0.20 * m_transform.m_scale.x, mat);
}

void DS_Bullet::Update()
{
}

void DS_Bullet::Render(Scene& scene)
{
	/*scene.modelStack.PushMatrix();
	scene.modelStack.Translate(m_transform.m_position.x, m_transform.m_position.y, m_transform.m_position.z);
	scene.modelStack.Rotate(m_transform.m_rotation.y, 0, 1, 0);
	scene.modelStack.Rotate(m_transform.m_rotation.x, 1, 0, 0);
	
	
	scene.modelStack.Scale(m_transform.m_scale.x, m_transform.m_scale.y, m_transform.m_scale.z);*/
	//scene.RenderMesh(scene.meshList[SceneDuckShooting::GEO_BULLET], scene.enableLight, m_transform);
	scene.RenderRigidMesh(scene.meshList[SceneDuckShooting::GEO_BULLET], scene.enableLight, m_transform, rb);
	//scene.modelStack.PopMatrix();
}
