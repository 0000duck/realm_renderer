#ifndef __RR3D_VERTEX__
#define __RR3D_VERTEX__

#include <glm.hpp>

struct RR3DVertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;
	RR3DVertex(float x, float y, float z, float r, float g, float b, float nx, float ny, float nz, float u, float v) {
		pos[0] = x; pos[1] = y; pos[2] = z;
		color[0] = r; color[1] = g; color[2] = b;
		normal[0] = nx; normal[1] = ny; normal[2] = nz;
		uv[0] = u; uv[1] = v;
	}
};

struct RR3DVertex3 {
	glm::vec3 pos;
};

struct RR3DVertexUV {
	glm::vec2 uv;
};

#endif