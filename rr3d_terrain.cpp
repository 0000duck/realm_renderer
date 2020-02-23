#include <gtx/transform.hpp>

#include "rr3d_terrain.hpp"

Terrain::Terrain(TerrainData* data) {
	this->data = data;
}

Terrain::~Terrain() { }

void Terrain::Draw(Camera camera) {
	// shader
	glUseProgram(data->program);
	GLuint M_location = glGetUniformLocation(data->program, "u_M");
	GLuint V_location = glGetUniformLocation(data->program, "u_V");
	GLuint P_location = glGetUniformLocation(data->program, "u_P");
	glm::mat4 M = glm::translate(data->translation);
	glUniformMatrix4fv(M_location, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(V_location, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
	glUniformMatrix4fv(P_location, 1, GL_FALSE, &camera.getProjectionMatrix()[0][0]);
	// texture
	glActiveTexture(GL_TEXTURE0);
	if (data->texture != 0) {
		glBindTexture(GL_TEXTURE_2D, data->texture->getTexture());
	}
	// attributes
	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // normal
	glEnableVertexAttribArray(2); // uv
	for (int i = 0; i < data->gc; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_positions);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)((data->gc + 1) * i * 24));
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_normals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)((data->gc + 1) * i * 24));
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_uvs);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)((data->gc + 1) * i * 16));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (data->gc + 1) * 2);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned Terrain::getHidden() {
	return hidden;
}

void Terrain::setHidden(unsigned v) {
	hidden = v;
}
