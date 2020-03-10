#include <glew.h>
#include <windows.h>

#include "lodepng/lodepng.hpp"
#include "rr3d_texture.hpp"

Texture::Texture(char* fn, double m, double p, unsigned w, unsigned h) {
	width = w;
	height = h;
	pixels = 0;
	hidden = 0;

	if (fn != NULL) {
		unsigned int error = lodepng_decode32_file(&pixels, &width, &height, fn);
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	if (p == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else if (p == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// dont save memory
	if (m == 0) { 
		if (pixels != NULL) {
			free(pixels);
		}
		pixels = NULL;
	}
}

Texture::~Texture() {
	glDeleteTextures(1, &texture);
	if (pixels != NULL) { free(pixels); }
}

unsigned Texture::getHidden() {
	return hidden;
}

GLuint Texture::getTexture() {
	return texture;
}

GLuint Texture::getWidth() {
	return width;
}

GLuint Texture::getHeight() {
	return height;
}

unsigned char* Texture::getPixels() {
	return pixels;
}

void Texture::setHidden(unsigned v) {
	hidden = v;
}

void Texture::setPixels() {
	pixels = NULL;
}
