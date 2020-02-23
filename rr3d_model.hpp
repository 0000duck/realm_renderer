#ifndef __RR3D_MODEL__
#define __RR3D_MODEL__

#include <glew.h>
#include <glm.hpp>

#include "rr3d_camera.hpp"
#include "rr3d_vertex.hpp"
#include "rr3d_texture.hpp"

typedef struct ModelData_str {
	char id;
	Texture* texture;
	GLuint program;
	GLuint buff_vertices;
	unsigned int vertices_cnt;
	RR3DVertex* vertices;
	glm::vec3 color;
	glm::vec3 translation;
	glm::vec3 scale;
	glm::vec3 rotation;
} ModelData;

class Model {
public:
	Model(ModelData* data);
	~Model();
	void Draw(Camera);
	unsigned getHidden();
	void setHidden(unsigned);
	ModelData* getData();
private:
	unsigned hidden;
	ModelData* data;
};

#endif