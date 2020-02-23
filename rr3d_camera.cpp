#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>

#include "rr3d_camera.hpp"

Camera::Camera(float fov, float aspect, float znear, float zfar) {
	pitch = 0;
	yaw = 0;
	roll = 0;
	up = glm::vec3(0.0, 1.0, 0.0);
	direction = glm::vec3(0.0, 0.0, -1.0);
	right = glm::cross(up, direction);
	translation = glm::vec3(0.0, 0.0, 1.0);
	P = glm::perspective(glm::radians(fov), aspect, znear, zfar);
	V = glm::lookAt(translation, translation + direction, up);
}

Camera::~Camera() {  }

glm::mat4 Camera::getProjectionMatrix() {
	return P;
}

glm::mat4 Camera::getViewMatrix() {
	return V;
}

float* Camera::getTranslationX() {
	return &translation[0];
}

float* Camera::getTranslationY() {
	return &translation[1];
}

float* Camera::getTranslationZ() {
	return &translation[2];
}

void Camera::setPitch(float v) {
	pitch = v;
	direction.y = cos(glm::radians(pitch)) - sin(glm::radians(pitch));
	direction.z = sin(glm::radians(pitch)) + cos(glm::radians(pitch));
	
	V = glm::lookAt(translation, translation + direction, up);
	right = glm::vec3(direction.z, 0, -direction.x);
}

void Camera::setYaw(float v) {
	yaw = v;
	direction.x = cos(glm::radians(yaw)) - sin(glm::radians(yaw));
	direction.z = sin(glm::radians(yaw)) + cos(glm::radians(yaw));

	V = glm::lookAt(translation, translation + direction, up);
	right = glm::vec3(direction.z, 0, -direction.x);
}

void Camera::setRoll(float v) {
	roll = v;
	up.x = cos(glm::radians(roll)) - sin(glm::radians(roll));
	up.y = sin(glm::radians(roll)) + cos(glm::radians(roll));

	V = glm::lookAt(translation, translation + direction, up);
	right = glm::vec3(direction.z, 0, -direction.x);
}

void Camera::addPitch(float v) {
	pitch += v;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	V = glm::lookAt(translation, translation + direction, up);
	right = glm::vec3(direction.z, 0, -direction.x);
}

void Camera::addYaw(float v) {
	yaw += v;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	V = glm::lookAt(translation, translation + direction, up);
	right = glm::vec3(direction.z, 0, -direction.x);
}

void Camera::addRoll(float v) {
	roll += v;
	up.x = cos(glm::radians(roll)) - sin(glm::radians(roll));
	up.y = sin(glm::radians(roll)) + cos(glm::radians(roll));

	V = glm::lookAt(translation, translation + direction, up);
	right = glm::vec3(direction.z, 0, -direction.x);
}

void Camera::setTranslation(float x, float y, float z) {
	translation = glm::vec3(x, y, z);
	V = glm::lookAt(translation, translation + direction, up);
}

float Camera::getPitch() {
	return pitch;
}

float Camera::getYaw() {
	return yaw;
}

float Camera::getRoll() {
	return roll;
}

void Camera::move(float v) {
	translation += direction * glm::vec3(v);
}

void Camera::strafe(float v) {
	glm::vec3 dir = glm::vec3(cos(glm::radians(yaw - 90)) * 1, 0, sin(glm::radians(yaw - 90) * 1));
	translation += dir * glm::vec3(v);
}

void Camera::lift(float v) {
	translation += up * glm::vec3(v);
}