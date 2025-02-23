#ifndef MATERIAL_H
#define MATERIAL_H

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Material
{
public:
	glm::vec3 kAmbient; //color for ambient light
	glm::vec3 kDiffuse; //color for diffuse light
	glm::vec3 kSpecular; //color for specular light
	glm::vec3 kEmission;
	float kShininess; //coefficient to define how shiny the material is
	Material(void);

	static float additionalAmbient;

	static Material Plastic(glm::vec3 color);
	static Material Metal(glm::vec3 color);
	static Material Sillicon(glm::vec3 color);
	static Material LED_Panel(glm::vec3 color);
	static Material Concrete(glm::vec3 color);
	static Material Wood(glm::vec3 color);

	unsigned size;

	Material& operator=(const Material& rhs);
};
#endif

