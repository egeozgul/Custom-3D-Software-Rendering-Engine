#include "Window.h"

/**
 @width: width of the pixel plane
 @height: height of the pixel plane
 @title_t: title of the window
*/
G_WINDOW createWindow(int width, int height, const char* title_t, HINSTANCE hInstance_t)
{
	G_WINDOW window_t;
	window_t.title = (char*)title_t;

	window_t.clientAreaWidth = width;
	window_t.clientAreaHeight = height;

	WNDCLASS wc = { 0 };

	wc.style = 0;// CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance_t;
	wc.lpszClassName = "Window_Class";

	if (!RegisterClassA(&wc))
	{
		return window_t;
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
	}


	window_t.hadler = CreateWindowEx(0,
		wc.lpszClassName, title_t,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		//WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, window_t.clientAreaWidth + 16, window_t.clientAreaHeight + 39,
		NULL, NULL, hInstance_t, NULL);

	if (window_t.hadler == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return window_t;
	}

	window_t.DeviceContext = GetDC(window_t.hadler);

	return window_t;
}

LARGE_INTEGER  frameEndTime;
LARGE_INTEGER  frameBeginTime;

LARGE_INTEGER  frameDuration;
LARGE_INTEGER  frequency;

//microseconds
void timerInit(G_TIME *gtime)
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&frameBeginTime);

	return;
}

void fixedFPS(G_TIME *gtime, double targetFPS)
{
	//end
	QueryPerformanceCounter(&frameEndTime);

	frameDuration.QuadPart = frameEndTime.QuadPart - frameBeginTime.QuadPart;

	//begin
	QueryPerformanceCounter(&frameBeginTime);


	frameDuration.QuadPart *= 1000000;
	frameDuration.QuadPart /= frequency.QuadPart;

	long delayDuration = ((long)(1000000.0 / targetFPS) - (long)(frameDuration.QuadPart));

	if (delayDuration > 0)
		Sleep(delayDuration / 1000);

	if (gtime != NULL)
	{
		//in microseconds
		gtime->frameDuration = frameDuration.QuadPart;

		if(delayDuration > 0)
			gtime->frameDuration += delayDuration;

		//convert to milliseconds
		gtime->frameDuration /= 1000;

		//calculate fps
		if (gtime->frameDuration != 0)
			gtime->currentFPS = 1000.0f / (float)gtime->frameDuration;

		LARGE_INTEGER proctime = frameEndTime;
		proctime.QuadPart *= 1000000;
		proctime.QuadPart /= frequency.QuadPart;
		proctime.QuadPart /= 1000;

		gtime->processTime = proctime.QuadPart;
	}
}


MSG Msg;
void HandleWindowMessage()
{
	while (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

