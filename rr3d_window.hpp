#ifndef __RR3D_WINDOW__
#define __RR3D_WINDOW__

#include <glew.h>
#include "rr3d_camera.hpp"
#include "rr3d_texture.hpp"

#include <windows.h>

class Window {
public:
	Window(GLuint, Texture*, Texture*, Texture*, HWND, HDC, HGLRC);
	void setProgram(GLuint);
	void setColor(double);
	void setHwnd(HWND);
	void setHdc(HDC);
	void setCamera(Camera*);
	GLuint getQuadBuff();
	GLuint getProgram();
	Texture* getTextureCol();
	Texture* getTextureDepth();
	Texture* getTextureScreen();
	GLuint getFramebuffer();
	double getColor();
	HWND getHwnd();
	HDC getHdc();
	HGLRC getHglrc();
	Camera* getCamera();
private:
	GLfloat* quad;
	GLuint quad_buff;
	GLuint framebuffer;
	Texture* texture_col;
	Texture* texture_depth;
	Texture* texture_screen;
	GLuint program;
	double color;
	HWND hwnd;
	HDC hdc;
	HGLRC hglrc;
	Camera* camera;
};

#endif