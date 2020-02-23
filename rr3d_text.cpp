#include <glew.h>

#include "rr3d_text.hpp"

#include <windows.h>

void TextDraw(Texture* surface, Text* text, unsigned x, unsigned y) {

	glBindTexture(GL_TEXTURE_2D, surface->getTexture());

	unsigned x_start = x;
	unsigned y_start = y;

	for (int i = 0; i < text->str_len; ++i) {
		unsigned char c = text->str[i];
		if (c != '#') {
			if (text->font->getChars()[c] != 0) {
				unsigned cw = text->font->getCharWidth();
				unsigned ch = text->font->getCharHeight();
				unsigned char* cptr = text->font->getChars()[c];
				for (int yy = 0; yy < ch; ++yy) {
					for (int xx = 0; xx < cw; ++xx) {
						unsigned cindex = ((yy * cw) + xx) * 4;
						if (cptr[cindex + 3] != 0) {
							glTexSubImage2D(GL_TEXTURE_2D, 0, x + xx, y + yy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, cptr + cindex);
						}
					}
				}
			}
			x += text->font->getCharWidth() + text->str_xsize;
		} else {
			y -= text->font->getCharHeight() + text->str_ysize;
			x = x_start;
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}