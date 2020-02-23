#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lodepng/lodepng.hpp"

#include "rr3d_font.hpp"

#include <windows.h>

char* concat(char* s1, char* s2) {
	unsigned str_size = strlen(s1) + strlen(s2) + 1;
	char* result = (char*)malloc(str_size);
	int i = 0;
	for (i = 0; i < strlen(s1); ++i) {
		result[i] = s1[i];
	}
	for (int j = 0; j < strlen(s2); ++j) {
		result[i + j] = s2[j];
	}
	result[str_size - 1] = 0;
	return result;
}

Font::Font(char* fn) {
	
	char* all_chars = (char*)malloc(62);
	all_chars[0] = 'A'; all_chars[1] = 'B'; all_chars[2] = 'C'; all_chars[3] = 'D'; all_chars[4] = 'E'; all_chars[5] = 'F';
	all_chars[6] = 'G'; all_chars[7] = 'H'; all_chars[8] = 'I'; all_chars[9] = 'J'; all_chars[10] = 'K'; all_chars[11] = 'L';
	all_chars[12] = 'M'; all_chars[13] = 'N'; all_chars[14] = 'O'; all_chars[15] = 'P'; all_chars[16] = 'Q'; all_chars[17] = 'R';
	all_chars[18] = 'S'; all_chars[19] = 'T'; all_chars[20] = 'U'; all_chars[21] = 'V'; all_chars[22] = 'W'; all_chars[23] = 'X';
	all_chars[24] = 'Y'; all_chars[25] = 'Z'; all_chars[26] = 'a'; all_chars[27] = 'b'; all_chars[28] = 'c'; all_chars[29] = 'd';
	all_chars[30] = 'e'; all_chars[31] = 'f'; all_chars[32] = 'g'; all_chars[33] = 'h'; all_chars[34] = 'i'; all_chars[35] = 'j';
	all_chars[36] = 'k'; all_chars[37] = 'l'; all_chars[38] = 'm'; all_chars[39] = 'n'; all_chars[40] = 'o'; all_chars[41] = 'p';
	all_chars[42] = 'q'; all_chars[43] = 'r'; all_chars[44] = 's'; all_chars[45] = 't'; all_chars[46] = 'u'; all_chars[47] = 'v';
	all_chars[48] = 'w'; all_chars[49] = 'x'; all_chars[50] = 'y'; all_chars[51] = 'z'; all_chars[52] = '0'; all_chars[53] = '1';
	all_chars[54] = '2'; all_chars[55] = '3'; all_chars[56] = '4'; all_chars[57] = '5'; all_chars[58] = '6'; all_chars[59] = '7';
	all_chars[60] = '8'; all_chars[61] = '9';

	chars = (unsigned char**)malloc(256 * 4);
	memset(chars, 0, 256 * 4);

	for (int i = 0; i < 62; ++i) {

		char* fn2 = (char*)malloc(7);
		fn2[0] = '/'; fn2[1] = all_chars[i]; fn2[2] = '.';
		fn2[3] = 'p'; fn2[4] = 'n'; fn2[5] = 'g'; fn2[6] = 0;

		char* str = concat(fn, fn2);

		unsigned width, height;
		unsigned error = lodepng_decode32_file(&chars[all_chars[i]], &width, &height, str);

		if (!error) {
			char_width = width;
			char_height = height;
			chars_cnt++;
		}

		free(str);
	}

	free(all_chars);
}

Font::~Font() { 
	for (int i = 0; i < 256; ++i) {
		if (chars[i] != NULL) {
			free(chars[i]);
		}
	}
}

unsigned char** Font::getChars() {
	return chars;
}

unsigned Font::getCharXStep() {
	return char_xstep;
}

unsigned Font::getCharYStep() {
	return char_ystep;
}

unsigned Font::getCharWidth() {
	return char_width;
}

unsigned Font::getCharHeight() {
	return char_height;
}