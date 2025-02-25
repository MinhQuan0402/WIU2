#include "ColliderManager.h"
#include "GameObject.h"
#include <MyMath.h>

ColliderManager* ColliderManager::m_instance = nullptr;
ColliderManager::ColliderManager(void)
{
}
ColliderManager::~ColliderManager(void)
{
    if(colliders.size() > 3)
        for (btCollisionShape* shape : colliders) delete shape;
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

void ColliderManager::ClearAll(void)
{
    if (colliders.size() > 0)
    {
        for (btCollisionShape* collider : colliders)
        {
            delete collider;
        }
    }
    colliders.clear();
}

BoxCollider::BoxCollider(GameObject* go, float width, float height, float depth)
    : btBoxShape{ btVector3(width / 2.0f, height / 2.0f, depth / 2.0f) },
    m_width{ width }, m_height{ height }, m_depth{ depth }
{
    setUserPointer(go);
}

void BoxCollider::GetDimension(float& width, float& height, float& depth) const {
    width = m_width;
    height = m_height;
    depth = m_depth;
}

SphereCollider::SphereCollider(GameObject* go, float rad)
    : btSphereShape{ rad }, m_rad{ rad }
{
    setUserPointer(go);
}

float SphereCollider::GetRadius(void) const { return m_rad; }

CylinderCollider::CylinderCollider(GameObject* go, const float& height, const float& rad)
    : btCylinderShape{ btVector3(rad / 2.0f, height / 2.0f, rad / 2.0f) },
    m_rad{ rad }, m_height{ height }
{
    setUserPointer(go);
}

void CylinderCollider::GetDimension(float& rad, float& height) const {
    rad = m_rad;
    height = m_height;
}

TorusCollider::TorusCollider(GameObject* go, const float& rad, const float& tubeRad, const int& numSegments)
    : m_numSegment{ numSegments }, m_rad{ rad }, m_tubeRad{ tubeRad }
{
    float angleStep = 2 * Math::PI / numSegments;  // Angle step for placing cylinders

    for (int i = 0; i < numSegments; i++) {
        // Calculate position in the torus ring
        float angle = i * angleStep;
        float x = rad * cos(angle);
        float y = rad * sin(angle);

        // Create a small cylinder to represent part of the torus
        btCylinderShape* tubeSegment = new CylinderCollider(go, tubeRad, tubeRad * 0.5f);

        // Set the transform for this segment
        btTransform localTransform;
        localTransform.setIdentity();
        localTransform.setOrigin(btVector3(x, y, 0));  // Position in torus ring

        // Rotate the segment to align with the torus curvature
        btQuaternion rotation(btVector3(0, 0, 1), angle);
        localTransform.setRotation(rotation);

        // Add the cylinder segment to the compound shape
        this->addChildShape(localTransform, tubeSegment);
    }
    this->setUserPointer(go);
}

void TorusCollider::GetDimension(float& rad, float& tubeRad, int& numSegments) const
{
    rad = m_rad;
    tubeRad = m_tubeRad;
    numSegments = m_numSegment;
}
