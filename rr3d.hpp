#ifndef __RR3D__
#define __RR3D__

#define RRENDER_API extern "C" __declspec (dllexport)

#define RR_SUCCESS 1

#define RR_ID_PRIMITIVE			 0
#define RR_ID_MODEL				 1
#define RR_ID_TERRAIN            2
#define RR_ID_BILLBOARD          3
#define RR_ID_TEXTURE            4
#define RR_ID_TEXT               5
#define RR_PRIMITIVE_ID_CUBE	 0
#define RR_PRIMITIVE_ID_PLANE    1
#define RR_PRIMITIVE_ID_SPHERE   2

//* OTHER *//
RRENDER_API char* RRender_FileRead(char*);
RRENDER_API char* RRender_StringCreate(double);

//* VECTOR *//
RRENDER_API double RRender_VectorCreate(double, double, double);
RRENDER_API double RRender_VectorSet(double, double, double, double);
RRENDER_API double RRender_VectorGet(double, double);
RRENDER_API double RRender_VectorDot(double, double);
RRENDER_API double RRender_VectorCross(double, double);
RRENDER_API double RRender_VectorDelete(double);

//* COLLISION SYSTEM *//
RRENDER_API double RRender_CSCreate();
RRENDER_API double RRender_CSAddCollider(double, double, double, double, double, double);
RRENDER_API double RRender_CSRemoveCollider(double);
RRENDER_API double RRender_CSSetCamera(double);
RRENDER_API double RRender_CSSetRadius(double);
RRENDER_API double RRender_CSSetGravity(double, double);
RRENDER_API double RRender_CSMove(double);
RRENDER_API double RRender_CSStrafe(double);
RRENDER_API double RRender_CSLift(double);
RRENDER_API double RRender_CSUpdate();
RRENDER_API double RRender_CSGetColliderCount();

//* COLLISION *//
RRENDER_API double RRender_CollisionGetPosition(double);
RRENDER_API double RRender_CollisionRayTerrain(double, double);
RRENDER_API double RRender_CollisionRayBillboards(double);
RRENDER_API double RRender_CollisionRayModels(double);

//* CAMERA *//
RRENDER_API double RRender_CameraCreate(double, double, double, double);
RRENDER_API double RRender_CameraSetPitch(double, double);
RRENDER_API double RRender_CameraSetYaw(double, double);
RRENDER_API double RRender_CameraSetRoll(double, double);
RRENDER_API double RRender_CameraAddPitch(double, double);
RRENDER_API double RRender_CameraAddYaw(double, double);
RRENDER_API double RRender_CameraAddRoll(double, double);
RRENDER_API double RRender_CameraSetPosition(double, double, double, double);
RRENDER_API double RRender_CameraGetPitch(double);
RRENDER_API double RRender_CameraGetYaw(double);
RRENDER_API double RRender_CameraGetRoll(double);
RRENDER_API double RRender_CameraGetPosition(double, double);
RRENDER_API double RRender_CameraMove(double, double);
RRENDER_API double RRender_CameraStrafe(double, double);
RRENDER_API double RRender_CameraLift(double, double);
RRENDER_API double RRender_CameraDelete(double);

//* SCREEN BUFFER *//
RRENDER_API double RRender_ScreenBufferCreate();
RRENDER_API double RRender_ScreenBufferAdd(double, double, double, double);
RRENDER_API double RRender_ScreenBufferRemove(double);
RRENDER_API double RRender_ScreenBufferSet(double, double, double);
RRENDER_API double RRender_ScreenBufferGetCount();

//* INSTANCE BUFFER *//
RRENDER_API double RRender_InstanceBufferCreate();
RRENDER_API double RRender_InstanceBufferAdd(double, double);
RRENDER_API double RRender_InstanceBufferRemove(double);
RRENDER_API double RRender_InstanceBufferGetCount();

//* FONT *//
RRENDER_API double RRender_FontCreate(char*);
RRENDER_API double RRender_FontDelete(double);

//* TEXT *//
RRENDER_API double RRender_TextCreate(char*, double, double, double);
RRENDER_API double RRender_TextHide(double, double);
RRENDER_API double RRender_TextDelete(double);

//* TEXTURE *//
RRENDER_API double RRender_TextureCreate(char* fn, double, double, double);
RRENDER_API double RRender_TextureHide(double, double);
RRENDER_API double RRender_TextureSetRegion(double, double, double, double);
RRENDER_API double RRender_TextureGetWidth(double);
RRENDER_API double RRender_TextureGetHeight(double);
RRENDER_API double RRender_TextureFreeMemory(double);
RRENDER_API double RRender_TextureSave(double, char*);
RRENDER_API double RRender_TextureDelete(double);

//* BILLBOARD DATA *//
RRENDER_API double RRender_BillboardDataCreate();
RRENDER_API double RRender_BillboardDataSetMeta(double, double, double);
RRENDER_API double RRender_BillboardDataSetTexture(double, double);
RRENDER_API double RRender_BillboardDataSetPosition(double, double, double, double);
RRENDER_API double RRender_BillboardDataSetRotation(double, double, double, double);
RRENDER_API double RRender_BillboardDataSetScale(double, double, double, double);
RRENDER_API double RRender_BillboardDataSetShader(double, double);
RRENDER_API double RRender_BillboardDataGetMeta(double, double);
RRENDER_API double RRender_BillboardDataGetPosition(double, double);
RRENDER_API double RRender_BillboardDataGetRotation(double, double);
RRENDER_API double RRender_BillboardDataGetScale(double, double);
RRENDER_API double RRender_BillboardDataGetShader(double);
RRENDER_API double RRender_BillboardDataDelete(double);

//* TERRAIN DATA *//
RRENDER_API double RRender_TerrainDataCreate();
RRENDER_API double RRender_TerrainDataSetMeta(double, double, double);
RRENDER_API double RRender_TerrainDataSetTexture(double, double);
RRENDER_API double RRender_TerrainDataSetPosition(double, double, double, double);
RRENDER_API double RRender_TerrainDataSetShader(double, double);
RRENDER_API double RRender_TerrainDataSetParams(double, double, double);
RRENDER_API double RRender_TerrainDataGetMeta(double, double);
RRENDER_API double RRender_TerrainDataGetYGrid(double, double, double);
RRENDER_API double RRender_TerrainDataAddY(double, double, double, double);
RRENDER_API double RRender_TerrainDataGetPosition(double, double);
RRENDER_API double RRender_TerrainDataGetShader(double);
RRENDER_API double RRender_TerrainDataGenShape(double, double);
RRENDER_API double RRender_TerrainDataLoad(double, char*, double);
RRENDER_API double RRender_TerrainDataFreeMemory(double);
RRENDER_API double RRender_TerrainDataDelete(double);

//* MODEL DATA *//
RRENDER_API double RRender_ModelDataCreate();
RRENDER_API double RRender_ModelDataSetMeta(double, double, double);
RRENDER_API double RRender_ModelDataSetTexture(double, double);
RRENDER_API double RRender_ModelDataSetPosition(double, double, double, double);
RRENDER_API double RRender_ModelDataSetRotation(double, double, double, double);
RRENDER_API double RRender_ModelDataSetScale(double, double, double, double);
RRENDER_API double RRender_ModelDataSetShader(double, double);
RRENDER_API double RRender_ModelDataSetColor(double, double);
RRENDER_API double RRender_ModelDataGetMeta(double, double);
RRENDER_API double RRender_ModelDataGetPosition(double, double);
RRENDER_API double RRender_ModelDataGetRotation(double, double);
RRENDER_API double RRender_ModelDataGetScale(double, double);
RRENDER_API double RRender_ModelDataGetShader(double);
RRENDER_API double RRender_ModelDataGetColor(double, double);
RRENDER_API double RRender_ModelDataLoad(double, char*, double);
RRENDER_API double RRender_ModelDataFreeMemory(double);
RRENDER_API double RRender_ModelDataDelete(double);

//* PRIMITIVE DATA *//
RRENDER_API double RRender_PrimitiveDataCreate();
RRENDER_API double RRender_PrimitiveDataSetMeta(double, double, double);
RRENDER_API double RRender_PrimitiveDataSetPosition(double, double, double, double);
RRENDER_API double RRender_PrimitiveDataSetRotation(double, double, double, double);
RRENDER_API double RRender_PrimitiveDataSetScale(double, double, double, double);
RRENDER_API double RRender_PrimitiveDataSetShader(double, double);
RRENDER_API double RRender_PrimitiveDataSetColor(double, double);
RRENDER_API double RRender_PrimitiveDataGetMeta(double, double);
RRENDER_API double RRender_PrimitiveDataGetPosition(double, double);
RRENDER_API double RRender_PrimitiveDataGetRotation(double, double);
RRENDER_API double RRender_PrimitiveDataGetScale(double, double);
RRENDER_API double RRender_PrimitiveDataGetShader(double);
RRENDER_API double RRender_PrimitiveDataGetColor(double, double);
RRENDER_API double RRender_PrimitiveDataGenShape(double, double, double, double);
RRENDER_API double RRender_PrimitiveDataFreeMemory(double);
RRENDER_API double RRender_PrimitiveDataDelete(double);

//* BILLBOARD *//
RRENDER_API double RRender_BillboardCreate(double);
RRENDER_API double RRender_BillboardGetData(double);
RRENDER_API double RRender_BillboardHide(double, double);
RRENDER_API double RRender_BillboardDelete(double);

//* TERRAIN *//
RRENDER_API double RRender_TerrainCreate(double);
RRENDER_API double RRender_TerrainGetData(double);
RRENDER_API double RRender_TerrainHide(double, double);
RRENDER_API double RRender_TerrainDelete(double);

//* MODEL *//
RRENDER_API double RRender_ModelCreate(double);
RRENDER_API double RRender_ModelGetData(double);
RRENDER_API double RRender_ModelHide(double, double);
RRENDER_API double RRender_ModelDelete(double);

//* PRIMITIVE *//
RRENDER_API double RRender_PrimitiveCreate(double);
RRENDER_API double RRender_PrimitiveGetData(double);
RRENDER_API double RRender_PrimitiveHide(double, double);
RRENDER_API double RRender_PrimitiveDelete(double);

//* SHADER *//
RRENDER_API double RRender_ShaderCreate(char*, char*, char*);
RRENDER_API double RRender_ShaderDelete(double);
RRENDER_API double RRender_ShaderGetUniform(double, char*);
RRENDER_API double RRender_ShaderUniform1i(double, double, double);
RRENDER_API double RRender_ShaderUniform2f(double, double, double, double);
RRENDER_API double RRender_ShaderUniform3f(double, double, double, double, double);
RRENDER_API double RRender_ShaderUniform4f(double, double, double, double, double, double);

//* WINDOW *//
RRENDER_API double RRender_WindowInit(double, double, double, double);
RRENDER_API double RRender_WindowDraw(double);
RRENDER_API double RRender_WindowSetShader(double, double);
RRENDER_API double RRender_WindowSetColor(double, double);
RRENDER_API double RRender_WindowSetHwnd(double, double);
RRENDER_API double RRender_WindowSetHdc(double, double);
RRENDER_API double RRender_WindowSetCamera(double, double);
RRENDER_API double RRender_WindowGetTextureCol(double);
RRENDER_API double RRender_WindowGetTextureDepth(double);
RRENDER_API double RRender_WindowGetTextureScreen(double);
RRENDER_API double RRender_WindowGetShader(double);
RRENDER_API double RRender_WindowGetColor(double);
RRENDER_API double RRender_WindowGetHwnd(double);
RRENDER_API double RRender_WindowGetHdc(double);
RRENDER_API double RRender_WindowGetCamera(double);

#endif
