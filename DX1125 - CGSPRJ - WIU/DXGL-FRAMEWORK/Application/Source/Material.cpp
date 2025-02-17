#include "Material.h"

float Material::additionalAmbient = 0.0f;
Material::Material(void) : kAmbient(0.0f, 0.0f, 0.0f), kDiffuse(0.0f, 0.0f, 0.0f), kSpecular(0.0f, 0.0f, 0.0f), kShininess(1.f), size(0)
{
}

Material Material::Plastic(glm::vec3 color)
{
	float ambientStrength = 0.2f + additionalAmbient;
	float diffuseStrength = 0.5f;
	float specularStrength = 0.4f;

	Material plastic;
	plastic.kAmbient = color * ambientStrength;
	plastic.kDiffuse = color;
	plastic.kSpecular = glm::vec3(specularStrength, specularStrength, specularStrength);
	plastic.kShininess = 0.2f;
	return plastic;
}

Material Material::Metal(glm::vec3 color)
{
	float ambientStrength = 0.3f + additionalAmbient;
	float diffuseStrength = 0.6f;
	float specularStrength = 0.8f;

	Material metal;
	metal.kAmbient = color * ambientStrength;
	metal.kDiffuse = color * diffuseStrength;
	metal.kSpecular = glm::vec3(specularStrength, specularStrength, specularStrength);
	metal.kShininess = 5.0f;
	return metal;
}

Material Material::Sillicon(glm::vec3 color)
{
	float ambientStrength = 0.2f + additionalAmbient;
	float diffuseStrength = 0.4f;
	float specularStrength = 0.4f;

	Material sillicon;
	sillicon.kAmbient = color * ambientStrength;
	sillicon.kDiffuse = color;
	sillicon.kSpecular = glm::vec3(specularStrength, specularStrength, specularStrength);
	sillicon.kShininess = 1.0f;
	return sillicon;
}

Material Material::LED_Panel(glm::vec3 color)
{
	float ambientStrength = 1.0f;
	float diffuseStrength = 0.8f;
	float specularStrength = 1.0f;

	Material led;
	led.kAmbient = ambientStrength * color;
	led.kDiffuse = color;
	led.kSpecular = glm::vec3(specularStrength, specularStrength, specularStrength + 0.1f);
	led.kShininess = 1.0f;
	return led;
}

Material Material::Concrete(glm::vec3 color)
{
	float ambientStrength = 0.1f;
	float diffuseStrength = 0.9f;
	float specularStrength = 0.3f;

	Material concrete;
	concrete.kAmbient = color * ambientStrength;
	concrete.kDiffuse = color * diffuseStrength;
	concrete.kSpecular = glm::vec3(specularStrength, specularStrength, specularStrength);
	concrete.kShininess = 1.0f;
	return concrete;
}

Material Material::Wood(glm::vec3 color)
{
	float ambientStrength = 0.1f;
	float diffuseStrength = 0.4f;
	float specularStrength = 0.1f;

	Material wood;
	wood.kAmbient = color * ambientStrength;
	wood.kDiffuse = color * diffuseStrength;
	wood.kSpecular = glm::vec3(specularStrength, specularStrength, specularStrength);
	wood.kShininess = 1.0f;
	return wood;
}

Material& Material::operator=(const Material& rhs)
{
	kAmbient = rhs.kAmbient;
	kDiffuse = rhs.kDiffuse;
	kSpecular = rhs.kSpecular;
	kShininess = rhs.kShininess;
	size = rhs.size;
	return *this;
}
