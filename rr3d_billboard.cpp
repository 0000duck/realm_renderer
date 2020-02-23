#include <gtx/transform.hpp>

#include "rr3d_billboard.hpp"

Billboard::Billboard(BillboardData* data) {
	this->data = data;
}

Billboard::~Billboard() { }

void Billboard::Draw(Camera camera) {
	// shader
	glUseProgram(data->program);
	GLuint M_location = glGetUniformLocation(data->program, "u_M");
	GLuint V_location = glGetUniformLocation(data->program, "u_V");
	GLuint P_location = glGetUniformLocation(data->program, "u_P");
	glm::mat4 M_translate = glm::translate(data->translation);
	glm::mat4 M_rotateX = glm::rotate(data->rotation[0], glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 M_rotateY = glm::rotate(data->rotation[1], glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 M_rotateZ = glm::rotate(data->rotation[2], glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 M_rotate = M_rotateX * M_rotateY * M_rotateZ;
	glm::mat4 M_scale = glm::scale(data->scale);
	glm::mat4 M = M_translate * M_rotate * M_scale;
	glUniformMatrix4fv(M_location, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(V_location, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
	glUniformMatrix4fv(P_location, 1, GL_FALSE, &camera.getProjectionMatrix()[0][0]);
	// texture
	glActiveTexture(GL_TEXTURE0);
	if (data->texture != 0) {
		glBindTexture(GL_TEXTURE_2D, data->texture->getTexture());
	}
	// draw
	glDrawArrays(GL_POINTS, 0, 1);
	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned Billboard::getHidden() {
	return hidden;
}

void Billboard::setHidden(unsigned v) {
	hidden = v;
}

BillboardData* Billboard::getData() {
	return data;
}
