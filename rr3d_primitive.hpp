#ifndef __RR3D_PRIMITIVE__
#define __RR3D_PRIMITIVE__

#include <glew.h>
#include <glm.hpp>

#include "rr3d_camera.hpp"
#include "rr3d_vertex.hpp"

typedef struct PrimitiveData_str {
	char id;
	GLuint program;
	GLuint buff_vertices;
	unsigned int vertices_cnt;
	RR3DVertex* vertices;
	glm::vec3 color;
	glm::vec3 translation;
	glm::vec3 scale;
	glm::vec3 rotation;
} PrimitiveData;

class Primitive {
public:
	Primitive(PrimitiveData* data);
	~Primitive();
	void Draw(Camera);
	unsigned getHidden();
	void setHidden(unsigned);
	PrimitiveData* getData();
private:
	unsigned hidden;
	PrimitiveData* data;
};

#endif