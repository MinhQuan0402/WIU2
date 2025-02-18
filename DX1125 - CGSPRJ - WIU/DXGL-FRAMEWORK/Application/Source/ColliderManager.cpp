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

BoxCollider::BoxCollider(float width, float height, float depth)
    : btBoxShape{ btVector3(width / 2.0f, height / 2.0f, depth / 2.0f) },
    m_width{ width }, m_height{ height }, m_depth{ depth }
{
}

void BoxCollider::GetDimension(float& width, float& height, float& depth) const {
    width = m_width;
    height = m_height;
    depth = m_depth;
}

SphereCollider::SphereCollider(float rad) : btSphereShape{ rad }, m_rad{ rad }
{
}

float SphereCollider::GetRadius(void) const { return m_rad; }

CylinderCollider::CylinderCollider(const float& height, const float& rad)
    : btCylinderShape{ btVector3(rad / 2.0f, height / 2.0f, rad / 2.0f) },
    m_rad{ rad }, m_height{ height }
{
}

void CylinderCollider::GetDimension(float& rad, float& height) const {
    rad = m_rad;
    height = m_height;
}
