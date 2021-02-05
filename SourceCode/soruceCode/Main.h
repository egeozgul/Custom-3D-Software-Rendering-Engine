#pragma once
#pragma comment(linker,  "/STACK:20000000000") //to set limit of stack memory in bytes
//#pragma comment(linker, "/HEAP:200000000") //to set limit of heap memory in bytes

//library for windows audio stuff: not used yet
//#pragma comment(lib, "winmm.lib")

//standard libraries
#define NOMINMAX

#include <windows.h>
#include <stdint.h>
#include <tchar.h>
#include <math.h>
#include <vector>
#include <mmsystem.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include<stdio.h>
#include<string>
#include <limits>


//utility header files
#include "Window.h"		//about preparing a window
#include "Input.h"		
#include "Graphics.h"	
#include "Physics.h"
#include "commonTools.h"
#include "OceanWave.h"

void updateCameraInput(G_PIXELSURFACE *frameBuffer, OBJECT3D *boat,G_INPUT *ginput, CAMERA *camera, float walkSpeed);

void displayStats(G_PIXELSURFACE* frameBuffer, FONT *arial, CAMERA* camera, G_TIME * gtime, G_TIME * displayedTime, unsigned long int *lastTime);
Vector3D<> unitVector(Vector3D<> vector);
Vector3D<> vector3D(float x, float y, float z);
Vector3D<> AlignVector_x(Vector3D<> up, Vector3D<> right, Vector3D<> forward, Vector3D<> alignedVector); //align the x to stationalry vector;
bool load3DObject(const char* filename, OBJECT3D *object);
void drawObject(G_PIXELSURFACE *frameBuffer, CAMERA* camera, OBJECT3D *object, std::vector<POINT_LIGHT> *pointLightSource, std::vector<DIRECTIONAL_LIGHT> *directionalLightSource);
void translateObjectOrigin(OBJECT3D* object, Vector3D<> offset);
void resetZBuffer(G_PIXELSURFACE *frameBuffer);

void calculateNormals(OBJECT3D* obj);
void alignObject(OBJECT3D *object, CARTESIANAXIS* cartesianAxis);