#pragma once
#include "commonTools.h"
#include "Graphics.h"
#include <vector>
#include <algorithm> 

void scaleObject(OBJECT3D *object, float scaleFactor);

void rotateVector(Vector3D<> *vect, float horizontalAngle);
bool doSlowly(unsigned long int *startTime, unsigned long int currentTime, float frequency);
bool animateJump(float*altitude, unsigned long startTime, unsigned long currentTime, unsigned long totalDuration, double jumpHeight);
Vector2D<> scanline(G_SPRITE* sprite, Vector2D<> pos1, Vector2D<> center, G_COLOR32 CA, G_COLOR32 CB);

bool isInRect(Vector2D<> A, Vector2D<> B, Vector2D<> C, Vector2D<> D, Vector2D<> M);

std::vector<Vector2D<>> circularConcaveHullScanner(G_SPRITE* sprite, float deltaAngle, bool display_scanlines, G_COLOR32 A, G_COLOR32 B);


std::vector<Vector2D<>> convertConcaveToConvexHull(std::vector<Vector2D<>> Vertices);
Vector2D<> Support(G_OBJECT *object_A, G_OBJECT *object_B, Vector2D<> D);
inline static bool insideTriangle(Vector2D<> *p1, Vector2D<> *p2, Vector2D<> *p3);

bool EEE(G_OBJECT *object_A, G_OBJECT *object_B);

void setPosition(G_OBJECT *object, Vector2D<> newPosition);
void move(G_OBJECT *object, Vector2D<> displacement);


/**
vertices of object_B is checked if any are inside object_A
but it is not checked if any vertex of object_A is inside object_B.
So use the function twice if necessary by swapping the arguments for high precision.
*/
//bool convexHullCollision_SAT(G_OBJECT *object_A, G_OBJECT *object_B);

bool GJK(G_OBJECT *object_A, G_OBJECT *object_B);

void collisionResponse(G_PIXELSURFACE backBuffer, G_OBJECT *object_A, G_OBJECT *object_B);

int findFurthestPoint(std::vector<Vector2D<>> *A, Vector2D<> normal);

Vector2D<> randPos(int, int);

std::vector<G_RECT>  preMergeSort(std::vector<G_RECT> list, Vector3D<> cameraPos);

Vector2D<> make_Vector2D(float x, float y);
//void jump(float* camera);
void drawObj(G_PIXELSURFACE frameBuffer, Vector3D<> position, Vector3D<> cameraPos, std::vector<G_TRIANGLE> triangles);

//std::vector<Vector2D<>> minskowskiSum(std::vector<Vector2D<>> *A, std::vector<Vector2D<>> *B);// B around A

//std::vector<Vector2D<>> minskowskiDifference(std::vector<Vector2D<>> *A, std::vector<Vector2D<>> *B); // B around A

