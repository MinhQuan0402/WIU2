#ifndef LIGHT_H
#define LIGHT_H

#include "GameObject.h"

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Light : public GameObject
{
public:
	enum LIGHT_TYPE
	{
		LIGHT_POINT = 0,
		LIGHT_DIRECTIONAL,
		LIGHT_SPOT,
	};
	LIGHT_TYPE type;
	glm::vec3 color; // Define the light color
	float power; // Define the light power
	float kC, kL, kQ; // Variables to calculate attenuation
	glm::vec3 spotDirection;
	float cosCutoff;
	float cosInner;
	float exponent;
	
	float tempPow;

	Light(void);
	virtual ~Light(void);

	void EnableLight(bool on);
	void UpdateLight(glm::vec3 pos, glm::vec3 direction);
};
#endif

