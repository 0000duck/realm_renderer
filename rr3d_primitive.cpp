#include <gtx/transform.hpp>
#include "rr3d_primitive.hpp"

Primitive::Primitive(PrimitiveData* data) {
	this->data = data;
}

Primitive::~Primitive() {  }

void Primitive::Draw(Camera camera) {
	// shader
	glUseProgram(data->program);
	GLuint M_location = glGetUniformLocation(data->program, "M");
	GLuint V_location = glGetUniformLocation(data->program, "V");
	GLuint P_location = glGetUniformLocation(data->program, "P");
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
	// attributes
	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // color
	glEnableVertexAttribArray(2); // normal
	glEnableVertexAttribArray(3); // uv
	glBindBuffer(GL_ARRAY_BUFFER, data->buff_vertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 44, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 44, (void*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 44, (void*)24);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 44, (void*)36);
	glDrawArrays(GL_TRIANGLES , 0, data->vertices_cnt);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

unsigned Primitive::getHidden() {
	return hidden;
}

void Primitive::setHidden(unsigned v) {
	hidden = v;
}

PrimitiveData* Primitive::getData() {
	return data;
}