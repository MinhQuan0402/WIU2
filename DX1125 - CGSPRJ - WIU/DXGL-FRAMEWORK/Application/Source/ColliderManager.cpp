#include "ColliderManager.h"

ColliderManager* ColliderManager::m_instance = nullptr;
ColliderManager::ColliderManager(void)
{
}
ColliderManager* ColliderManager::GetInstance(void)
{
    if (!m_instance) m_instance = new ColliderManager();
    return m_instance;
}

void ColliderManager::DestroyInstance(void)
{
    if (m_instance) delete m_instance;
}

void ColliderManager::AddCollider(btCollisionShape* collider2Add) { colliders.push_back(collider2Add); }
void ColliderManager::RemoveCollider(btCollisionShape* collider2Remove) { colliders.remove(collider2Remove); }
