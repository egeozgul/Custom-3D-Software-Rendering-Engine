
#include"Graphics.h"
#include <math.h>
#include <algorithm>


int clearFrameBuffer(G_PIXELSURFACE frameBuffer, G_COLOR32 color)
{
	if (frameBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	unsigned int fillColor = (color.b | (color.g << 8) | (color.r << 16) | (color.a << 24));

	memset(frameBuffer.memory, fillColor, frameBuffer.size);

	return FUNCTION_SUCCESS;
}

int displayBuffer(G_PIXELSURFACE frameBuffer, G_WINDOW window_t, bool resizeAndStretch)
{
	if (frameBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	int nResult;

	if (resizeAndStretch == true)
	{
		nResult = StretchDIBits(window_t.DeviceContext,
			0, 0, window_t.clientAreaWidth, window_t.clientAreaHeight,
			0, 0, frameBuffer.width, frameBuffer.height,
			frameBuffer.memory, &frameBuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		if (nResult == 0)
			return FUNCTION_ERROR;
	}

	nResult = StretchDIBits(window_t.DeviceContext,//GetDC(NULL),
		0, 0, frameBuffer.width, frameBuffer.height,
		0, 0, frameBuffer.width, frameBuffer.height,
		frameBuffer.memory, &frameBuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	if (nResult == 0)
		return FUNCTION_ERROR;

	return FUNCTION_SUCCESS;
}

int allocateBufferMemory(G_PIXELSURFACE *frameBuffer, int width, int height)
{
	int bytePerPixel = 4;

	frameBuffer->width = width;
	frameBuffer->height = height;
	frameBuffer->size = width * height * bytePerPixel;
	frameBuffer->memory = (int*)VirtualAlloc(0, frameBuffer->size, MEM_COMMIT, PAGE_READWRITE);

	if (frameBuffer->memory == NULL)
		return FUNCTION_ALLOCATION_ERROR;

	frameBuffer->BitmapInfo = { 0 };

	frameBuffer->BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);// BitmapInfo.bmiHeader);
	frameBuffer->BitmapInfo.bmiHeader.biWidth = width;
	frameBuffer->BitmapInfo.bmiHeader.biHeight = -height;//this is negative to render it topdown
	frameBuffer->BitmapInfo.bmiHeader.biPlanes = 1;
	frameBuffer->BitmapInfo.bmiHeader.biBitCount = bytePerPixel * 8;
	frameBuffer->BitmapInfo.bmiHeader.biCompression = BI_RGB;

	return FUNCTION_SUCCESS;;
}

int deallocateBufferMemory(G_PIXELSURFACE* frameBuffer)
{
	if (frameBuffer == NULL)
		return FUNCTION_INVALID_MEMORY;

	VirtualFree((void*)frameBuffer->memory, 0, MEM_RELEASE);

	return FUNCTION_SUCCESS;
}


static struct BitmapFileHeader
{
	u_int fileSize;
	u_short reserved1, reserved2;
	u_int offsetToPixelData;
};

static struct BitmapInfoHeader
{
	unsigned int headerSize;
	int width, height;
	unsigned short planes;
	unsigned short bits;
	unsigned int compression;
	unsigned int imageSize;
	int xResolution, yResolution;
	unsigned int nColors;
	unsigned int importantColours;
};

G_SPRITE loadSprite(const char* filename)
{
	G_SPRITE sprite_t;

	FILE* bmpFile;
	fopen_s(&bmpFile, filename, "rb");

	char signature[2];
	fread(signature, sizeof(char), 2, bmpFile);

	BitmapFileHeader fileHeader;
	fread(&fileHeader, sizeof(fileHeader), 1, bmpFile);

	BitmapInfoHeader infoHeader;
	fread(&infoHeader, sizeof(infoHeader), 1, bmpFile);

	fseek(bmpFile, fileHeader.offsetToPixelData, SEEK_SET);

	sprite_t.height = infoHeader.height;
	sprite_t.width = infoHeader.width;

	sprite_t.memory = (G_COLOR32*)malloc(sprite_t.height* sprite_t.width * sizeof(G_COLOR32));

	fread(sprite_t.memory, sizeof(G_COLOR32), sprite_t.height* sprite_t.width, bmpFile);

	fclose(bmpFile);

	return sprite_t;
}


int drawSprite(G_PIXELSURFACE frameBuffer, G_SPRITE* sprite, Vector2D<> pos, Vector2D<> size, float teta)
{
	if (frameBuffer.memory == NULL || sprite->memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	if (teta == 0)
	{
		for (int iy = 0; iy < sprite->height; iy++)
		{
			for (int ix = 0; ix < sprite->width; ix++)
			{
				G_COLOR32 pixel = sprite->memory[ix + (sprite->height - 1 - iy) * sprite->height];

				Vector2D<> pos2 = { pos.x + ix - sprite->width / 2, pos.y + iy - sprite->height / 2 + 2 };

				putPixel(frameBuffer, pos2, pixel, true);
			}
		}

		return FUNCTION_SUCCESS;
	}
	else if (teta == 180)
	{
		for (int iy = 0; iy < sprite->height; iy++)
		{
			for (int ix = 0; ix < sprite->width; ix++)
			{
				G_COLOR32 pixel = sprite->memory[ix + (iy)* sprite->height];

				Vector2D<> pos2 = { pos.x + ix - sprite->width / 2, pos.y + iy - sprite->height / 2 + 2 };

				putPixel(frameBuffer, pos2, pixel, true);
			}
		}

		return FUNCTION_SUCCESS;
	}


	float diagonal = size.abs();
	float tangentAngle = atanf(size.y / size.x);

	Vector2D<> A = { 0,0 };
	Vector2D<> B = { size.y*cosf(PI / 2 + teta),-size.y * sinf(PI / 2 + teta) };
	Vector2D<> C = { diagonal*cosf(tangentAngle + teta), -diagonal * sinf(teta + tangentAngle) };
	Vector2D<> D = { size.x*cosf(teta), -size.x * sinf(teta) };

	Vector2D<> minVertx;
	minVertx.x = min(A.x, min(B.x, min(C.x, D.x)));
	minVertx.y = min(A.y, min(B.y, min(C.y, D.y)));

	Vector2D<> maxVertx;
	maxVertx.x = max(A.x, max(B.x, max(C.x, D.x)));
	maxVertx.y = max(A.y, max(B.y, max(C.y, D.y)));

	for (float ty = minVertx.y; ty < maxVertx.y; ty++)
	{
		Vector2D<> M;
		float txMin;
		float txMax;

		if (ty + (pos.y - C.y / 2) < 0)
		{
			continue;
		}
		else if (ty + (pos.y - C.y / 2) >= frameBuffer.height)
		{
			break;
		}
		else
		{
			M.y = ty;
		}

		for (txMin = minVertx.x; txMin < maxVertx.x; txMin++)
		{
			M.x = txMin;

			if ((M.y - D.y)*(D.x) < (D.y)*(M.x - D.x) &&
				(M.y - B.y)*(D.x) > (D.y)*(M.x - B.x) &&
				(M.y - B.y)*(B.x) > (B.y)*(M.x - B.x) &&
				(M.y - D.y)*(B.x) < (B.y)*(M.x - D.x))
			{
				if (txMin + (pos.x - C.x / 2) >= 0)
					break;
			}
		}

		for (txMax = maxVertx.x; txMax > minVertx.x; txMax--)
		{
			M.x = txMax;

			if ((M.y - D.y)*(D.x) < (D.y)*(M.x - D.x) &&
				(M.y - B.y)*(D.x) > (D.y)*(M.x - B.x) &&
				(M.y - B.y)*(B.x) > (B.y)*(M.x - B.x) &&
				(M.y - D.y)*(B.x) < (B.y)*(M.x - D.x))
			{
				if (txMax + (pos.x - C.x / 2) < frameBuffer.width)
					break;
			}
		}

		for (float i = txMin; i <= txMax; i++)
		{
			int yy = 0.5f + (int)(abs(-D.y*i + D.x*M.y)*((float)sprite->height / size.y) / size.x);
			int xx = 0.5f + (int)(abs(-B.y*i + B.x*M.y)*((float)sprite->width / size.x) / size.y);

			G_COLOR32 pix = sprite->memory[xx + yy * sprite->width];

			Vector2D<> pixLoc;
			pixLoc.x = 0.5f + i + (pos.x - C.x / 2);
			pixLoc.y = 0.5f + M.y + (pos.y - C.y / 2);

			if (pix.g > 50)
			{
				int tt = 0;
			}
			putPixel(frameBuffer, pixLoc, pix, false);
		}
	}

	return FUNCTION_SUCCESS;
}

int putPixel(G_PIXELSURFACE frameBuffer, Vector2D<> position, G_COLOR32 color, bool clipping)
{
	if (isnan<float>(position.y) || isnan<float>(position.x))
		return -1;

	//position.y = frameBuffer.height - position.y;

	if (clipping)
	{
		if (position.x < 0 || position.y < 0 || position.y >= frameBuffer.height || position.x >= frameBuffer.width)
			return FUNCTION_SUCCESS;
	}

	int baseC = frameBuffer.memory[(int)position.x + (int)position.y * frameBuffer.width];

	unsigned char baseBlue = baseC >> 0;
	unsigned char baseGreen = baseC >> 8;
	unsigned char baseRed = baseC >> 16;
	//unsigned char baseAlpha = baseC >> 24;

	color.b = (color.a)*(color.b - baseBlue) / 255 + baseBlue; // ((color.b)*(color.a) + (baseBlue)*(255 - color.a)) / (255);
	color.g = (color.a)*(color.g - baseGreen) / 255 + baseGreen;//((color.g)*(color.a) + (baseGreen)* (255 - color.a)) / (255);
	color.r = (color.a)*(color.r - baseRed) / 255 + baseRed;//((color.r)*(color.a) + (baseRed) * (255 - color.a)) / (255);

	frameBuffer.memory[(int)position.x + (int)position.y * frameBuffer.width] = (color.b | (color.g << 8) | (color.r << 16) | (color.a << 24)); //b, g, r, a
	
	return FUNCTION_SUCCESS;
}


inline void putPixelFast_nonalphablend(G_PIXELSURFACE *frameBuffer, int posx, int posy, G_COLOR32 color)
{
	//if(frameBuffer.memory[posx + posy * frameBuffer.width] == 0)
	frameBuffer->memory[posx + posy * frameBuffer->width] = (color.b | (color.g << 8) | (color.r << 16) | (color.a << 24));  //b, g, r, a

	return;
}

G_COLOR32  getPixel(G_PIXELSURFACE frameBuffer, Vector2D<int> position, bool clipping)
{
	position.y = frameBuffer.height - position.y;
	G_COLOR32 c = { 0,0,0,0 };

	if (position.x < 0 || position.y < 0 || position.y >= frameBuffer.height || position.x >= frameBuffer.width)
		return c;

	unsigned int k = frameBuffer.memory[position.x + position.y * frameBuffer.width];//b, g, r, a

	c.b = k;
	c.g = k >> 8;
	c.r = k >> 16;
	c.a = k >> 24;

	return c;
}



int DrawRect(G_PIXELSURFACE frameBuffer, std::pair<Vector2D<>, Vector2D<>> corners, G_COLOR32 color)
{
	if (frameBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	if (corners.first.x < 0)
		corners.first.x = 0;

	if (corners.first.y < 0)
		corners.first.y = 0;

	if (corners.second.x > frameBuffer.width - 1)
		corners.second.x = frameBuffer.width - 1;

	if (corners.second.y > frameBuffer.height - 1)
		corners.second.y = frameBuffer.height - 1;

	for (int cY = corners.first.y; cY <= corners.second.y; cY++)
		for (int cX = corners.first.x; cX <= corners.second.x; cX++)
		{
			frameBuffer.memory[cX + frameBuffer.width * (frameBuffer.height - cY - 1)] = (color.b | (color.g << 8) | (color.r << 16) | (color.a << 24));
		}
	return FUNCTION_SUCCESS;
}

void DrawRect(G_PIXELSURFACE frameBuffer, Vector2D<> MinCorner, Vector2D<> MaxCorner)
{
	DrawLine(frameBuffer, MinCorner, { MaxCorner.x,MinCorner.y }, { 255,255,255,255 });
	DrawLine(frameBuffer, MinCorner, { MinCorner.x,MaxCorner.y }, { 255,255,255,255 });
	DrawLine(frameBuffer, MaxCorner, { MinCorner.x,MaxCorner.y }, { 255,255,255,255 });
	DrawLine(frameBuffer, MaxCorner, { MaxCorner.x,MinCorner.y }, { 255,255,255,255 });
}

int DrawCircle(G_PIXELSURFACE backBuffer, Vector2D<> pos, float rad, G_COLOR32 color, bool fill)
{
	if (backBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	if (rad <= 0.0f)
		return FUNCTION_SUCCESS;

	int range = 5 * rad;

	if (fill)
	{
		for (int tx = 0; tx <= range; tx++)
		{
			int ty = 0.5 + sqrt(rad*rad - tx * tx);

			for (float ix = -tx + pos.x; (ix <= tx + pos.x); ix++)
			{
				putPixel(backBuffer, { ix, ty + pos.y }, color, true);
				putPixel(backBuffer, { ix, -ty + pos.y }, color, true);
			}

			for (float ix = -ty + pos.x; (ix <= ty + pos.x); ix++)
			{
				putPixel(backBuffer, { ix, tx + pos.y }, color, true);
				putPixel(backBuffer, { ix, -tx + pos.y }, color, true);
			}
		}
	}
	else
	{
		for (float tx = 0; tx <= range; tx++)
		{
			float ty = sqrt(rad*rad - tx * tx);

			putPixel(backBuffer, { tx + pos.x, ty + pos.y }, color, true);
			putPixel(backBuffer, { -tx + pos.x, ty + pos.y }, color, true);

			putPixel(backBuffer, { tx + pos.x, -ty + pos.y }, color, true);
			putPixel(backBuffer, { -tx + pos.x, -ty + pos.y }, color, true);

			putPixel(backBuffer, { ty + pos.x, tx + pos.y }, color, true);
			putPixel(backBuffer, { -ty + pos.x, tx + pos.y }, color, true);

			putPixel(backBuffer, { ty + pos.x, -tx + pos.y }, color, true);
			putPixel(backBuffer, { -ty + pos.x, -tx + pos.y }, color, true);
		}
	}
	return FUNCTION_SUCCESS;
}

int DrawLine(G_PIXELSURFACE backBuffer, Vector2D<> pos1, Vector2D<> pos2, G_COLOR32 color)
{
	if (backBuffer.memory == NULL)
		return FUNCTION_INVALID_MEMORY;

	float slope = (pos1.y - pos2.y) / (pos1.x - pos2.x);

	if (slope == INFINITY)
		return -1;
	if (slope == INFINITE)
		return -1;

	G_COLOR32 result = color;
	if (slope > 1 || slope < -1)
	{
		//iterate by y axis
		float iy = min(pos1.y, pos2.y);
		float iyMax = max(pos1.y, pos2.y);

		for (; iy < iyMax; iy++)
		{
			float valX = 0.5f + pos1.x + (iy - pos1.y) / slope;
			float diff = 255.0f*(valX - floor(valX));

			result.a = (unsigned char)(diff);

			putPixel(backBuffer, { valX + 1,iy }, result, true);

			result.a = (unsigned char)(255 - diff);
			putPixel(backBuffer, { valX,iy }, result, true);
		}
	}
	else
	{
		//terate by x axis
		float ix = min(pos1.x, pos2.x);
		float ixMax = max(pos1.x, pos2.x);

		for (; ix < ixMax; ix++)
		{
			float valY = 0.5f + (ix - pos1.x)*slope + pos1.y;
			float diff = 255.0f*(valY - floor(valY));

			result.a = (unsigned char)(diff);
			putPixel(backBuffer, { ix, valY + 1 }, result, true);
			
			result.a = (unsigned char)(255 - diff);
			putPixel(backBuffer, { ix, valY }, result, true);
		}
	}

	return FUNCTION_SUCCESS;
}

void drawTriangle0(G_PIXELSURFACE frameBuffer, Vector2D<> p1, Vector2D<> p2, Vector2D<> p3, G_COLOR32 color)
{
	DrawLine(frameBuffer, p1, p2, color);
	DrawLine(frameBuffer, p2, p3, color);
	DrawLine(frameBuffer, p1, p3, color);
}

bool getIntersection(Vector2D<> *result, Vector2D<> l1p1_constant, Vector2D<> l1p2, Vector2D<> l2p1, Vector2D<> l2p2,float slope2)
{
	float x, y;
	if ((l1p2.x - l1p1_constant.x != 0) && (l2p2.x - l2p1.x != 0))
	{
		float slope1 = ((l1p2.y - l1p1_constant.y) / (l1p2.x - l1p1_constant.x));
		//float slope2 = ((l2p2.y - l2p1.y) / (l2p2.x - l2p1.x));
		if (slope2 != slope1) {
			x = (-slope1 * l1p1_constant.x + l1p1_constant.y - l2p1.y + slope2 * l2p1.x) / (slope2 - slope1);
			y = slope1 * (x - l1p1_constant.x) + l1p1_constant.y;
		}
		else
			return false;//doðrular çakýþýk
	}
	else
	{
		if ((l1p2.x - l1p1_constant.x == 0) && (l2p2.x - l2p1.x != 0))
		{
			//float slope2 = ((l2p2.y - l2p1.y) / (l2p2.x - l2p1.x));
			x = l1p1_constant.x;
			y = slope2 * (x - l2p1.x) + l2p1.y;
		}
		else if ((l2p2.x - l2p1.x == 0) && (l1p2.x - l1p1_constant.x != 0))
		{
			float slope1 = ((l1p2.y - l1p1_constant.y) / (l1p2.x - l1p1_constant.x));
			x = l2p1.x;
			y = slope1 * (x - l1p1_constant.x) + l1p1_constant.y;
		}
		else
			return false;//doðrular çakýþýk
	}

	*result = { x, y };
	return true;
}

union
{
	int i;
	float x;
}u;
inline float fastSqrt(float var)
{
	u.x = var;
	u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
	return u.x;
}


//a and b are the base vertices
void DrawHalfTriangle(G_PIXELSURFACE *framebuffer, CAMERA* camera, G_TRIANGLE *triangle_3D,float dist[], Vector2D<> triangle2d[],Vector3D<> triangle3d[], Vector3D<> positionOffset,Vector2D<> a, Vector2D<> b, Vector2D<> c, G_COLOR32 color)
{
	int triangleColor = (color.b | (color.g << 8) | (color.r << 16) | (color.a << 24));

	//y = m*x + n

	//define the line on the left side
	float slope_Left = ((float)(a.y - c.y)) / ((float)(a.x - c.x));
	float yIntercept_Left = c.y - (c.x * slope_Left);

	//define the line on the right side
	float slope_Right = ((float)(b.y - c.y)) / ((float)(b.x - c.x));
	float yIntercept_Right = c.y - (c.x * slope_Right);

	//x offset of the starting and ending points of the horizontal rendering lines
	int tx1 = min(a.x, b.x);
	int tx2 = max(a.x, b.x);

	//y offsets of the top and bottom horizontal rendering lines.
	int Ymin = min(a.y, c.y);
	int Ymax = max(a.y, c.y);

	//vertical clipping
	if (Ymin < 0)
		Ymin = 0;
	if (Ymax > framebuffer->height - 1)
		Ymax = framebuffer->height - 1;

	//the triangle is be rendered from bottom to up
	for (int y = Ymin; y < Ymax; y++)
	{
		if (a.x != c.x)
		{
			tx1 = ((y - yIntercept_Left) / slope_Left);
		}
		else if(a.x == c.x)
		{
			tx1 = c.x;// c.x;
		}
		else
			continue;
		
		if (b.x != c.x)
		{
			tx2 = ((y - yIntercept_Right) / slope_Right);
		}
		else if (b.x == c.x)
		{
			tx2 = c.x;// c.x;
		}
		else
			continue;
		
		//horizontal clipping
		if (tx2 > framebuffer->width - 1)
			tx2 = framebuffer->width - 1;
		if (tx1 < 0)
			tx1 = 0;
		
		Vector2D<> p1 = (triangle2d[1] - triangle2d[2]);
		float p1distance = Q_rsqrt(p1.x*p1.x + p1.y*p1.y);
		//intel password: yyK&hi#wyr6
		Vector2D<> currentPixel, intersectionA;

		float slope2 = ((triangle2d[1].y - triangle2d[2].y) / (triangle2d[1].x - triangle2d[2].x));
		
		float minimumDist = min(dist[0], min(dist[1], dist[2]));
		int zBufferYindex = y * framebuffer->width;

		//z_buffering is applied
		for (int x = tx1; x <= tx2; x++)
		{
			if (minimumDist > framebuffer->zBuffer[x + zBufferYindex])
				continue;

			currentPixel.x = x;
			currentPixel.y = y;

			//l1p1_constant, l1p2, l2p1, l2p2
			if (!getIntersection(&intersectionA, currentPixel, triangle2d[0], triangle2d[2], triangle2d[1], slope2))
			{
				//problem here
				continue;
			}

			Vector2D<> p2 = (triangle2d[0] - intersectionA);

			float  distanceRatio_1_int_2 = (triangle2d[1] - intersectionA).fastAbs() * p1distance;
			
			Vector2D<> temp = triangle2d[0] - currentPixel;
			float distanceRatio_0_pixel_int = fastSqrt((temp.x*temp.x + temp.y*temp.y)/(p2.x*p2.x + p2.y*p2.y));

			float intersectionDistance = dist[1] * (1 - distanceRatio_1_int_2) + dist[2] * distanceRatio_1_int_2;

			//dont know if these are really needed
			//if (distanceRatio_0_pixel_int > 1) 
			//	distanceRatio_0_pixel_int = 1.0f; 
			//if (distanceRatio_1_int_2 > 1)
			//	distanceRatio_1_int_2 = 1.0f;

			float  pixelDistance = dist[0]* (1 - distanceRatio_0_pixel_int)+ intersectionDistance* distanceRatio_0_pixel_int;

			if (pixelDistance > framebuffer->zBuffer[x + zBufferYindex])
				continue;

			framebuffer->zBuffer[x + zBufferYindex] = pixelDistance;
			framebuffer->memory[x + zBufferYindex] = triangleColor;  //b, g, r, a
		}
	}
}

void DrawTriangle(G_PIXELSURFACE *framebuffer, CAMERA* camera, G_TRIANGLE *triangle_3D,Vector3D<> positionOffset, std::vector<Vector3D<float>> *points,Vector2D<> a, Vector2D<>  b, Vector2D<> c, G_COLOR32 color)
{
	int Ax, Ay;//up
	int Bx, By;//middle
	int Cx, Cy;//bottom

	int Dx, Dy;//middle extra
	
	float dist[3];
	dist[0] = ((*points)[triangle_3D->pointIndex[0]] + positionOffset - camera->position).abs();
	dist[1] = ((*points)[triangle_3D->pointIndex[1]] + positionOffset - camera->position).abs();
	dist[2] = ((*points)[triangle_3D->pointIndex[2]] + positionOffset - camera->position).abs();
	
	Vector3D<> triangle3d[3] = {(*points)[triangle_3D->pointIndex[0]] , (*points)[triangle_3D->pointIndex[1]] ,(*points)[triangle_3D->pointIndex[2]] };
	Vector2D<> triangle2d[3] = {a,b,c};


	if (a.y == b.y)
	{
		DrawHalfTriangle(framebuffer, camera, triangle_3D,dist, triangle2d, triangle3d, positionOffset,floatVector(min(a.x, b.x), a.y), floatVector(max(a.x, b.x), a.y), floatVector(c.x, c.y), color);//up triangle
		return;
	}
	else if (b.y == c.y)
	{
		DrawHalfTriangle(framebuffer, camera, triangle_3D, dist, triangle2d, triangle3d, positionOffset,floatVector(min(b.x, c.x), b.y), floatVector(max(b.x, c.x), b.y), floatVector(a.x, a.y), color);//up triangle
		return;
	}
	else if (c.y == a.y)
	{
		DrawHalfTriangle(framebuffer, camera, triangle_3D, dist, triangle2d, triangle3d, positionOffset,floatVector(min(c.x, a.x), c.y), floatVector(max(c.x, a.x), c.y), floatVector(b.x, b.y), color);//up triangle
		return;
	}

	if ((a.y > b.y) && (a.y > c.y)) // A and C done
	{
		Ax = a.x;
		Ay = a.y;

		if (b.y < c.y)
		{
			Cx = b.x;
			Cy = b.y;

			Bx = c.x;
			By = c.y;
		}
		else if ((b.y > c.y))
		{
			Cx = c.x;
			Cy = c.y;
			Bx = b.x;
			By = b.y;
		}
	}
	else if ((b.y > c.y) && (b.y > a.y))
	{
		Ax = b.x;
		Ay = b.y;

		if (a.y < c.y)
		{
			Cx = a.x;
			Cy = a.y;
			Bx = c.x;
			By = c.y;
		}
		else if (a.y > c.y)
		{
			Cx = c.x;
			Cy = c.y;
			Bx = a.x;
			By = a.y;
		}
	}
	else if ((c.y > b.y) && (c.y > a.y))
	{
		Ax = c.x;
		Ay = c.y;
		if (b.y < a.y)
		{
			Cx = b.x;
			Cy = b.y;
			Bx = a.x;
			By = a.y;
		}
		else if (b.y > a.y)
		{
			Cx = a.x;
			Cy = a.y;
			Bx = b.x;
			By = b.y;
		}
	}
	else
	{
		//a,b,x are colinear, so do not attempt to draw a triangle
		return;
	}

	float S = (float)(Ay - Cy) / (float)(Ax - Cx);

	Dy = By;

	if (Ax != Cx && Ay != Cy)
	{
		float N = Ay - (S*Ax);//y = mx + n
		Dx = (int)(((float)(Dy - N)) / S); // x = (y-n)/m
	}
	else
		Dx = Ax;
	
	DrawHalfTriangle(framebuffer, camera, triangle_3D, dist, triangle2d, triangle3d, positionOffset,floatVector(min(Dx, Bx), By), floatVector(max(Dx, Bx), By), floatVector(Ax, Ay), color);
	DrawHalfTriangle(framebuffer, camera, triangle_3D, dist, triangle2d, triangle3d, positionOffset,floatVector(min(Dx, Bx), By), floatVector(max(Dx, Bx), By), floatVector(Cx, Cy), color);
}


void displayPolygon(G_PIXELSURFACE backBuffer, std::vector<Vector2D<>> nodes, Vector2D<> pos, G_COLOR32 vertexColor, G_COLOR32 edgeColor, float vertexRadius, float normalLength)
{
	int nodeSize = nodes.size();

	for (int i = 0; i < nodeSize - 1; i++)
	{
		DrawLine(backBuffer, pos + nodes[i], pos + nodes[i + 1], edgeColor);
	}

	DrawLine(backBuffer, pos + nodes[nodeSize - 1], pos + nodes[0], edgeColor);

	if (vertexRadius > 0)
	{
		DrawCircle(backBuffer, pos + nodes[0], vertexRadius * 2, vertexColor, true);

		for (int i = 1; i < nodeSize; i++)
			DrawCircle(backBuffer, pos + nodes[i], vertexRadius, vertexColor, true);
	}

	if (normalLength > 0)
	{
		for (int i = 0; i < nodeSize - 1; i++)
		{
			Vector2D<> normal = (nodes[i] - nodes[i + 1]).perp_left();
			normal.toUnitVect();
			normal = normal * 10;
			DrawLine(backBuffer, pos + (nodes[i] + nodes[i + 1]) / 2, pos + ((nodes[i] + nodes[i + 1]) / 2) + normal, edgeColor);
		}

		Vector2D<> normal = (nodes[nodeSize - 1] - nodes[0]).perp_left();
		normal.toUnitVect();
		normal = normal * 10;
		DrawLine(backBuffer, pos + (nodes[nodeSize - 1] + nodes[0]) / 2, pos + ((nodes[nodeSize - 1] + nodes[0]) / 2) + normal, edgeColor);

	}
}


void draw_BlueBackgound_and_GreenGround(G_PIXELSURFACE *surface, G_WINDOW *window)
{
	for (int iy = 0; iy < surface->height; iy++)
		for (int ix = 0; ix < surface->width; ix++)
		{
			putPixelFast_nonalphablend(surface, ix, iy, { 0 ,255,140 - (unsigned int)(iy / 5),140 - (unsigned int)(iy/5)});
		}

	return;
	float sonnokta = 11700 * tanf(WORLD_SLOPE);
	for (int i = 1; i < sonnokta; i++)
	{
		for (int j = 1; j < window->clientAreaWidth; j++)
		{
			//100 + (int)(155 * i / sonnokta)
			putPixelFast_nonalphablend(surface, j, window->clientAreaHeight - i, { 100 + (unsigned int)(155 * (sonnokta - i) / sonnokta),0,255,0 });
		}
	}
	for (int i = sonnokta; i < window->clientAreaHeight; i++)
	{
		for (int j = 1; j < window->clientAreaWidth; j++)
		{
			//100 + (int)(155 * i / sonnokta)
			putPixelFast_nonalphablend(surface, j, window->clientAreaHeight - i, { 100 + (unsigned int)(155 * (1 - (i - sonnokta) / (window->clientAreaHeight - sonnokta))),255,90,90 });
		}
	}
}


//zooman4tres
Vector2D<float> Project3D(G_WINDOW *gameWindow,FONT*arial,G_PIXELSURFACE *frameBuffer,Vector3D<> point, CAMERA* camera)
{
	Vector3D<float> P = point - camera->position;

	double projectionDistance = camera->forwardVector.dotProduct(P);// / camera->forwardVector.abs();
	
	double d = camera->rightVector.dotProduct(P);// / camera->rightVector.abs();
	double dx = d / projectionDistance;
	//float dx =	d*Fx/P.abs();
	
	double h = camera->upVector.dotProduct(P);// / camera->upVector.abs();
	double dy = h / projectionDistance;
	//float dy = h * Fy / P.abs();
	
	Vector2D<float> result;
	result.x = roundf(frameBuffer->width / 2 + dx * frameBuffer->width / (camera->lensRatio));
	result.y = roundf(frameBuffer->height / 2 + dy *frameBuffer->height / (camera->lensRatio));

	return result;
}

//calculates the inverse squareroot using Newton's approximation technique
float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = *(long *)&y;    // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
	//y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration,
	
	return y;
}


void drawSphere(G_PIXELSURFACE *frameBuffer,Vector3D<> pos, CAMERA *camera,float radius, G_COLOR32 color, Vector3D<> lightRay)
{
	Vector2D<> position2D = Project3D(NULL, NULL, frameBuffer, pos, camera);
	Vector3D<> radVect = camera->upVector;
	radVect.toUnitVect();
	radVect = radVect * radius;
	Vector2D<> position2D_rad = Project3D(NULL, NULL, frameBuffer, pos+ radVect, camera);

	Vector3D<> aa = (pos - camera->position);
	aa.toUnitVect();

	Vector3D<> realNormal;

	Vector3D<> sideVector = -aa.crossProduct(camera->upVector);
	Vector3D<> angledUpvector = sideVector.crossProduct(aa);

	angledUpvector.toUnitVect();
	sideVector.toUnitVect();
	G_COLOR32 shadedColor;

	float screenRadius = (position2D_rad - position2D).abs();

	float shadingFactorConstant = lightRay.abs();//realNormal.abs();


	int yStart = -screenRadius;
	int yLimit = screenRadius;

	if (yStart < -position2D.y)
		yStart = -position2D.y;
	if (yLimit > frameBuffer->height - 1 - position2D.y)
		yLimit = frameBuffer->height - 1 - position2D.y;

	for (int iy = yStart; iy <= yLimit; iy++)
	{
		int linearRad = sqrtf(screenRadius*screenRadius - iy*iy);
		
		
		int xLimit = linearRad;
		
		int xStart = -linearRad;
		if (xStart < -position2D.x)
			xStart = -position2D.x;
		if (xLimit > frameBuffer->width - 1 - position2D.x)
			xLimit = frameBuffer->width - 1 - position2D.x;

		for (int ix = xStart; ix <= xLimit; ix++)
		{
			if (ix % 3 != 0 && (ix != xStart))
			{
				putPixelFast_nonalphablend(frameBuffer, ix + position2D.x, iy + position2D.y, shadedColor);
				continue;
			}

			float iz = sqrtf(screenRadius*screenRadius - ix * ix - iy * iy);
			Vector3D<> screenNormal = { ix / screenRadius,iz / screenRadius,iy / screenRadius };

			realNormal = (sideVector * screenNormal.x) + (angledUpvector * screenNormal.z) - (aa * screenNormal.y);

			//if ((pos - cameraPos).dotProduct(realNormal) < 0) 
			//	continue;

			float shadeFactor = (realNormal.dotProduct(-lightRay));

			if (shadeFactor < 0)
			{
				shadedColor = black;
				putPixelFast_nonalphablend(frameBuffer, ix + position2D.x, iy + position2D.y, shadedColor);
				continue;
			}

			shadeFactor /= shadingFactorConstant; 

			shadedColor = { 255, (unsigned int)(shadeFactor * color.b),(unsigned int)(shadeFactor * color.g),(unsigned int)(shadeFactor * color.r) };

			putPixelFast_nonalphablend(frameBuffer, ix + position2D.x, iy + position2D.y, shadedColor);
		}
	}
}


bool SatForTriangleClipping(Vector2D<> *triangle, int windowWidth, int windowHeight)
{
	if (!(triangle[0].x < 0 && triangle[0].x > windowWidth - 1 && triangle[0].y < 0 && triangle[0].y > windowHeight - 1))
		return true;

	if (!(triangle[1].x < 0 && triangle[1].x > windowWidth - 1 && triangle[1].y < 0 && triangle[1].y > windowHeight - 1))
		return true;

	if (!(triangle[2].x < 0 && triangle[2].x > windowWidth - 1 && triangle[2].y < 0 && triangle[2].y > windowHeight - 1))
		return true;

	Vector2D<> rectangle[4] = { {0,0},{windowWidth - 1,0},{windowWidth - 1,windowHeight - 1},{0,windowHeight - 1} };

	for (int i = 0; i < 3; i++)
	{
		Vector2D<> axisVector = triangle[i] - triangle[(i + 1) % 3];
		axisVector = axisVector * (-1);

		//projection for triangle
		float triangleProjection1 = axisVector.dotProduct(triangle[0]);
		float triangleProjection2 = axisVector.dotProduct(triangle[1]);
		float triangleProjection3 = axisVector.dotProduct(triangle[2]);

		float maxTriangleProjection = max(triangleProjection1, max(triangleProjection2, triangleProjection3));
		float minTriangleProjection = min(triangleProjection1, min(triangleProjection2, triangleProjection3));

		//proection for rectangle
		float rectProjection1 = axisVector.dotProduct(rectangle[0]);
		float rectProjection2 = axisVector.dotProduct(rectangle[1]);
		float rectProjection3 = axisVector.dotProduct(rectangle[2]);
		float rectProjection4 = axisVector.dotProduct(rectangle[3]);

		float maxRectProjection = max(rectProjection1, max(rectProjection2, max(rectProjection3, rectProjection4)));
		float minRectProjection = min(rectProjection1, min(rectProjection2, max(rectProjection3, rectProjection4)));

		//projection comparison test
		if (maxTriangleProjection > minRectProjection && minTriangleProjection < maxRectProjection)
		{
			//there might be collision, but not certainly, so we must keep checking all sides
		}
		else
		{
			//there isnt any collision for sure
			return false;
		}
	}

	return true;
}


void draw3DLine(G_PIXELSURFACE *backBuffer, CAMERA *camera,Vector3D<> p1, Vector3D<> p2, G_COLOR32 color)
{
	Vector2D<> pa = Project3D(NULL,NULL,backBuffer,p1,camera);
	Vector2D<> pb = Project3D(NULL, NULL, backBuffer, p2, camera);

	DrawLine(*backBuffer,pa,pb,color);
	return;
}

void drawNormals(G_PIXELSURFACE *backBuffer, CAMERA *camera, OBJECT3D *obj,G_COLOR32 color, float length)
{
	for (int i = 0;i<obj->triangles.size();i++)
	{
		Vector3D<> point = obj->transformedPoints[obj->triangles[i].pointIndex[0]];
		point = point +  obj->transformedPoints[obj->triangles[i].pointIndex[1]];
		point = point + obj->transformedPoints[obj->triangles[i].pointIndex[2]];
		point = point / 3;

		Vector3D<> p1 = point + obj->position;
		
		Vector3D<> p2;

		if (length > 0)
		{
			Vector3D<> pt = obj->triangles[i].normal;
			pt.toUnitVect();
			pt = pt * length;

			p2 = p1 + pt;
		}
		else
		{
			p2 = p1 + obj->triangles[i].normal;
		}

		draw3DLine(backBuffer,camera,p1,p2,color);
	}
}

Vector2D<float> vertices[3];
void draw3DTriangleMaster(G_PIXELSURFACE *frameBuffer, CAMERA* camera, G_TRIANGLE triangle, std::vector<Vector3D<float>> *points, Vector3D<> positionOffset, G_COLOR32 color, std::vector<POINT_LIGHT> *pointLightSource, std::vector<DIRECTIONAL_LIGHT> *directionalLightSource,float glossiness)
{
	//culling the triangles at the back of the camera
	int cullCounter = 0;
	for (int i = 0; i < 3; i++)
	{
		if (camera->forwardVector.dotProduct((*points)[triangle.pointIndex[i]] + positionOffset - camera->position) > 1)
			cullCounter++;//point is outside
	}
	if (cullCounter == 3)
		return;
	
	vertices[0] = Project3D(NULL, NULL, frameBuffer, (*points)[triangle.pointIndex[0]] + positionOffset, camera);
	vertices[1] = Project3D(NULL, NULL, frameBuffer, (*points)[triangle.pointIndex[1]] + positionOffset, camera);
	vertices[2] = Project3D(NULL, NULL, frameBuffer, (*points)[triangle.pointIndex[2]] + positionOffset, camera);

	//backface culling
	Vector2D<> axis = (vertices[1] - vertices[2]).perp_right();
	if (axis.dotProduct(vertices[0]) > axis.dotProduct(vertices[1]))
		return;

	//culling the triangles outside the client area
	for (int i = 0; i < 3; i++)
	{
		if (!((vertices[i].x < 0) || (vertices[i].x >= frameBuffer->width) || (vertices[i].y < 0) || (vertices[i].y >= frameBuffer->height)))
			goto pass;
	}
	return;
	pass:

	float Cblue = 0;
	float Cred = 0;
	float Cgreen= 0;

	color = triangle.color;

	//point light
	for (int i = 0; i < pointLightSource->size(); i++)
	{
		double shadeFactor = triangle.normal.dotProduct((*pointLightSource)[i].direction);

		float lightDistance = ((*points)[triangle.pointIndex[0]] + positionOffset - (*pointLightSource)[i].position).abs();

		if (shadeFactor < 0)
			shadeFactor = 0.0f;

		shadeFactor *= (*pointLightSource)[i].intensity / lightDistance;

		Cblue += shadeFactor * ((*pointLightSource)[i].color.b*color.b + glossiness * (*pointLightSource)[i].color.b) + (*pointLightSource)[i].ambient *(*pointLightSource)[i].color.b*color.b*(*pointLightSource)[i].intensity / lightDistance;
		Cred += shadeFactor * ((*pointLightSource)[i].color.r*color.r + glossiness * (*pointLightSource)[i].color.r) + (*pointLightSource)[i].ambient *(*pointLightSource)[i].color.r*color.r*(*pointLightSource)[i].intensity / lightDistance;
		Cgreen += shadeFactor * ((*pointLightSource)[i].color.g*color.g + glossiness * (*pointLightSource)[i].color.g) + (*pointLightSource)[i].ambient*(*pointLightSource)[i].color.g*color.g*(*pointLightSource)[i].intensity / lightDistance;
	}

	for (int i = 0; i < directionalLightSource->size(); i++)
	{
		double shadeFactor = triangle.normal.dotProduct((*directionalLightSource)[i].direction);

		if (shadeFactor < 0)
			shadeFactor = 0.0f;

		shadeFactor *= (*directionalLightSource)[i].intensity;

		Cblue += shadeFactor * ((*directionalLightSource)[i].color.b*color.b + glossiness * (*directionalLightSource)[i].color.b) + (*directionalLightSource)[i].ambient *(*directionalLightSource)[i].color.b*color.b*(*directionalLightSource)[i].intensity;
		Cred += shadeFactor * ((*directionalLightSource)[i].color.r*color.r + glossiness * (*directionalLightSource)[i].color.r) + (*directionalLightSource)[i].ambient *(*directionalLightSource)[i].color.r*color.r*(*directionalLightSource)[i].intensity;
		Cgreen += shadeFactor * ((*directionalLightSource)[i].color.g*color.g + glossiness * (*directionalLightSource)[i].color.g) + (*directionalLightSource)[i].ambient*(*directionalLightSource)[i].color.g*color.g*(*directionalLightSource)[i].intensity;
	}


	if (Cblue > 255) Cblue = 255;
	if (Cred> 255) Cred = 255;
	if (Cgreen > 255) Cgreen= 255;
	
	color.r = (unsigned char)Cred;
	color.g = (unsigned char)Cgreen;
	color.b = (unsigned char)Cblue;

	DrawTriangle(frameBuffer, camera, &triangle, positionOffset, points, vertices[0], vertices[1], vertices[2], color);
}

void cameraInit(CAMERA *camera)
{
	camera->position = { 0,0,0 };
	camera->forwardVector = { 0,1,0 };
	camera->rightVector = { 1,0,0 };
	camera->upVector = { 0,0,1 };

	float screenDistance = 0.7;
	float lensAngle = 120.0f;
	camera->lensRatio = screenDistance * tanf(lensAngle  * 180.0f / 3.14);
}


bool loadFont(FONT* font, const char* filename)
{
	FILE* bmpFile;
	fopen_s(&bmpFile, filename, "rb");

	//read the file signature
	char signature[3];
	fread(signature, 1, 3, bmpFile);

	if (!(signature[0] == 'B' && signature[1] == 'M' && signature[2] == 'F'))
	{
		//not correct file format
		fclose(bmpFile);
		return false;
	}

	//read format version number
	char version;
	fread(&version, sizeof(char), 1, bmpFile);

	/*
	Version 1 (introduced with application version 1.8).

	Version 2 (introduced with application version 1.9)
	--->added the outline field in the infoBlock and the encoded field
		in the commonBlock.

	Version 3 (introduced with application version 1.10)
	--->removed the encoded field in the commonBlock, and added
		the alphaChnl, redChnl, greenChnl, blueChnl instead.
		The size of each block is now stored without accounting
		for the size field itself. The character id in
		the charsBlock and the kerningPairsBlock was increased to 4
		bytes to support the full unicode range.
	*/

	if (font == NULL)
	{
		fclose(bmpFile);
		return false;
	}

	//font = (FONT*)malloc(sizeof(FONT));
	char blockInfo;
	int blockSize;

	//INFO
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	fread(&font->info, 14, 1, bmpFile);
	font->info.fontName = (char*)malloc(blockSize - 14);
	fread(font->info.fontName, blockSize - 14, 1, bmpFile);

	//COMMON
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	fread(&font->common, blockSize, 1, bmpFile);

	//PAGES
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	/*
	fpos_t position1,position2;
	fgetpos(bmpFile, &position1);
	char temp;
	do {
		fread(&temp,1,1,bmpFile);
	} while (temp != '\0');

	fgetpos(bmpFile, &position2);

	int fileName = position2 - position1;

	fsetpos(bmpFile, &position1);
	*/

	font->nPages = 1;
	font->pages = (PAGES*)malloc(font->nPages * sizeof(PAGES));
	font->pages[0].pageNames = (char*)malloc(blockSize);
	//for(int i= 0; i<font->nPages; i++)
	fread(font->pages[0].pageNames, blockSize, 1, bmpFile);

	//CHARS
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);
	font->nChars = blockSize / 20;
	font->chars = (CHARS*)malloc(blockSize);
	fread(font->chars, 20, font->nChars, bmpFile);

	//KERNING
	fread(&blockInfo, 1, 1, bmpFile);
	fread(&blockSize, 4, 1, bmpFile);

	font->kerning = (KERNING*)malloc(blockSize);
	font->nKerning = blockSize / 10;
	fread(font->kerning, 10, font->nKerning, bmpFile);

	fclose(bmpFile);

	font->pages[0].fontBitmap.memory = NULL;

	char* name = font->pages[0].pageNames;

	std::string path = filename;
	for (int i = path.size(); i >= 0; i--)
	{
		if (path[i] == '\\')
		{
			break;
		}
		else
		{
			path.erase(i);
		}
	}

	path += name;

	font->pages[0].fontBitmap = loadSprite(path.c_str());
}


void drawFontChar(G_PIXELSURFACE *frameBuffer, FONT* font, u_int index, Vector2D<> pos)
{
	PAGES *page = &font->pages[font->chars[index].page];
	int height = font->chars[index].height;
	int width = font->chars[index].width;

	for (int iy = 0; iy < height; iy++)
	{
		for (int ix = 0; ix < width; ix++)
		{
			int cx = ix + font->chars[index].x;
			int cy = iy + font->chars[index].y;

			G_COLOR32 pixel = page->fontBitmap.memory[cx + (page->fontBitmap.height - cy - 1) * page->fontBitmap.height];

			Vector2D<> pos2 = { pos.x + ix, pos.y + iy };

			//putPixel(buffer, pos2, MakeColor(255, 0, 0, 80), true);

			putPixel(*frameBuffer, pos2, pixel, true);
			if(pixel.a>50)
			frameBuffer->zBuffer[(int)pos2.x + (int)pos2.y*frameBuffer->width] = 0;

		}
	}


}


void printText(G_PIXELSURFACE *frameBuffer, FONT* font, Vector2D<> pos_t, std::string text)
{
	Vector2D<> pos = pos_t;

	u_int charId = 0;
	CHARS kar;
	bool newLine = true;
	for (int i = 0; i < text.size(); i++)
	{
		//check for new line
		if (text[i] == '\n')
		{
			pos.y = pos.y + font->common.base;
			pos.x = pos_t.x;
			newLine = true;
			continue;
		}

		u_int lastCharId = charId;
		charId = (u_int)text[i];

		int index = 0;
		for (; index < font->nChars; index++)
		{
			if (font->chars[index].id == charId)
				break;
		}

		if (index == font->nChars)
		{
			// character not fount
			continue;
		}

		if (i != 0 && newLine == false)
		{
			//do kerning if necessary(if pairs are found)
			for (int ik = 0; ik < font->nKerning; ik++)
			{
				if (font->kerning[ik].first == lastCharId &&
					font->kerning[ik].second == charId)
				{
					pos.x = pos.x + font->kerning[ik].amount;
					break;
				}
			}

			Vector2D<> ad = { kar.xadvance, 0 };
			pos = pos + ad;
		}

		kar = font->chars[index];

		Vector2D<> prec = { kar.xoffset, kar.yoffset };
		drawFontChar(frameBuffer, font, index, pos + prec);
		newLine = false;
	}
}

