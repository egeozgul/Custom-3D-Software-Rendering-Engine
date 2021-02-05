#include "Input.h"



void GInput_Init(G_INPUT* ginput_t)
{
	G_INPUT a;
	*ginput_t = a;
	return;
}

void GetClientAreaCoordinates(int *x1, int *y1, int *x2, int *y2) 
{
	HWND hWnd = GetActiveWindow();
	RECT rect = { NULL };
	GetClientRect(hWnd, &rect);

	POINT pointUL;
	pointUL.x = rect.left;
	pointUL.y = rect.top;

	ClientToScreen(hWnd, &pointUL);

	POINT pointBR;
	pointBR.x = rect.right;
	pointBR.y = rect.bottom;
	
	ClientToScreen(hWnd,&pointBR);
	
	*x1 = pointUL.x;//rect.left;
	*y1 = pointUL.y;//rect.top;

	*x2 = pointBR.x;//rect.right;
	*y2 = pointBR.y;//rect.bottom;
}

void GetMessages(G_INPUT *input, UINT data, WPARAM wParam, LPARAM LParam)
{
	switch (data)
	{
	case WM_KEYDOWN:
	{
		input->keyboard.IsKbHit = true;
		input->keyboard.keyBuffer[wParam + ' '] = KeyDown;
		input->keyboard.lastPressed = wParam;

		switch (wParam)
		{
		case VK_RETURN:
		{
			input->keyboard.Enter = true;
		}break;
		case VK_SHIFT:
		{
			input->keyboard.Shift = true;
		}break;
		case VK_CONTROL:
		{
			input->keyboard.Ctrl = true;
		}break;

		case VK_ESCAPE:
		{
			input->keyboard.Esc = true;
		}break;
		case VK_SPACE:
		{
			input->keyboard.Space = true;
		}break;
		case VK_LEFT:
		{
			input->keyboard.ArrowLeft = true;
		}break;
		case VK_RIGHT:
		{
			input->keyboard.ArrowRight = true;
		}break;
		case VK_UP:
		{
			input->keyboard.ArrowUp = true;
		}break;
		case VK_DOWN:
		{
			input->keyboard.ArrowDown = true;
		}break;
		}
	}break;

	case WM_KEYUP:
	{
		input->keyboard.IsKbHit = false;
		input->keyboard.keyBuffer[wParam + ' '] = KeyUp;

		switch (wParam)
		{
		case VK_RETURN:
		{
			input->keyboard.Enter = false;
		}break;
		case VK_SHIFT:
		{
			input->keyboard.Shift = false;
		}break;
		case VK_CONTROL:
		{
			input->keyboard.Ctrl = false;
		}break;

		case VK_ESCAPE:
		{
			input->keyboard.Esc = false;
		}break;
		case VK_SPACE:
		{
			input->keyboard.Space = false;
		}break;
		case VK_LEFT:
		{
			input->keyboard.ArrowLeft = false;
		}break;
		case VK_RIGHT:
		{
			input->keyboard.ArrowRight = false;
		}break;
		case VK_UP:
		{
			input->keyboard.ArrowUp = false;
		}break;
		case VK_DOWN:
		{
			input->keyboard.ArrowDown = false;
		}break;
		}
	}break;

	case WM_MOUSEMOVE:
	{
		input->mouse.currentPos.x = (float)(GET_X_LPARAM(LParam));
		input->mouse.currentPos.y = (float)(GET_Y_LPARAM(LParam));
		
		int x1, y1 ,x2 ,y2;
		GetClientAreaCoordinates(&x1,&y1, &x2, &y2);
		int windowCenterX = (x1 + x2 )/ 2;
		int windowCenterY = (y1 + y2) / 2;
		SetCursorPos(windowCenterX, windowCenterY);
	}break;

	case WM_MOUSEHWHEEL:
	{
		input->mouse.Wheel_Delta_Pos = GET_WHEEL_DELTA_WPARAM(wParam);
	}break;

	case WM_MOUSELEAVE:
	{
		input->mouse.IsInWindow = false;
	}break;

	case WM_MOUSEACTIVATE:
	{
		input->mouse.IsInWindow = true;
	}break;

	case	WM_LBUTTONDOWN://	Left button down
	{
		input->mouse.LeftButton = true;
	}break;
	case	WM_LBUTTONUP:	//Left button up
	{
		input->mouse.LeftButton = false;
	}break;
	case	WM_MBUTTONDOWN://	Middle button down
	{
		input->mouse.MiddleButton = true;
	}break;
	case	WM_MBUTTONUP:	//Middle button up
	{
		input->mouse.MiddleButton = false;
	}break;
	case	WM_RBUTTONDOWN:	//Right button down
	{
		input->mouse.RightButton = true;
	}break;
	case	WM_RBUTTONUP:	//Right button up
	{
		input->mouse.RightButton = false;
	}break;
	}
}
