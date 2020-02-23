#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "rr3d_model_loader.hpp"

#define MAX_SIZE 30
int scanfloat(unsigned char* buffer, float* f1, float* f2) {
	unsigned char* ptr = buffer;
	char f[MAX_SIZE], fsz = 0; f[MAX_SIZE - 1] = 0;
	memset(f, 0, MAX_SIZE);
	// first float
	while (1) {
		if (*ptr == 0x20 || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		f[fsz++] = *ptr++;
	}
	*f1 = atof(f);
	// second float
	fsz = 0;
	memset(f, 0, MAX_SIZE);
	while (1) {
		if (*ptr == 0x20 || *ptr == 0xa || *ptr == 0xd) {
			break;
		}
		f[fsz++] = *ptr++;
	}
	*f2 = atof(f);
	return ptr - buffer;
}

int scanfloat(unsigned char* buffer, float* f1, float* f2, float* f3) {
	unsigned char* ptr = buffer;
	char f[MAX_SIZE], fsz = 0; f[MAX_SIZE - 1] = 0;
	memset(f, 0, MAX_SIZE);
	// first float
	while (1) {
		if (*ptr == 0x20 || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		f[fsz++] = *ptr++;
	}
	*f1 = atof(f);
	// second float
	fsz = 0;
	memset(f, 0, MAX_SIZE);
	while (1) {
		if (*ptr == 0x20 || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		f[fsz++] = *ptr++;
	}
	*f2 = atof(f);
	// third float
	fsz = 0;
	memset(f, 0, MAX_SIZE);
	while (1) {
		if (*ptr == 0x20 || *ptr == 0xa || *ptr == 0xd) {
			break;
		}
		f[fsz++] = *ptr++;
	}
	*f3 = atof(f);
	return ptr - buffer;
}

int ccount(unsigned char* buffer, char character) {
	int count = 0;
	unsigned char* ptr = buffer;
	while (1) {
		if (*buffer == 0xa || *buffer == 0xd) {
			break;
		}
		if (*buffer++ == character && *buffer != 0xa && *buffer != 0xd) {
			count++;
		}
	}
	return count;
}

int scanface(unsigned char* buffer, int* i1, int* t1, int* n1, int* i2, int* t2, int* n2, int* i3, int* t3, int* n3) {
	unsigned char* ptr = buffer;
	char i[MAX_SIZE], isz = 0; i[MAX_SIZE - 1] = 0;
	memset(i, 0, MAX_SIZE);
	// first group
	while (1) {
		if (*ptr == '/' || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		i[isz++] = *ptr++;
	}
	*i1 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);
	while (1) {
		if (*ptr == '/' || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		i[isz++] = *ptr++;
	}
	*t1 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);
	while (1) {
		if (*ptr == '/' || *ptr == 0x20 || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		i[isz++] = *ptr++;
	}
	*n1 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);
	// second group
	while (1) {
		if (*ptr == '/' || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		i[isz++] = *ptr++;
	}
	*i2 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);
	while (1) {
		if (*ptr == '/' || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		i[isz++] = *ptr++;
	}
	*t2 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);
	while (1) {
		if (*ptr == '/' || *ptr == 0x20 || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		i[isz++] = *ptr++;
	}
	*n2 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);
	// third group
	while (1) {
		if (*ptr == '/' || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		i[isz++] = *ptr++;
	}
	*i3 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);
	while (1) {
		if (*ptr == '/' || *ptr == 0xa || *ptr == 0xd) {
			*ptr++;
			break;
		}
		i[isz++] = *ptr++;
	}
	*t3 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);
	while (1) {
		if (*ptr == '/' || *ptr == 0x20 || *ptr == 0xa || *ptr == 0xd) {
			break;
		}
		i[isz++] = *ptr++;
	}
	*n3 = atoi(i);
	isz = 0;
	memset(i, 0, MAX_SIZE);

	return ptr - buffer;
}

unsigned int RRender_ML_Load(char* fn, RR3DVertex** vertices) {

	if (vertices != NULL) {
		free(vertices);
	}

	FILE* model = fopen(fn, "rb");
	if (model == NULL) {
		return 0;
	}

	fseek(model, 0, SEEK_END);
	int file_size = ftell(model);
	fseek(model, 0, SEEK_SET);

	unsigned char* buffer = (unsigned char*)malloc(file_size);
	fread(buffer, 1, file_size, model);
	fclose(model);

	RR3DVertex3* temp_positions = (RR3DVertex3*)malloc(sizeof(RR3DVertex3) * 1000000);
	RR3DVertexUV* temp_uvs = (RR3DVertexUV*)malloc(sizeof(RR3DVertexUV) * 1000000);
	RR3DVertex3* temp_normals = (RR3DVertex3*)malloc(sizeof(RR3DVertex3) * 1000000);

	*vertices = (RR3DVertex*)malloc(sizeof(RR3DVertex) * 1000000);

	unsigned int vertices_cnt, vertex_cnt, uv_cnt, normal_cnt;
	vertices_cnt = vertex_cnt = uv_cnt = normal_cnt = 0;

	for (int i = 0; i < file_size; ++i) {
		// mtllib
		if (buffer[i] == 'm'	&& buffer[i + 1] == 't' && buffer[i + 2] == 'l' &&
			buffer[i + 3] == 'l'&& buffer[i + 4] == 'i'	&& buffer[i + 5] == 'b') {
			while (1) {
				// prevent overflow
				if (i >= file_size || buffer[i] == 0xa || buffer[i] == 0xd) { i++; break; }
				++i;
			}
		}
		// usemtl
		if (buffer[i] == 'u'	&& buffer[i + 1] == 's' && buffer[i + 2] == 'e' &&
			buffer[i + 3] == 'm'&& buffer[i + 4] == 't'	&& buffer[i + 5] == 'l') {
			while (1) {
				// prevent overflow
				if (i >= file_size || buffer[i] == 0xa || buffer[i] == 0xd) { i++; break; }
				++i;
			}
		}
		// Comment
		if (buffer[i] == '#') {
			while (1) {
				// prevent overflow
				if (i >= file_size || buffer[i] == 0xa || buffer[i] == 0xd) { i++; break; }
				++i;
			}
		}
		// Object name
		if (buffer[i] == 'o') {
			while (1) {
				// prevent overflow
				if (i >= file_size || buffer[i] == 0xa || buffer[i] == 0xd) { i++; break; }
				++i;
			}
		}
		// Group
		if (buffer[i] == 'g') {
			while (1) {
				// prevent overflow
				if (i >= file_size || buffer[i] == 0xa || buffer[i] == 0xd) { i++; break; }
				++i;
			}
		}
		// Smooth
		if (buffer[i] == 's') {
			while (1) {
				// prevent overflow
				if (i >= file_size || buffer[i] == 0xa || buffer[i] == 0xd) { i++; break; }
				++i;
			}
		}
		// Vertex
		if (buffer[i] == 'v') {
			++i;
			if (buffer[i] == ' ') {
				++i;
				while (buffer[i] == ' ') { ++i; }
				RR3DVertex3 vertex;
				i += scanfloat(buffer + i, &vertex.pos.x, &vertex.pos.y, &vertex.pos.z);
				temp_positions[vertex_cnt++] = vertex;
			}
			if (buffer[i] == 't') {
				++i;
				while (buffer[i] == ' ') { ++i; }
				RR3DVertexUV uv;
				i += scanfloat(buffer + i, &uv.uv.x, &uv.uv.y);
				temp_uvs[uv_cnt++] = uv;
			}
			if (buffer[i] == 'n') {
				++i;
				while (buffer[i] == ' ') { ++i; }
				RR3DVertex3 n;
				i += scanfloat(buffer + i, &n.pos.x, &n.pos.y, &n.pos.z);
				temp_normals[normal_cnt++] = n;
			}
		}
		// Face
		if (buffer[i] == 'f') {
			++i;
			while (buffer[i] == ' ') { ++i; }
			int count = ccount(buffer + i, ' ') + 1;
			if (count == 3) {
				int i1, t1, n1, i2, t2, n2, i3, t3, n3;
				i += scanface(buffer + i, &i1, &t1, &n1, &i2, &t2, &n2, &i3, &t3, &n3);
				i1--, i2--, i3--, t1--, t2--, t3--, n1--, n2--, n3--;
				(*vertices)[vertices_cnt].pos = temp_positions[i1].pos;
				(*vertices)[vertices_cnt].uv = temp_uvs[t1].uv;
				(*vertices)[vertices_cnt].uv.y = 1 - (*vertices)[vertices_cnt].uv.y;
				(*vertices)[vertices_cnt].normal = temp_normals[n1].pos;
				(*vertices)[vertices_cnt].color.r = 255;
				(*vertices)[vertices_cnt].color.g = 255;
				(*vertices)[vertices_cnt].color.b = 255;
				vertices_cnt++;
				(*vertices)[vertices_cnt].pos = temp_positions[i2].pos;
				(*vertices)[vertices_cnt].uv = temp_uvs[t2].uv;
				(*vertices)[vertices_cnt].uv.y = 1 - (*vertices)[vertices_cnt].uv.y;
				(*vertices)[vertices_cnt].normal = temp_normals[n2].pos;
				(*vertices)[vertices_cnt].color.r = 255;
				(*vertices)[vertices_cnt].color.g = 255;
				(*vertices)[vertices_cnt].color.b = 255;
				vertices_cnt++;
				(*vertices)[vertices_cnt].pos = temp_positions[i3].pos;
				(*vertices)[vertices_cnt].uv = temp_uvs[t3].uv;
				(*vertices)[vertices_cnt].uv.y = 1 - (*vertices)[vertices_cnt].uv.y;
				(*vertices)[vertices_cnt].normal = temp_normals[n3].pos;
				(*vertices)[vertices_cnt].color.r = 255;
				(*vertices)[vertices_cnt].color.g = 255;
				(*vertices)[vertices_cnt].color.b = 255;
				vertices_cnt++;
			} else {
				while (1) {
					// prevent overflow
					if (i >= file_size || buffer[i] == 0xa || buffer[i] == 0xd) { i++; break; }
					++i;
				}
			}
		}
	}

	*vertices = (RR3DVertex*)realloc(*vertices, sizeof(RR3DVertex) * vertices_cnt);

	free(buffer);
	free(temp_positions);
	free(temp_uvs);
	free(temp_normals);

	return vertices_cnt;
}