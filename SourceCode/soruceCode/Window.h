#pragma once
#include <windows.h>
#include<stdint.h>
#include "commonTools.h"


struct G_WINDOW
{
	int clientAreaWidth;
	int clientAreaHeight;
	char* title;
	HWND hadler;
	HDC DeviceContext;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

G_WINDOW createWindow(int width, int height, const char* title_t, HINSTANCE hInstance_t);

void HandleWindowMessage();


struct G_TIME
{
	float currentFPS;
	unsigned int frameDuration;
	unsigned long int processTime;
};

void timerInit(G_TIME *gtime);

void fixedFPS(G_TIME *gtime, double targetFPS = 90);
