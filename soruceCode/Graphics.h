#pragma once
#include <windows.h>
#include<stdint.h>
#include"Window.h"
#include<stdio.h>
#include<string>
#include<vector>
#include"Input.h"
#include "commonTools.h"
#include <fstream>
#include <math.h>


//error flags
#define FUNCTION_INVALID_MEMORY 100
#define FUNCTION_SUCCESS 101
#define FUNCTION_ALLOCATION_ERROR 103
#define FUNCTION_ERROR 104



#pragma pack(push, 1)//prevents padding


//FONT
struct INFO
{
	short fontSize;

	char smooth : 1;//0
	char unicode : 1;//1
	char italic : 1;//2
	char bold : 1;//3
	char fixedHeight : 1;//4
	char reserved : 3;//5-7

	u_char charSet;
	u_char stretchH;
	u_char aa;
	u_char paddingUp;
	u_char paddingRight;
	u_char paddingDown;
	u_char paddingLeft;
	u_char spacingHoriz;
	u_char spacingVert;
	u_char outline;

	char* fontName = NULL;//can be ignored
};

struct COMMON
{
	u_short lineHeight;
	u_short base;
	u_short scaleW;
	u_short scaleH;
	u_short pages;

	char reserved : 7;
	char packet : 1;

	u_char alphaChnl;
	u_char redChnl;
	u_char greenChnl;
	u_char blueChnl;
};

struct PAGES
{
	char* pageNames = NULL;
	G_SPRITE fontBitmap;
};

struct CHARS
{
	u_int id;
	u_short x;
	u_short y;
	u_short width;
	u_short height;
	u_short xoffset;
	u_short yoffset;
	u_short xadvance;
	u_char page;
	u_char chnl;
};

struct KERNING
{
	u_int first;
	u_int second;
	short amount;
};

struct FONT
{
	INFO info;
	COMMON common;

	u_short nPages;
	PAGES *pages;

	u_short nChars;
	CHARS *chars;

	u_short nKerning;
	KERNING *kerning;
};

#pragma pack(pop)



int clearFrameBuffer(G_PIXELSURFACE backbuffer, G_COLOR32 color = { 0,0,0,0 });

int displayBuffer(G_PIXELSURFACE frameBuffer_t, G_WINDOW window_t, bool resizeAndStretch = false);

int allocateBufferMemory(G_PIXELSURFACE *buffer, int width, int height);

int deallocateBufferMemory(G_PIXELSURFACE* frameBuffer_t);



int putPixel(G_PIXELSURFACE frameBuffer, Vector2D<> position, G_COLOR32 color, bool clipping = false);

inline void putPixelFast_nonalphablend(G_PIXELSURFACE *frameBuffer, int posx, int posy, G_COLOR32 color);

G_COLOR32  getPixel(G_PIXELSURFACE frameBuffer, Vector2D<int> position, bool clipping = false);


G_SPRITE loadSprite(const char* filename);

int drawSprite(G_PIXELSURFACE backBuffer, G_SPRITE* sprite, Vector2D<> pos, Vector2D<> size, float teta);

int DrawRect(G_PIXELSURFACE frameBuffer, std::pair<Vector2D<>, Vector2D<>> corners, G_COLOR32 color);
void DrawRect(G_PIXELSURFACE frameBuffer, Vector2D<> XYmin, Vector2D<> XYmax);

int DrawCircle(G_PIXELSURFACE backBuffer, Vector2D<> pos, float rad, G_COLOR32 color, bool fill);

int DrawLine(G_PIXELSURFACE backBuffer, Vector2D<> pos1, Vector2D<> pos2, G_COLOR32 color);

void displayPolygon(G_PIXELSURFACE backBuffer, std::vector<Vector2D<>> nodes, Vector2D<> pos, G_COLOR32 vertexColor, G_COLOR32 edgeColor, float vertexRadius, float normalLength);


void draw_BlueBackgound_and_GreenGround(G_PIXELSURFACE *surface, G_WINDOW *window);


void drawSphere(G_PIXELSURFACE *frameBuffer,Vector3D<> pos, CAMERA *camera,float radius, G_COLOR32 color, Vector3D<> lightRay);

void cameraInit(CAMERA *camera);

Vector2D<> Project3D(G_WINDOW *gameWindow, FONT*arial, G_PIXELSURFACE *frameBuffer, Vector3D<> point, CAMERA* camera);


//void draw3DTriangle(G_PIXELSURFACE *frameBuffer, Vector3D<> cameraPos, Vector3D<> Ck, Vector3D<> Cn, Vector3D<> Cm, G_TRIANGLE triangle, G_COLOR32 color);

float Q_rsqrt(float number);

void draw3DTriangleMaster(G_PIXELSURFACE *frameBuffer, CAMERA* camera, G_TRIANGLE triangle, std::vector<Vector3D<float>> *points, Vector3D<> positionOffset, G_COLOR32 color, std::vector<POINT_LIGHT> *pointLightSource, std::vector<DIRECTIONAL_LIGHT> *directionalLightSource, float glossiness);

//void draw3DTriangle(G_PIXELSURFACE *frameBuffer, CAMERA* camera, G_TRIANGLE triangle, std::vector<Vector3D<float>> *points, Vector3D<> offset, G_COLOR32 color, LIGHTSOURCE *lightRay);

bool loadFont(FONT* font, const char* filename);


void drawFontChar(G_PIXELSURFACE *frameBuffer, FONT* font, u_int index, Vector2D<> pos);

void printText(G_PIXELSURFACE *frameBuffer, FONT* font, Vector2D<> pos_t, std::string text);


void draw3DLine(G_PIXELSURFACE *backBuffer, CAMERA *camera, Vector3D<> p1, Vector3D<> p2, G_COLOR32 color);

void drawNormals(G_PIXELSURFACE *backBuffer, CAMERA *camera, OBJECT3D *obj, G_COLOR32 color, float length);