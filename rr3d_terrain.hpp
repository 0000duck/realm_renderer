#ifndef __RR3D_TERRAIN__
#define __RR3D_TERRAIN__

#include <glew.h>
#include "rr3d_texture.hpp"
#include "rr3d_camera.hpp"

typedef struct TerrainData_str {
	Texture* texture;
	GLuint program;
	GLuint buff_positions;
	GLuint buff_normals;
	GLuint buff_uvs;
	unsigned int vertices_cnt;
	GLfloat* positions;
	GLfloat* normals;
	GLfloat* uvs;
	unsigned int gs;
	unsigned int gc;
	glm::vec3 translation;
} TerrainData;

class Terrain {
public:
	Terrain(TerrainData*);
	~Terrain();
	unsigned getHidden();
	void setHidden(unsigned);
	void Draw(Camera);
private:
	unsigned hidden;
	TerrainData* data;
};

#endif