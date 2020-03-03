#ifndef __RR3D_COLLIDER__
#define __RR3D_COLLIDER__

#include <glm.hpp>

typedef struct OBBCollider_str {
	glm::vec3 pos;
	glm::vec3 rotation;
	glm::vec3 scale;
	float sphere_radius;
} OBBCollider;

#endif