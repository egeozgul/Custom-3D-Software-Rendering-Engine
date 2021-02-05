#pragma once

#include "commonTools.h"

Vector3D<> vector3D(float x, float y, float z);


//x,y,z,d ;z is up, d is color
Vector3D<float> waveFunction(float x, float y, float time, WAVE* wave, int nWave);

void simulateWave(float time, WAVE*wave, int height, int width, int nWave, float seperation, OBJECT3D *oceanWave);

void createWaveSurface(WAVE* wave, int nWave, int width, int height, float seperation, OBJECT3D *objectWave, G_COLOR32 color);

int scanAndFindNeighborTriangles(OBJECT3D *object);
bool calculateAreaOfTriangles(OBJECT3D *obj);

float areaOfTriangle(Vector3D<> p1, Vector3D<> p2, Vector3D<> p3);


void calculateBouyantForceAndTorque(float *resultantForce, Vector3D<> *resultantTorque, OBJECT3D *boat, OBJECT3D *oceanWave, WAVE*wave, int nWave, float time);