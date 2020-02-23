#include "rr3d_window.hpp"

Window::Window(GLuint framebuffer, Texture* texture_screen, Texture* texture_col, Texture* texture_depth, HWND hwnd, HDC hdc, HGLRC hglrc) {
	this->hwnd = hwnd;
	this->hdc = hdc;
	this->hglrc = hglrc;
	this->framebuffer = framebuffer;
	this->texture_screen = texture_screen;
	this->texture_col = texture_col;
	this->texture_depth = texture_depth;
	this->quad = (GLfloat*)malloc(16 * 4);
	this->quad[0] = -1.0f; this->quad[1] = 1.0f; this->quad[2] = 0.0f; this->quad[3] = 1.0f;
	this->quad[4] = 1.0f; this->quad[5] = 1.0f; this->quad[6] = 1.0f; this->quad[7] = 1.0f;
	this->quad[8] = 1.0f; this->quad[9] = -1.0f; this->quad[10] = 1.0f; this->quad[11] = 0.0f;
	this->quad[12] = -1.0f; this->quad[13] = -1.0f; this->quad[14] = 0.0f; this->quad[15] = 0.0f;
	glGenBuffers(1, &(this->quad_buff));
	glBindBuffer(GL_ARRAY_BUFFER, this->quad_buff);
	glBufferData(GL_ARRAY_BUFFER, 16 * 4, this->quad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Window::setProgram(GLuint program) {
	this->program = program;
}

void Window::setColor(double color) {
	this->color = color;
}

void Window::setHwnd(HWND hwnd) {
	this->hwnd = hwnd;
}

void Window::setHdc(HDC hdc) {
	this->hdc = hdc;
}

void Window::setCamera(Camera* camera) {
	this->camera = camera;
}

GLuint Window::getQuadBuff() {
	return quad_buff;
}

GLuint Window::getProgram() {
	return program;
}

Texture* Window::getTextureScreen() {
	return texture_screen;
}

Texture* Window::getTextureCol() {
	return texture_col;
}

Texture* Window::getTextureDepth() {
	return texture_depth;
}

GLuint Window::getFramebuffer() {
	return framebuffer;
}

double Window::getColor() {
	return color;
}

HWND Window::getHwnd() {
	return hwnd;
}

HDC Window::getHdc() {
	return hdc;
}

HGLRC Window::getHglrc() {
	return hglrc;
}

Camera* Window::getCamera() {
	return camera;
}