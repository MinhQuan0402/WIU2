#include "Ducks.h"
#include "RigidBody.h"

Ducks::Ducks()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.1f;
	mat.m_bounciness = 0.2f;
	mat.m_friction = 1.0f;
}

void Ducks::Start()
{
}

void Ducks::Update()
{
}

void Ducks::Render(Scene& scene)
{
}
