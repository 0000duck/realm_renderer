#ifndef __RR3D_TEXT__
#define __RR3D_TEXT__

#include "rr3d_font.hpp"
#include "rr3d_texture.hpp"

typedef struct Text_str {
	unsigned hidden;
	Font* font;
	unsigned str_len;
	char* str;
	short str_xsize;
	short str_ysize;
} Text;

void TextDraw(Texture*, Text*, unsigned, unsigned);

#endif