#define _CRT_SECURE_NO_WARNINGS

#include <glew.h>

#include <stdio.h>
#include <windows.h>

#include "lodepng/lodepng.hpp"

#include "rr3d.hpp"
#include "rr3d_camera.hpp"
#include "rr3d_model.hpp"
#include "rr3d_model_loader.hpp"
#include "rr3d_primitive.hpp"
#include "rr3d_terrain.hpp"
#include "rr3d_billboard.hpp"
#include "rr3d_text.hpp"
#include "rr3d_font.hpp"
#include "rr3d_texture.hpp"
#include "rr3d_vertex.hpp"
#include "rr3d_window.hpp"

void** RR3D_instance_buffer = NULL;
char* RR3D_instance_buffer_id = NULL;
unsigned int RR3D_instance_buffer_cnt = 0;

void** RR3D_screen_buffer = NULL;
char* RR3D_screen_buffer_id = NULL;
unsigned* RR3D_screen_buffer_pos = NULL;
unsigned int RR3D_screen_buffer_cnt = 0;

//* NOT GM FUNCTIONS *//

unsigned clamp(signed v, unsigned min, unsigned max) {

	if (v <= min) { return min; }
	if (v >= max) { return max; }

	return v;
}

glm::vec3 calculate_normal(signed int* ybuffer, unsigned gc, unsigned gs, unsigned x, unsigned z) {

	unsigned v;
	float hL, hR, hD, hU;
	v = clamp(x - 1, 0, gc - 1);
	hL = ybuffer[(z * (gc + 1)) + v];
	v = clamp(x + 1, 0, gc - 1);
	hR = ybuffer[(z * (gc + 1)) + v];
	v = clamp(z - 1, 0, gc - 1);
	hD = ybuffer[(v * (gc + 1)) + x];
	v = clamp(z + 1, 0, gc - 1);
	hU = ybuffer[(v * (gc + 1)) + x];

	return glm::normalize(glm::vec3(hL - hR, gs << 1, hD - hU));
}

glm::vec3 calculate_normalNYB(GLfloat* positions, unsigned gc, unsigned gs, unsigned x, unsigned z) {

	unsigned v, zv, p;
	float hL, hR, hD, hU;
	v = clamp(x - 1, 0, gc - 1);
	p = ((z * ((gc + 1) * 6)) + ((v * 6) + 3)) + 1;
	hL = positions[p];
	v = clamp(x + 1, 0, gc - 1);
	p = ((z * ((gc + 1) * 6)) + ((v * 6) + 3)) + 1;
	hR = positions[p];
	v = clamp(z - 1, 0, gc - 1);
	p = ((v * ((gc + 1) * 6)) + ((x * 6) + 3)) + 1;
	hD = positions[p];
	v = clamp(z + 1, 0, gc - 1);
	p = ((v * ((gc + 1) * 6)) + ((x * 6) + 3)) + 1;
	hU = positions[p];

	return glm::normalize(glm::vec3(hL - hR, gs << 1, hD - hU));
}

//* OTHER *//

RRENDER_API char* RRender_FileRead(char* path) {

	FILE* file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	unsigned int fsz = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* str = (char*)malloc(fsz + 1);
	memset(str, 0, fsz);
	for (unsigned int i = 0; i < fsz; ++i) {
		str[i] = fgetc(file);
	}
	str[fsz] = 0;
	fclose(file);

	return str;
}

RRENDER_API char* RRender_StringCreate(double size) {

	char* str = (char*)malloc((unsigned)size);
	memset(str, 0, (unsigned)size);

	return str;
}

RRENDER_API double RRender_StringDelete(char* str) {

	free(str);

	return RR_SUCCESS;
}

//* VECTOR *//

RRENDER_API double RRender_VectorCreate(double x, double y, double z) {

	RR3DVertex3* vec = (RR3DVertex3*)malloc(sizeof(RR3DVertex));
	vec->pos.x = (GLfloat)x;
	vec->pos.y = (GLfloat)y;
	vec->pos.z = (GLfloat)z;

	return (double)(ULONG)vec;
}

RRENDER_API double RRender_VectorSet(double p, double x, double y, double z) {

	RR3DVertex3* vec = (RR3DVertex3*)(ULONG)p;
	vec->pos.x = (GLfloat)x;
	vec->pos.y = (GLfloat)y;
	vec->pos.z = (GLfloat)z;

	return RR_SUCCESS;
}

RRENDER_API double RRender_VectorGet(double p, double v) {

	RR3DVertex3* vec = (RR3DVertex3*)(ULONG)p;
	char val = (char)v;
	switch (val) {
	case 0:
		return vec->pos.x;
		break;
	case 1:
		return vec->pos.y;
		break;
	case 2:
		return vec->pos.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_VectorDot(double p1, double p2) {
	
	RR3DVertex3* v1 = (RR3DVertex3*)(ULONG)p1;
	RR3DVertex3* v2 = (RR3DVertex3*)(ULONG)p2;
	
	return glm::dot(v1->pos, v2->pos);
}

RRENDER_API double RRender_VectorCross(double p1, double p2) {

	RR3DVertex3* v1 = (RR3DVertex3*)(ULONG)p1;
	RR3DVertex3* v2 = (RR3DVertex3*)(ULONG)p2;
	RR3DVertex3* v3 = (RR3DVertex3*)malloc(sizeof(RR3DVertex));

	v3->pos = glm::cross(v1->pos, v2->pos);

	return (double)(ULONG)v3;
}

RRENDER_API double RRender_VectorDelete(double p) {

	RR3DVertex3* v1 = (RR3DVertex3*)(ULONG)p;
	free(v1);

	return RR_SUCCESS;
}

//* CAMERA *//

RRENDER_API double RRender_CameraCreate(double fov, double aspect, double znear, double zfar) {

	Camera* camera = new Camera((float)fov, (float)aspect, (float)znear, (float)zfar);

	return (double)(ULONG)camera;
}

RRENDER_API double RRender_CameraSetPitch(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->setPitch((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraSetYaw(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->setYaw((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraSetRoll(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->setRoll((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraAddPitch(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->addPitch((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraAddYaw(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->addYaw((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraAddRoll(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->addRoll((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraSetPosition(double cam, double x, double y, double z) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->setTranslation((float)x, (float)y, (float)z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraGetPitch(double cam) {

	Camera* camera = (Camera*)(ULONG)cam;

	return (double)camera->getPitch();
}

RRENDER_API double RRender_CameraGetYaw(double cam) {

	Camera* camera = (Camera*)(ULONG)cam;

	return (double)camera->getYaw();
}

RRENDER_API double RRender_CameraGetRoll(double cam) {

	Camera* camera = (Camera*)(ULONG)cam;

	return (double)camera->getRoll();
}

RRENDER_API double RRender_CameraGetPosition(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	char val = (char)v;
	switch (val) {
	case 0:
		return (double)*(camera->getTranslationX());
		break;
	case 1:
		return (double)*(camera->getTranslationY());
		break;
	case 2:
		return (double)*(camera->getTranslationZ());
		break;
	}
	
	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraMove(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->move((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraStrafe(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->strafe((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraLift(double cam, double v) {

	Camera* camera = (Camera*)(ULONG)cam;
	camera->lift((float)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_CameraDelete(double cam) {

	Camera* camera = (Camera*)(ULONG)cam;
	delete camera;

	return RR_SUCCESS;
}

//* SCREEN BUFFER *//

RRENDER_API double RRender_ScreenBufferCreate() {

	if (RR3D_screen_buffer != NULL) {
		free(RR3D_screen_buffer);
		free(RR3D_screen_buffer_pos);
		RR3D_screen_buffer_cnt = 0;
	}
	RR3D_screen_buffer = (void**)malloc(4);
	RR3D_screen_buffer_id = (char*)malloc(1);
	RR3D_screen_buffer_pos = (unsigned*)malloc(4);
	RR3D_screen_buffer_cnt = 0;

	return RR_SUCCESS;
}

RRENDER_API double RRender_ScreenBufferAdd(double ptr, double id, double x, double y) {

	RR3D_screen_buffer = (void**)realloc(RR3D_screen_buffer, (RR3D_screen_buffer_cnt + 1) * 4);
	RR3D_screen_buffer_id = (char*)realloc(RR3D_screen_buffer_id, RR3D_screen_buffer_cnt + 1);
	RR3D_screen_buffer_pos = (unsigned*)realloc(RR3D_screen_buffer_pos, (RR3D_screen_buffer_cnt + 1) * 4);

	RR3D_screen_buffer[RR3D_screen_buffer_cnt] = (void*)(ULONG)ptr;
	RR3D_screen_buffer_id[RR3D_screen_buffer_cnt] = (char)id;
	RR3D_screen_buffer_pos[RR3D_screen_buffer_cnt] = (unsigned)(((short)x << 16) | (short)y);

	RR3D_screen_buffer_cnt++;

	return RR_SUCCESS;
}

RRENDER_API double RRender_ScreenBufferRemove(double ptr) {

	if (RR3D_screen_buffer_cnt == 0) {
		return 0;
	}

	unsigned int i = 0;
	void* p = (void*)(ULONG)ptr;
	for (i = 0; i < RR3D_screen_buffer_cnt; ++i) {
		if (RR3D_screen_buffer[i] == p) {
			break;
		}
	}

	// if not last
	if (i != RR3D_screen_buffer_cnt - 1) {
		for (; i < RR3D_screen_buffer_cnt - 1; ++i) {
			RR3D_screen_buffer[i] = RR3D_screen_buffer[i + 1];
			RR3D_screen_buffer_id[i] = RR3D_screen_buffer_id[i + 1];
			RR3D_screen_buffer_pos[i] = RR3D_screen_buffer_pos[i + 1];
		}
	}

	RR3D_screen_buffer_cnt -= 1;

	if (RR3D_screen_buffer_cnt > 0) {
		RR3D_screen_buffer = (void**)realloc(RR3D_screen_buffer, RR3D_screen_buffer_cnt * 4);
		RR3D_screen_buffer_id = (char*)realloc(RR3D_screen_buffer_id, RR3D_screen_buffer_cnt);
		RR3D_screen_buffer_pos = (unsigned*)realloc(RR3D_screen_buffer_pos, RR3D_screen_buffer_cnt * 4);
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_ScreenBufferGetCount() {

	return (double)RR3D_screen_buffer_cnt;
}

RRENDER_API double RRender_ScreenBufferSet(double ptr, double x, double y) {

	if (RR3D_screen_buffer_cnt == 0) {
		return 0;
	}

	unsigned int i = 0;
	void* p = (void*)(ULONG)ptr;
	for (i = 0; i < RR3D_screen_buffer_cnt; ++i) {
		if (RR3D_screen_buffer[i] == p) {
			break;
		}
	}

	RR3D_screen_buffer_pos[i] = ((short)x << 16) | (short)y;

	return RR_SUCCESS;
}

//* INSTANCE BUFFER *//

RRENDER_API double RRender_InstanceBufferCreate() {

	if (RR3D_instance_buffer != NULL) {
		free(RR3D_instance_buffer);
		free(RR3D_instance_buffer_id);
		RR3D_instance_buffer_cnt = 0;
	}
	RR3D_instance_buffer = (void**)malloc(4);
	RR3D_instance_buffer_id = (char*)malloc(1);
	RR3D_instance_buffer_cnt = 0;

	return RR_SUCCESS;
}

RRENDER_API double RRender_InstanceBufferAdd(double ptr, double id) {

	RR3D_instance_buffer = (void**)realloc(RR3D_instance_buffer, (RR3D_instance_buffer_cnt + 1) * 4);
	RR3D_instance_buffer_id = (char*)realloc(RR3D_instance_buffer_id, RR3D_instance_buffer_cnt + 1);

	RR3D_instance_buffer[RR3D_instance_buffer_cnt] = (void*)(ULONG)ptr;
	RR3D_instance_buffer_id[RR3D_instance_buffer_cnt] = (char)id;

	RR3D_instance_buffer_cnt++;

	return RR_SUCCESS;
}

RRENDER_API double RRender_InstanceBufferRemove(double ptr) {

	if (RR3D_instance_buffer_cnt == 0) {
		return 0;
	}

	unsigned int i = 0;
	void* p = (void*)(ULONG)ptr;
	for (i = 0; i < RR3D_instance_buffer_cnt; ++i) {
		if (RR3D_instance_buffer[i] == p) {
			break;
		}
	}

	// if not last
	if (i != RR3D_instance_buffer_cnt - 1) {
		for (; i < RR3D_instance_buffer_cnt - 1; ++i) {
			RR3D_instance_buffer[i] = RR3D_instance_buffer[i + 1];
			RR3D_instance_buffer_id[i] = RR3D_instance_buffer_id[i + 1];
		}
	}
	
	RR3D_instance_buffer_cnt -= 1;
	
	if (RR3D_instance_buffer_cnt > 0) {
		RR3D_instance_buffer = (void**)realloc(RR3D_instance_buffer, RR3D_instance_buffer_cnt * 4);
		RR3D_instance_buffer_id = (char*)realloc(RR3D_instance_buffer_id, RR3D_instance_buffer_cnt);
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_InstanceBufferGetCount() {

	return (double)RR3D_instance_buffer_cnt;
}

//* FONT *//

RRENDER_API double RRender_FontCreate(char* fn) {
	
	Font* font = new Font(fn);

	return (double)(ULONG)font;
}

RRENDER_API double RRender_FontDelete(double d) {

	Font* font = (Font*)(ULONG)d;
	delete font;

	return RR_SUCCESS;
}

//* TEXT *//

RRENDER_API double RRender_TextCreate(char* str, double xsize, double ysize, double font) {

	Text* text = (Text*)malloc(sizeof(Text));
	text->str = str;
	text->str_len = strlen(text->str);
	text->str_xsize = (short)xsize;
	text->str_ysize = (short)ysize;
	text->font = (Font*)(ULONG)font;
	text->hidden = 0;

	return (double)(ULONG)text;
}

RRENDER_API double RRender_TextHide(double d, double v) {

	Text* text = (Text*)(ULONG)d;
	text->hidden = (unsigned)v;

	return RR_SUCCESS;
}

RRENDER_API double RRender_TextDelete(double d) {

	Text* text = (Text*)(ULONG)d;
	free(text->str);
	free(text);

	return RR_SUCCESS;
}

//* TEXTURE *//

RRENDER_API double RRender_TextureCreate(char* fn, double p, double m) {

	Texture* texture = new Texture(fn, m, p);

	return (double)(ULONG)texture;
}

RRENDER_API double RRender_TextureHide(double d, double v) {

	Texture* texture = (Texture*)(ULONG)d;
	texture->setHidden((unsigned)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_TextureSetRegion(double t1, double t2, double x, double y) {

	Texture* data1 = (Texture*)(ULONG)t1;
	Texture* data2 = (Texture*)(ULONG)t2;

	if (data2->getPixels() == NULL) { return 0; }

	// writing on texture
	glBindTexture(GL_TEXTURE_2D, data1->getTexture());
	glTexSubImage2D(GL_TEXTURE_2D, 0, (GLint)x, (GLint)y, data2->getWidth(), data2->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, data2->getPixels());
	glBindTexture(GL_TEXTURE_2D, 0);

	if (data1->getPixels() == NULL) { return 0; }

	// writing on pixels
	unsigned ptr1, ptr2;
	ptr1 = ptr2 = 0;
	for (int y1 = 0, y2 = (int)y; y1 < data2->getHeight(); ++y1, ++y2) {
		for (int x1 = 0, x2 = (int)x; x1 < data2->getWidth(); ++x1, ++x2) {
			ptr1 = (data1->getWidth() * y2 * 4) + (x2 * 4);
			ptr2 = (data2->getWidth() * y1 * 4) + (x1 * 4);
			data1->getPixels()[ptr1] = data2->getPixels()[ptr2];
			data1->getPixels()[ptr1 + 1] = data2->getPixels()[ptr2 + 1];
			data1->getPixels()[ptr1 + 2] = data2->getPixels()[ptr2 + 2];
			data1->getPixels()[ptr1 + 3] = data2->getPixels()[ptr2 + 3];
		}
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_TextureFreeMemory(double t) {

	Texture* data = (Texture*)(ULONG)t;
	if (data->getPixels() != NULL) {
		free(data->getPixels());
		data->setPixels();
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_TextureSave(double t, char* fn) {

	Texture* data = (Texture*)(ULONG)t;
	unsigned error = lodepng_encode32_file(fn, data->getPixels(), data->getWidth(), data->getHeight());
	if (error) {
		MessageBox(NULL, "Error saving PNG texture!", "Error", MB_ICONERROR);
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_TextureDelete(double t) {

	Texture* data = (Texture*)(ULONG)t;
	delete data;

	return RR_SUCCESS;
}

//* BILLBOARD DATA *//

RRENDER_API double RRender_BillboardDataCreate() {

	BillboardData* data = (BillboardData*)malloc(sizeof(BillboardData));
	data->program = 0;
	data->texture = 0;

	return (double)(ULONG)data;
}

RRENDER_API double RRender_BillboardDataSetTexture(double d, double t) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	Texture* texture = (Texture*)(ULONG)t;
	data->texture = texture;

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDataSetPosition(double d, double x, double y, double z) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	data->translation = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDataSetRotation(double d, double x, double y, double z) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	data->rotation = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDataSetScale(double d, double x, double y, double z) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	data->scale = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDataSetShader(double d, double shader) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	data->program = (GLuint)shader;

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDataGetPosition(double d, double v) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->translation.x;
		break;
	case 1:
		return data->translation.y;
		break;
	case 2:
		return data->translation.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDataGetRotation(double d, double v) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->rotation.x;
		break;
	case 1:
		return data->rotation.y;
		break;
	case 2:
		return data->rotation.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDataGetScale(double d, double v) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->scale.x;
		break;
	case 1:
		return data->scale.y;
		break;
	case 2:
		return data->scale.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDataGetShader(double d) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	return data->program;
}

RRENDER_API double RRender_BillboardDataDelete(double d) {

	BillboardData* data = (BillboardData*)(ULONG)d;
	free(data);

	return RR_SUCCESS;
}

//* TERRAIN DATA *//

RRENDER_API double RRender_TerrainDataCreate() {

	TerrainData* data = (TerrainData*)malloc(sizeof(TerrainData));
	data->positions = 0;
	data->normals = 0;
	data->uvs = 0;
	data->program = 0;
	data->vertices_cnt = 0;
	data->texture = 0;
	data->gc = 0;
	data->gs = 0;

	glGenBuffers(1, &data->buff_positions);
	glGenBuffers(1, &data->buff_normals);
	glGenBuffers(1, &data->buff_uvs);

	return (double)(ULONG)data;
}

RRENDER_API double RRender_TerrainDataSetTexture(double d, double t) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	Texture* texture = (Texture*)(ULONG)t;
	data->texture = texture;

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDataSetPosition(double d, double x, double y, double z) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	data->translation = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDataSetShader(double d, double shader) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	data->program = (GLuint)shader;

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDataSetParams(double d, double gs, double gc) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	data->gc = (unsigned int)gc;
	data->gs = (unsigned int)gs;

	return RR_SUCCESS;

}

RRENDER_API double RRender_TerrainDataAddY(double d, double x, double y, double v) {

	TerrainData* data = (TerrainData*)(ULONG)d;

	if (data->positions == NULL || data->normals == NULL) { return 0; }

	unsigned gx = x / data->gs;
	unsigned gy = y / data->gs;

	if (gy == 0) {
		unsigned ptr = (gx * 24) + 12;
		unsigned bptr = (gx * 6) + 3;
		glm::vec3 normal = calculate_normalNYB(data->positions, data->gc, data->gs, gx, gy);
		data->positions[bptr + 1] += (GLfloat)v;
		data->normals[bptr] = normal.x;
		data->normals[bptr + 1] = normal.y;
		data->normals[bptr + 2] = normal.z;
		// update buffers
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_positions);
		glBufferSubData(GL_ARRAY_BUFFER, ptr + 4, 4, ((char*)data->positions + ptr + 4));
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_normals);
		glBufferSubData(GL_ARRAY_BUFFER, ptr + 4, 4, ((char*)data->normals + ptr + 4));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
		unsigned ptr = ((data->gc + 1) * (gy - 1) * 24) + (gx * 24);
		unsigned bptr = ((data->gc + 1) * (gy - 1) * 6) + (gx * 6);
		glm::vec3 normal = calculate_normalNYB(data->positions, data->gc, data->gs, gx, gy);
		data->positions[bptr + 1] += (GLfloat)v;
		data->normals[bptr] = normal.x;
		data->normals[bptr + 1] = normal.y;
		data->normals[bptr + 2] = normal.z;
		// update buffers
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_positions);
		glBufferSubData(GL_ARRAY_BUFFER, ptr + 4, 4, ((char*)data->positions + ptr + 4));
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_normals);
		glBufferSubData(GL_ARRAY_BUFFER, ptr + 4, 4, ((char*)data->normals + ptr + 4));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (gy != data->gc) {
			ptr = (((data->gc + 1) * (gy) * 24) + 12) + (gx * 24);
			bptr = ((data->gc + 1) * (gy) * 6) + ((gx * 6) + 3);
			glm::vec3 normal = calculate_normalNYB(data->positions, data->gc, data->gs, gx, gy);
			data->positions[bptr + 1] += (GLfloat)v;
			data->normals[bptr] = normal.x;
			data->normals[bptr + 1] = normal.y;
			data->normals[bptr + 2] = normal.z;
			// update buffers
			glBindBuffer(GL_ARRAY_BUFFER, data->buff_positions);
			glBufferSubData(GL_ARRAY_BUFFER, ptr + 4, 4, ((char*)data->positions + ptr + 4));
			glBindBuffer(GL_ARRAY_BUFFER, data->buff_normals);
			glBufferSubData(GL_ARRAY_BUFFER, ptr + 4, 4, ((char*)data->normals + ptr + 4));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDataGetPosition(double d, double v) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->translation.x;
		break;
	case 1:
		return data->translation.y;
		break;
	case 2:
		return data->translation.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDataGetShader(double d) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	return data->program;
}

RRENDER_API double RRender_TerrainDataGenShape(double d, double m) {

	TerrainData* data = (TerrainData*)(ULONG)d;

	if (data->gs == 0 || data->gc == 0) { return 0; }

	if (data->positions != NULL) { free(data->positions); }
	if (data->normals != NULL) { free(data->normals); }
	if (data->uvs != NULL) { free(data->uvs); }

	// construct terrain
	data->vertices_cnt = ((data->gc + 1) * data->gc) * 2;
	data->positions = (GLfloat*)malloc((data->gc + 1) * data->gc * 24);
	data->normals = (GLfloat*)malloc((data->gc + 1) * data->gc * 24);
	data->uvs = (GLfloat*)malloc((data->gc + 1) * data->gc * 16);

	unsigned int ptr = 0;
	for (int zz = 0; zz < data->gc; ++zz) {
		for (int xx = 0; xx < data->gc + 1; ++xx) {
			unsigned pos;
			pos = ((zz + 1) * (data->gc + 1)) + xx;
			data->positions[ptr] = xx * data->gs;
			data->positions[ptr + 1] = 0;
			data->positions[ptr + 2] = (zz + 1) * data->gs;
			ptr += 3;
			pos = (zz * (data->gc + 1)) + xx;
			data->positions[ptr] = xx * data->gs;
			data->positions[ptr + 1] = 0;
			data->positions[ptr + 2] = zz * data->gs;
			ptr += 3;
		}
	}

	ptr = 0;
	for (int zz = 0; zz < data->gc; ++zz) {
		for (int xx = 0; xx < data->gc + 1; ++xx) {
			glm::vec3 normal;
			data->normals[ptr] = 0;
			data->normals[ptr + 1] = 1;
			data->normals[ptr + 2] = 0;
			ptr += 3;
			data->normals[ptr] = 0;
			data->normals[ptr + 1] = 1;
			data->normals[ptr + 2] = 0;
			ptr += 3;
		}
	}

	ptr = 0;
	for (int zz = 0; zz < data->gc; ++zz) {
		for (int xx = 0; xx < data->gc + 1; ++xx) {
			data->uvs[ptr] = (GLfloat)xx / (GLfloat)(data->gc);
			data->uvs[ptr + 1] = (GLfloat)(zz + 1) / (GLfloat)(data->gc);
			ptr += 2;
			data->uvs[ptr] = (GLfloat)xx / (GLfloat)(data->gc);
			data->uvs[ptr + 1] = (GLfloat)(zz) / (GLfloat)(data->gc);
			ptr += 2;
		}
	}

	unsigned v = GL_STATIC_DRAW;
	if (m == 1) { v = GL_DYNAMIC_DRAW; }
	glBindBuffer(GL_ARRAY_BUFFER, data->buff_positions);
	glBufferData(GL_ARRAY_BUFFER, ((data->gc + 1) * data->gc * 24), data->positions, v);
	glBindBuffer(GL_ARRAY_BUFFER, data->buff_normals);
	glBufferData(GL_ARRAY_BUFFER, ((data->gc + 1) * data->gc * 24), data->normals, v);
	glBindBuffer(GL_ARRAY_BUFFER, data->buff_uvs);
	glBufferData(GL_ARRAY_BUFFER, ((data->gc + 1) * data->gc * 16), data->uvs, v);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// dont save memory
	if (m == 0) {
		free(data->positions);
		free(data->normals);
		free(data->uvs);
		data->positions = NULL;
		data->normals = NULL;
		data->uvs = NULL;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDataLoad(double d, char* fn, double m) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	
	// parse terrain file
	unsigned int null, precision;
	null = precision = data->gs = data->gc = 0;

	FILE* terrain = fopen(fn, "rb");
	if (terrain == NULL) { return 0; }

	fread(&null, 4, 1, terrain);
	data->gs |= fgetc(terrain);
	data->gs |= (fgetc(terrain) << 8);
	data->gc |= fgetc(terrain);
	data->gc |= (fgetc(terrain) << 8);
	precision |= fgetc(terrain);
	precision |= (fgetc(terrain) << 8);
	fread(&null, 4, 1, terrain);

	signed int* ybuffer;
	if (precision == 4 || precision == 67108864) {
		unsigned size = (data->gc + 1) * (data->gc + 1) * 4;
		ybuffer = (signed int*)malloc(size);
		memset(ybuffer, 0, size);
		for (int i = 0; i < ((data->gc + 1) * (data->gc + 1)); ++i) {
			ybuffer[i] |= fgetc(terrain);
			ybuffer[i] |= (fgetc(terrain) << 8);
			ybuffer[i] |= (fgetc(terrain) << 16);
			ybuffer[i] |= (fgetc(terrain) << 24);
		}
	} else {
		return 0;
	}

	fclose(terrain);

	// construct terrain
	data->vertices_cnt = ((data->gc + 1) * data->gc) * 2;
	data->positions = (GLfloat*)malloc((data->gc + 1) * data->gc * 24);
	data->normals = (GLfloat*)malloc((data->gc + 1) * data->gc * 24);
	data->uvs = (GLfloat*)malloc((data->gc + 1) * data->gc * 16);

	unsigned int ptr = 0;
	for (int zz = 0; zz < data->gc; ++zz) {
		for (int xx = 0; xx < data->gc + 1; ++xx) {
			unsigned pos;
			pos = ((zz + 1) * (data->gc + 1)) + xx;
			data->positions[ptr] = xx * data->gs;
			data->positions[ptr + 1] = ybuffer[pos];
			data->positions[ptr + 2] = (zz + 1) * data->gs;
			ptr += 3;
			pos = (zz * (data->gc + 1)) + xx;
			data->positions[ptr] = xx * data->gs;
			data->positions[ptr + 1] = ybuffer[pos];
			data->positions[ptr + 2] = zz * data->gs;
			ptr += 3;
		}
	}

	ptr = 0;
	for (int zz = 0; zz < data->gc; ++zz) {
		for (int xx = 0; xx < data->gc + 1; ++xx) {
			glm::vec3 normal;
			normal = calculate_normal(ybuffer, data->gc, data->gs, xx, zz + 1);
			data->normals[ptr] = normal.x;
			data->normals[ptr + 1] = normal.y;
			data->normals[ptr + 2] = normal.z;
			ptr += 3;
			normal = calculate_normal(ybuffer, data->gc, data->gs, xx, zz);
			data->normals[ptr] = normal.x;
			data->normals[ptr + 1] = normal.y;
			data->normals[ptr + 2] = normal.z;
			ptr += 3;
		}
	}

	ptr = 0;
	for (int zz = 0; zz < data->gc; ++zz) {
		for (int xx = 0; xx < data->gc + 1; ++xx) {
			data->uvs[ptr] = (GLfloat)xx / (GLfloat)(data->gc);
			data->uvs[ptr + 1] = (GLfloat)(zz + 1) / (GLfloat)(data->gc);
			ptr += 2;
			data->uvs[ptr] = (GLfloat)xx / (GLfloat)(data->gc);
			data->uvs[ptr + 1] = (GLfloat)(zz) / (GLfloat)(data->gc);
			ptr += 2;
		}
	}

	unsigned v = GL_STATIC_DRAW;
	if (m == 1) { v = GL_DYNAMIC_DRAW; }
	glBindBuffer(GL_ARRAY_BUFFER, data->buff_positions);
	glBufferData(GL_ARRAY_BUFFER, ((data->gc + 1) * data->gc * 24), data->positions, v);
	glBindBuffer(GL_ARRAY_BUFFER, data->buff_normals);
	glBufferData(GL_ARRAY_BUFFER, ((data->gc + 1) * data->gc * 24), data->normals, v);
	glBindBuffer(GL_ARRAY_BUFFER, data->buff_uvs);
	glBufferData(GL_ARRAY_BUFFER, ((data->gc + 1) * data->gc * 16), data->uvs, v);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// dont save memory
	free(ybuffer);
	if (m == 0) {
		free(data->positions);
		free(data->normals);
		free(data->uvs);
		data->positions = NULL;
		data->normals = NULL;
		data->uvs = NULL;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDataFreeMemory(double d) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	if (data->positions != NULL) {
		free(data->positions);
		data->positions = NULL;
	}
	if (data->normals != NULL) {
		free(data->normals);
		data->normals = NULL;
	}
	if (data->uvs != NULL) {
		free(data->uvs);
		data->uvs = NULL;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDataDelete(double d) {

	TerrainData* data = (TerrainData*)(ULONG)d;
	glDeleteBuffers(1, &data->buff_positions);
	glDeleteBuffers(1, &data->buff_normals);
	glDeleteBuffers(1, &data->buff_uvs);
	free(data->positions);
	free(data->normals);
	free(data->uvs);
	free(data);

	return RR_SUCCESS;
}

//* MODEL DATA *//

RRENDER_API double RRender_ModelDataCreate() {

	ModelData* data = (ModelData*)malloc(sizeof(ModelData));
	data->buff_vertices = 0;
	data->id = 0;
	data->program = 0;
	data->vertices = 0;
	data->vertices_cnt = 0;
	data->texture = 0;

	glGenBuffers(1, &data->buff_vertices);

	return (double)(ULONG)data;
}

RRENDER_API double RRender_ModelDataSetTexture(double d, double t) {

	ModelData* data = (ModelData*)(ULONG)d;
	Texture* texture = (Texture*)(ULONG)t;
	data->texture = texture;
	
	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataSetPosition(double d, double x, double y, double z) {

	ModelData* data = (ModelData*)(ULONG)d;
	data->translation = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataSetRotation(double d, double x, double y, double z) {

	ModelData* data = (ModelData*)(ULONG)d;
	data->rotation = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataSetScale(double d, double x, double y, double z) {

	ModelData* data = (ModelData*)(ULONG)d;
	data->scale = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataSetShader(double d, double shader) {

	ModelData* data = (ModelData*)(ULONG)d;
	data->program = (GLuint)shader;

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataSetColor(double d, double col) {

	unsigned int color = (double)col;
	ModelData* data = (ModelData*)(ULONG)d;
	data->color.r = (float)(color & 255);
	data->color.g = (float)((color >> 8) & 255);
	data->color.b = (float)((color >> 16) & 255);
	for (unsigned int i = 0; i < data->vertices_cnt; ++i) {
		data->vertices[i].color.r = data->color.r;
		data->vertices[i].color.g = data->color.g;
		data->vertices[i].color.b = data->color.b;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataGetPosition(double d, double v) {

	ModelData* data = (ModelData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->translation.x;
		break;
	case 1:
		return data->translation.y;
		break;
	case 2:
		return data->translation.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataGetRotation(double d, double v) {

	ModelData* data = (ModelData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->rotation.x;
		break;
	case 1:
		return data->rotation.y;
		break;
	case 2:
		return data->rotation.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataGetScale(double d, double v) {

	ModelData* data = (ModelData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->scale.x;
		break;
	case 1:
		return data->scale.y;
		break;
	case 2:
		return data->scale.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataGetShader(double d) {

	ModelData* data = (ModelData*)(ULONG)d;
	return data->program;
}

RRENDER_API double RRender_ModelDataGetColor(double d, double v) {

	ModelData* data = (ModelData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->color.r;
		break;
	case 1:
		return data->color.g;
		break;
	case 2:
		return data->color.b;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataLoad(double d, char* fn, double m) {

	ModelData* data = (ModelData*)(ULONG)d;
	data->vertices_cnt = RRender_ML_Load(fn, &data->vertices);

	glBindBuffer(GL_ARRAY_BUFFER, data->buff_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RR3DVertex) * data->vertices_cnt, data->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// dont save memory
	if (m == 0) {
		free(data->vertices);
		data->vertices = NULL;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataFreeMemory(double d) {

	ModelData* data = (ModelData*)(ULONG)d;
	if (data->vertices != NULL) {
		free(data->vertices);
		data->vertices = NULL;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDataDelete(double d) {

	ModelData* data = (ModelData*)(ULONG)d;
	glDeleteBuffers(1, &data->buff_vertices);
	if (data->vertices != NULL) {
		free(data->vertices);
	}
	free(data);

	return RR_SUCCESS;
}

//* PRIMITIVE DATA *//

RRENDER_API double RRender_PrimitiveDataCreate() {

	PrimitiveData* data = (PrimitiveData*)malloc(sizeof(PrimitiveData));
	data->buff_vertices = 0;
	data->id = 0;
	data->program = 0;
	data->vertices = 0;
	data->vertices_cnt = 0;

	glGenBuffers(1, &data->buff_vertices);

	return (double)(ULONG)data;
}

RRENDER_API double RRender_PrimitiveDataSetPosition(double d, double x, double y, double z) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	data->translation = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataSetRotation(double d, double x, double y, double z) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	data->rotation = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataSetScale(double d, double x, double y, double z) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	data->scale = glm::vec3(x, y, z);

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataSetShader(double d, double shader) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	data->program = (GLuint)shader;

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataSetColor(double d, double col) {

	unsigned int color = (double)col;
	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	data->color.r = (float)(color & 255);
	data->color.g = (float)((color >> 8) & 255);
	data->color.b = (float)((color >> 16) & 255);
	for (unsigned int i = 0; i < data->vertices_cnt; ++i) {
		data->vertices[i].color.r = data->color.r;
		data->vertices[i].color.g = data->color.g;
		data->vertices[i].color.b = data->color.b;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataGetPosition(double d, double v) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->translation.x;
		break;
	case 1:
		return data->translation.y;
		break;
	case 2:
		return data->translation.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataGetRotation(double d, double v) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->rotation.x;
		break;
	case 1:
		return data->rotation.y;
		break;
	case 2:
		return data->rotation.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataGetScale(double d, double v) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->scale.x;
		break;
	case 1:
		return data->scale.y;
		break;
	case 2:
		return data->scale.z;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataGetShader(double d) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	return data->program;
}

RRENDER_API double RRender_PrimitiveDataGetColor(double d, double v) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	char val = (char)v;
	switch (val) {
	case 0:
		return data->color.r;
		break;
	case 1:
		return data->color.g;
		break;
	case 2:
		return data->color.b;
		break;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataGenShape(double d, double idd, double param, double m) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	char id = (char)idd;
	data->id = id;

	switch (id) {
	case RR_PRIMITIVE_ID_SPHERE : {
		// Sphere
		unsigned total = (unsigned)param;
		data->vertices_cnt = ((total + 1) * (total + 1) * 6);
		data->vertices = (RR3DVertex*)malloc(sizeof(RR3DVertex) * data->vertices_cnt);
		for (int i = 0; i <= total; ++i) {
			for (int j = 0; j <= total; ++j) {
				float lon, lat;
				// triangle 1
				lon = glm::radians((float)i * (360.0f / (float)total));
				lat = glm::radians((float)j * (360.0f / (float)total));
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].pos.x = sin(lon) * cos(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].pos.y = cos(lon);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].pos.z = sin(lon) * sin(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].normal.x = data->vertices[(i * ((total + 1) * 6)) + (j * 6)].pos.x;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].normal.y = data->vertices[(i * ((total + 1) * 6)) + (j * 6)].pos.y;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].normal.z = data->vertices[(i * ((total + 1) * 6)) + (j * 6)].pos.z;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].color.r = data->color.r;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].color.g = data->color.g;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6)].color.b = data->color.b;
				lon = glm::radians((float)(i + 1) * (360.0f / (float)total));
				lat = glm::radians((float)(j + 1) * (360.0f / (float)total));
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].pos.x = sin(lon) * cos(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].pos.y = cos(lon);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].pos.z = sin(lon) * sin(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].normal.x = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].pos.x;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].normal.y = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].pos.y;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].normal.z = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].pos.z;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].color.r = data->color.r;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].color.g = data->color.g;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 1].color.b = data->color.b;
				lon = glm::radians((float)(i + 1) * (360.0f / (float)total));
				lat = glm::radians((float)j * (360.0f / (float)total));
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].pos.x = sin(lon) * cos(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].pos.y = cos(lon);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].pos.z = sin(lon) * sin(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].normal.x = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].pos.x;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].normal.y = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].pos.y;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].normal.z = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].pos.z;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].color.r = data->color.r;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].color.g = data->color.g;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 2].color.b = data->color.b;
				// triangle 2
				lon = glm::radians((float)i * (360.0f / (float)total));
				lat = glm::radians((float)j * (360.0f / (float)total));
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].pos.x = sin(lon) * cos(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].pos.y = cos(lon);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].pos.z = sin(lon) * sin(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].normal.x = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].pos.x;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].normal.y = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].pos.y;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].normal.z = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].pos.z;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].color.r = data->color.r;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].color.g = data->color.g;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 3].color.b = data->color.b;
				lon = glm::radians((float)i * (360.0f / (float)total));
				lat = glm::radians((float)(j + 1) * (360.0f / (float)total));
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].pos.x = sin(lon) * cos(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].pos.y = cos(lon);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].pos.z = sin(lon) * sin(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].normal.x = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].pos.x;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].normal.y = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].pos.y;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].normal.z = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].pos.z;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].color.r = data->color.r;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].color.g = data->color.g;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 4].color.b = data->color.b;
				lon = glm::radians((float)(i + 1) * (360.0f / (float)total));
				lat = glm::radians((float)(j + 1) * (360.0f / (float)total));
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].pos.x = sin(lon) * cos(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].pos.y = cos(lon);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].pos.z = sin(lon) * sin(lat);
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].normal.x = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].pos.x;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].normal.y = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].pos.y;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].normal.z = data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].pos.z;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].color.r = data->color.r;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].color.g = data->color.g;
				data->vertices[(i * ((total + 1) * 6)) + (j * 6) + 5].color.b = data->color.b;
			}
		}
		// filling buffer
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RR3DVertex) * data->vertices_cnt, data->vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		break; }
	case RR_PRIMITIVE_ID_PLANE :
		// Plane
		data->vertices_cnt = 6;
		data->vertices = (RR3DVertex*)malloc(sizeof(RR3DVertex) * data->vertices_cnt);
		// front face
		data->vertices[0] = *(new RR3DVertex(-0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 0.0, 1.0));
		data->vertices[1] = *(new RR3DVertex(0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 0.0, 0.0));
		data->vertices[2] = *(new RR3DVertex(-0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 1.0, 1.0));
		data->vertices[3] = *(new RR3DVertex(-0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 0.0, 0.0));
		data->vertices[4] = *(new RR3DVertex(0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 1.0, 1.0));
		data->vertices[5] = *(new RR3DVertex(0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 1.0, 0.0));
		// filling buffer
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RR3DVertex) * data->vertices_cnt, data->vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		break;
	case RR_PRIMITIVE_ID_CUBE :
		// Cube
		data->vertices_cnt = 36;
		data->vertices = (RR3DVertex*)malloc(sizeof(RR3DVertex) * data->vertices_cnt);
		// front face
		data->vertices[0] = *(new RR3DVertex(-0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 0.0, 1.0));
		data->vertices[1] = *(new RR3DVertex(0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 0.0, 0.0));
		data->vertices[2] = *(new RR3DVertex(-0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 1.0, 1.0));
		data->vertices[3] = *(new RR3DVertex(-0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 0.0, 0.0));
		data->vertices[4] = *(new RR3DVertex(0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 1.0, 1.0));
		data->vertices[5] = *(new RR3DVertex(0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, 1.0, 1.0, 0.0));
		// back face
		data->vertices[6] = *(new RR3DVertex(0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, -1.0, 0.0, 1.0));
		data->vertices[7] = *(new RR3DVertex(-0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, -1.0, 0.0, 0.0));
		data->vertices[8] = *(new RR3DVertex(-0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, -1.0, 1.0, 1.0));
		data->vertices[9] = *(new RR3DVertex(0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, -1.0, 0.0, 0.0));
		data->vertices[10] = *(new RR3DVertex(-0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, -1.0, 1.0, 1.0));
		data->vertices[11] = *(new RR3DVertex(0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 0.0, -1.0, 1.0, 0.0));
		// up face
		data->vertices[12] = *(new RR3DVertex(-0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 1.0, 0.0, 0.0, 1.0));
		data->vertices[13] = *(new RR3DVertex(-0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 1.0, 0.0, 0.0, 0.0));
		data->vertices[14] = *(new RR3DVertex(0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 1.0, 0.0, 1.0, 1.0));
		data->vertices[15] = *(new RR3DVertex(-0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 1.0, 0.0, 0.0, 0.0));
		data->vertices[16] = *(new RR3DVertex(0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, 1.0, 0.0, 1.0, 1.0));
		data->vertices[17] = *(new RR3DVertex(0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, 1.0, 0.0, 1.0, 0.0));
		// bottom face
		data->vertices[18] = *(new RR3DVertex(-0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, -1.0, 0.0, 0.0, 1.0));
		data->vertices[19] = *(new RR3DVertex(-0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, -1.0, 0.0, 0.0, 0.0));
		data->vertices[20] = *(new RR3DVertex(0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, -1.0, 0.0, 1.0, 1.0));
		data->vertices[21] = *(new RR3DVertex(-0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, -1.0, 0.0, 0.0, 0.0));
		data->vertices[22] = *(new RR3DVertex(0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 0.0, -1.0, 0.0, 1.0, 1.0));
		data->vertices[23] = *(new RR3DVertex(0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, 0.0, -1.0, 0.0, 1.0, 0.0));
		// right face
		data->vertices[24] = *(new RR3DVertex(0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 1.0, 0.0, 0.0, 0.0, 1.0));
		data->vertices[25] = *(new RR3DVertex(0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, 1.0, 0.0, 0.0, 0.0, 0.0));
		data->vertices[26] = *(new RR3DVertex(0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, 1.0, 0.0, 0.0, 1.0, 1.0));
		data->vertices[27] = *(new RR3DVertex(0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, 1.0, 0.0, 0.0, 0.0, 0.0));
		data->vertices[28] = *(new RR3DVertex(0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, 1.0, 0.0, 0.0, 1.0, 1.0));
		data->vertices[29] = *(new RR3DVertex(0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, 1.0, 0.0, 0.0, 1.0, 0.0));
		// left face
		data->vertices[30] = *(new RR3DVertex(-0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, -1.0, 0.0, 0.0, 0.0, 1.0));
		data->vertices[31] = *(new RR3DVertex(-0.5, 0.5, -0.5, data->color.r, data->color.g, data->color.b, -1.0, 0.0, 0.0, 0.0, 0.0));
		data->vertices[32] = *(new RR3DVertex(-0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, -1.0, 0.0, 0.0, 1.0, 1.0));
		data->vertices[33] = *(new RR3DVertex(-0.5, -0.5, -0.5, data->color.r, data->color.g, data->color.b, -1.0, 0.0, 0.0, 0.0, 0.0));
		data->vertices[34] = *(new RR3DVertex(-0.5, 0.5, 0.5, data->color.r, data->color.g, data->color.b, -1.0, 0.0, 0.0, 1.0, 1.0));
		data->vertices[35] = *(new RR3DVertex(-0.5, -0.5, 0.5, data->color.r, data->color.g, data->color.b, -1.0, 0.0, 0.0, 1.0, 0.0));
		// filling buffer
		glBindBuffer(GL_ARRAY_BUFFER, data->buff_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RR3DVertex) * data->vertices_cnt, data->vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		break;
	}

	// dont save memory
	if (m == 0) {
		free(data->vertices);
		data->vertices = NULL;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataFreeMemory(double d) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	if (data->vertices != NULL) {
		free(data->vertices);
		data->vertices = NULL;
	}

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDataDelete(double d) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)d;
	glDeleteBuffers(1, &data->buff_vertices);
	if (data->vertices != NULL) {
		free(data->vertices);
	}
	free(data);

	return RR_SUCCESS;
}

//* BILLBOARD *//

RRENDER_API double RRender_BillboardCreate(double dataptr) {

	BillboardData* data = (BillboardData*)(ULONG)dataptr;
	Billboard* terrain = new Billboard(data);

	return (double)(ULONG)terrain;
}

RRENDER_API double RRender_BillboardHide(double p, double v) {

	Billboard* billboard = (Billboard*)(ULONG)p;
	billboard->setHidden((unsigned)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_BillboardDelete(double p) {

	Billboard* billboard = (Billboard*)(ULONG)p;
	delete billboard;

	return RR_SUCCESS;
}

//* TERRAIN *//

RRENDER_API double RRender_TerrainCreate(double dataptr) {

	TerrainData* data = (TerrainData*)(ULONG)dataptr;
	Terrain* terrain = new Terrain(data);

	return (double)(ULONG)terrain;
}

RRENDER_API double RRender_TerrainHide(double p, double v) {

	Terrain* terrain = (Terrain*)(ULONG)p;
	terrain->setHidden((unsigned)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_TerrainDelete(double p) {

	Terrain* terrain = (Terrain*)(ULONG)p;
	delete terrain;

	return RR_SUCCESS;
}

//* MODEL *//

RRENDER_API double RRender_ModelCreate(double dataptr) {

	ModelData* data = (ModelData*)(ULONG)dataptr;
	Model* model = new Model(data);

	return (double)(ULONG)model;
}

RRENDER_API double RRender_ModelHide(double p, double v) {

	Model* model = (Model*)(ULONG)p;
	model->setHidden((unsigned)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_ModelDelete(double p) {

	Model* model = (Model*)(ULONG)p;
	delete model;

	return RR_SUCCESS;
}

//* PRIMITIVE *//

RRENDER_API double RRender_PrimitiveCreate(double dataptr) {

	PrimitiveData* data = (PrimitiveData*)(ULONG)dataptr;
	Primitive* primitive = new Primitive(data);

	return (double)(ULONG)primitive;
}

RRENDER_API double RRender_PrimitiveHide(double p, double v) {

	Primitive* primitive = (Primitive*)(ULONG)p;
	primitive->setHidden((unsigned)v);

	return RR_SUCCESS;
}

RRENDER_API double RRender_PrimitiveDelete(double p) {

	Primitive* primitive = (Primitive*)(ULONG)p;
	delete primitive;

	return RR_SUCCESS;
}

//* SHADER *//

RRENDER_API double RRender_ShaderCreate(char* vstr, char* fstr, char* gstr) {

	if (gstr[0] == 'N' && gstr[1] == 'U' && gstr[2] == 'L' && gstr[3] == 'L') { gstr = NULL; }

	GLuint program = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint gs;
	if (gstr != NULL) { gs = glCreateShader(GL_GEOMETRY_SHADER); }
	GLint vstrlen = strlen(vstr);
	GLint fstrlen = strlen(fstr);
	GLint gstrlen;
	if (gstr != NULL) { gstrlen = strlen(gstr); }
	glShaderSource(vs, 1, &vstr, &vstrlen);
	glShaderSource(fs, 1, &fstr, &fstrlen);
	if (gstr != NULL) { glShaderSource(gs, 1, &gstr, &gstrlen); }
	glCompileShader(vs);
	glCompileShader(fs);
	if (gstr != NULL) { glCompileShader(gs); }
	GLint res;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &res);
	if (res != GL_TRUE) { MessageBox(NULL, "Error creating vertex shader!", "Error", MB_ICONERROR); }
	glGetShaderiv(fs, GL_COMPILE_STATUS, &res);
	if (res != GL_TRUE) { MessageBox(NULL, "Error creating fragment shader!", "Error", MB_ICONERROR); }
	if (gstr != NULL) {
		glGetShaderiv(gs, GL_COMPILE_STATUS, &res);
		if (res != GL_TRUE) { MessageBox(NULL, "Error creating geometry shader!", "Error", MB_ICONERROR); }
	}
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	if (gstr != NULL) { glAttachShader(program, gs); }
	glLinkProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	if (gstr != NULL) { glDeleteShader(gs); }
	free(vstr);
	free(fstr);
	if (gstr != NULL) { free(gstr); }

	return (double)program;
}

RRENDER_API double RRender_ShaderDelete(double p) {

	GLuint program = (GLuint)p;
	glDeleteProgram(program);

	return RR_SUCCESS;
}

RRENDER_API double RRender_ShaderGetUniform(double p, char* u) {

	GLuint program = (GLuint)p;

	return glGetUniformLocation(program, u);
}

RRENDER_API double RRender_ShaderUniform1i(double p, double u, double x) {

	GLuint program = (GLuint)p;
	glUseProgram(program);
	glUniform1i((GLint)u, (GLint)x);
	glUseProgram(0);

	return RR_SUCCESS;
}

RRENDER_API double RRender_ShaderUniform2f(double p, double u, double x, double y) {

	GLuint program = (GLuint)p;
	glUseProgram(program);
	glUniform2f((GLint)u, (float)x, (float)y);
	glUseProgram(0);

	return RR_SUCCESS;
}

RRENDER_API double RRender_ShaderUniform3f(double p, double u, double x, double y, double z) {
	
	GLuint program = (GLuint)p;
	glUseProgram(program);
	glUniform3f((GLint)u, (float)x, (float)y, (float)z);
	glUseProgram(0);

	return RR_SUCCESS;
}

RRENDER_API double RRender_ShaderUniform4f(double p, double u, double x, double y, double z, double w) {

	GLuint program = (GLuint)p;
	glUseProgram(program);
	glUniform4f((GLint)u, (float)x, (float)y, (float)z, (float)w);
	glUseProgram(0);

	return RR_SUCCESS;
}

//* WINDOW *//

RRENDER_API double RRender_WindowInit(double handle, double w, double h, double f) {

	// init
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	HWND hwnd = (HWND)(ULONG)handle;
	HDC hdc = GetDC(hwnd);
	if (hdc == NULL) {
		MessageBox(NULL, "Error creating HDC!", "Error", MB_ICONERROR);
		return 0;
	}

	int fmt = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, fmt, &pfd);

	HGLRC hglrc = wglCreateContext(hdc);
	if (!wglMakeCurrent(hdc, hglrc)) {
		MessageBox(NULL, "Error creating OpenGL context!", "Error", MB_ICONERROR);
		return 0;
	}

	int res = glewInit();
	if (res != GLEW_OK) {
		MessageBox(NULL, "Error initializing GLEW!", "Error", MB_ICONERROR);
		return 0;
	}

	if (f == 1) {
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP);
		SetWindowPos(hwnd, 0, 0, 0, (unsigned)w, (unsigned)h, SWP_SHOWWINDOW);
	}

	RECT rect;
	GetClientRect(WindowFromDC(hdc), &rect);
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;
	width = (UINT)w;
	height = (UINT)h;

	// framebuffer
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);

	Texture* texture_col = new Texture(NULL, 0, 0);
	Texture* texture_depth = new Texture(NULL, 0, 0);
	
	glBindTexture(GL_TEXTURE_2D, texture_col->getTexture());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, texture_depth->getTexture());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_col->getTexture(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_depth->getTexture(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// screen texture
	Texture* texture_screen = new Texture(NULL, 0, 0);
	glBindTexture(GL_TEXTURE_2D, texture_screen->getTexture());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create window
	Window* window = new Window(framebuffer, texture_screen, texture_col, texture_depth, hwnd, hdc, hglrc);

	return (double)(ULONG)window;
}

RRENDER_API double RRender_WindowDraw(double w) {

	Window* window = (Window*)(ULONG)w;

	Camera* camera = window->getCamera();
	HDC hdc = window->getHdc();

	RECT rect;
	GetClientRect(WindowFromDC(hdc), &rect);
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	glViewport(0, 0, width, height);

	unsigned int color = (unsigned int)window->getColor();
	float r = (float)(color & 255) / 255;
	float g = (float)((color >> 8) & 255) / 255;
	float b = (float)((color >> 16) & 255) / 255;

	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, window->getFramebuffer());
	// enable
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	// clear
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// draw instance buffer
	for (unsigned int i = 0; i < RR3D_instance_buffer_cnt; ++i) {
		if (RR3D_instance_buffer_id[i] == RR_ID_PRIMITIVE) {
			Primitive* primitive = (Primitive*)RR3D_instance_buffer[i];
			if (!primitive->getHidden()) {
				primitive->Draw(*camera);
			}
		}
		if (RR3D_instance_buffer_id[i] == RR_ID_MODEL) {
			Model* model = (Model*)RR3D_instance_buffer[i];
			if (!model->getHidden()) {
				model->Draw(*camera);
			}
		}
		if (RR3D_instance_buffer_id[i] == RR_ID_TERRAIN) {
			Terrain* terrain = (Terrain*)RR3D_instance_buffer[i];
			if (!terrain->getHidden()) {
				terrain->Draw(*camera);
			}
		}
		if (RR3D_instance_buffer_id[i] == RR_ID_BILLBOARD) {
			Billboard* billboard = (Billboard*)RR3D_instance_buffer[i];
			if (!billboard->getHidden()) {
				billboard->Draw(*camera);
			}
		}
	}
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// draw screen buffer
	double screen_texture = (double)(ULONG)window->getTextureScreen();
	for (unsigned int i = 0; i < RR3D_screen_buffer_cnt; ++i) {
		if (RR3D_screen_buffer_id[i] == RR_ID_TEXTURE) {
			Texture* texture = (Texture*)RR3D_screen_buffer[i];
			if (!texture->getHidden()) {
				RRender_TextureSetRegion(screen_texture, (double)(ULONG)texture, RR3D_screen_buffer_pos[i] >> 16, RR3D_screen_buffer_pos[i] & 0xff);
			}
		}
		if (RR3D_screen_buffer_id[i] == RR_ID_TEXT) {
			Text* text = (Text*)RR3D_screen_buffer[i];
			if (!text->hidden) {
				TextDraw(window->getTextureScreen(), text, RR3D_screen_buffer_pos[i] >> 16, RR3D_screen_buffer_pos[i] & 0xff);
			}
		}
	}

	// draw texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, window->getTextureCol()->getTexture());
	glUseProgram(window->getProgram());
	GLuint texture_location = glGetUniformLocation(window->getProgram(), "u_tex");
	glUniform1i(texture_location, 0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, window->getQuadBuff());
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);
	glDrawArrays(GL_QUADS, 0, 4);

	glBindTexture(GL_TEXTURE_2D, window->getTextureScreen()->getTexture());
	glUniform1i(texture_location, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);
	glDrawArrays(GL_QUADS, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);

	SwapBuffers(hdc);

	return RR_SUCCESS;
}

RRENDER_API double RRender_WindowSetShader(double w, double program) {

	Window* window = (Window*)(ULONG)w;
	window->setProgram((GLuint)program);

	return RR_SUCCESS;
}

RRENDER_API double RRender_WindowSetColor(double w, double col) {

	Window* window = (Window*)(ULONG)w;
	window->setColor(col);

	return RR_SUCCESS;
}

RRENDER_API double RRender_WindowSetHwnd(double w, double ptr) {

	HWND hwnd = (HWND)(ULONG)ptr;

	Window* window = (Window*)(ULONG)w;
	window->setHwnd(hwnd);

	return RR_SUCCESS;
}

RRENDER_API double RRender_WindowSetHdc(double w, double ptr) {

	HDC hdc = (HDC)(ULONG)ptr;

	Window* window = (Window*)(ULONG)w;
	window->setHdc(hdc);

	return RR_SUCCESS;
}

RRENDER_API double RRender_WindowSetCamera(double w, double ptr) {

	Camera* camera = (Camera*)(ULONG)ptr;

	Window* window = (Window*)(ULONG)w;
	window->setCamera(camera);

	return RR_SUCCESS;
}

RRENDER_API double RRender_WindowGetTextureScreen(double w) {

	Window* window = (Window*)(ULONG)w;

	return (double)(ULONG)window->getTextureScreen();
}

RRENDER_API double RRender_WindowGetTextureCol(double w) {

	Window* window = (Window*)(ULONG)w;

	return (double)(ULONG)window->getTextureCol();
}

RRENDER_API double RRender_WindowGetTextureDepth(double w) {

	Window* window = (Window*)(ULONG)w;

	return (double)(ULONG)window->getTextureDepth();
}

RRENDER_API double RRender_WindowGetShader(double w) {

	Window* window = (Window*)(ULONG)w;

	return (double)window->getProgram();
}

RRENDER_API double RRender_WindowGetColor(double w) {

	Window* window = (Window*)(ULONG)w;

	return (double)window->getColor();
}

RRENDER_API double RRender_WindowGetHwnd(double w) {

	Window* window = (Window*)(ULONG)w;

	return (double)(ULONG)window->getHwnd();
}

RRENDER_API double RRender_WindowGetHdc(double w) {

	Window* window = (Window*)(ULONG)w;

	return (double)(ULONG)window->getHdc();
}

RRENDER_API double RRender_WindowGetCamera(double w) {

	Window* window = (Window*)(ULONG)w;

	return (double)(ULONG)window->getCamera();
}