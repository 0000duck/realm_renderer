#ifndef __RR3D_FONT__
#define __RR3D_FONT__

#include "rr3d_texture.hpp"

class Font {
public:
	Font(char*);
	~Font();
	unsigned char** getChars();
	unsigned getCharXStep();
	unsigned getCharYStep();
	unsigned getCharWidth();
	unsigned getCharHeight();
private:
	unsigned char_xstep;
	unsigned char_ystep;
	unsigned char_width;
	unsigned char_height;
	unsigned chars_cnt;
	unsigned char** chars;
};

#endif