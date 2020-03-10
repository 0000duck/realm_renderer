#ifndef __RR3D_TEXTURE__
#define __RR3D_TEXTURE__

#include <glew.h>

class Texture {
public:
	Texture(char*, double, double, unsigned, unsigned);
	~Texture();
	unsigned getHidden();
	GLuint getTexture();
	GLuint getWidth();
	GLuint getHeight();
	unsigned char* getPixels();
	void setHidden(unsigned);
	void setPixels();
private:
	unsigned hidden;
	unsigned char* pixels;
	GLuint texture;
	unsigned int width;
	unsigned int height;
	unsigned int parameter;
};

#endif
