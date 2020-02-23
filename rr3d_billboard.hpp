#ifndef __RR3D_BILLBOARD__
#define __RR3D_BILLBOARD__

#include <glew.h>
#include <glm.hpp>

#include "rr3d_camera.hpp"
#include "rr3d_texture.hpp"

typedef struct BillboardData_str {
	Texture* texture;
	GLuint program;
	glm::vec3 translation;
	glm::vec3 scale;
	glm::vec3 rotation;
} BillboardData;

class Billboard {
public:
	Billboard(BillboardData* data);
	~Billboard();
	void Draw(Camera);
	unsigned getHidden();
	void setHidden(unsigned);
	BillboardData* getData();
private:
	unsigned hidden;
	BillboardData* data;
};

#endif