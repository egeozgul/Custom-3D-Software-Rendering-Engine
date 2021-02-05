#pragma once

#include<Windows.h>
#include<vector>
#include<algorithm>
#include <assert.h>

#define PI 3.14159265359

#define EYE_CONST 800
#define WORLD_SLOPE PI / 144

#define debugPrint(...) char *str = new char[100];  sprintf_s(str,100,__VA_ARGS__ );	OutputDebugString(str);

#define MakeVector(x,y) {(int)x,(int)y}
#define floatVector(x,y) {(float)x,(float)y}

#define eprint(frameBuffer, font,pos, formatString,...)		\
{															\
	char newString[50];										\
	sprintf_s(newString,formatString, __VA_ARGS__);			\
	printText(frameBuffer, font, pos,newString);			\
}															\

#define alert(message) if(MessageBox(NULL,message, "ERROR: click no to abort / yes to continue",MB_OK | MB_ICONEXCLAMATION | MB_YESNO) == IDNO) exit(0);

#define random(min,max) (rand()%((max)-(min)+1) + (min))


#define MakeColor(r,g,b,a) {a,b,g,r}

#define red		MakeColor(255, 0, 0, 255)
#define blue	MakeColor(0, 0, 255, 255)
#define green	MakeColor(0, 255, 0, 255)
#define black	MakeColor(0, 0, 0, 255)
#define white	MakeColor(255, 255, 255, 255)
#define gray	MakeColor(125, 125, 125, 255)
#define warmWhite	MakeColor(160, 125, 125, 255)

#define brown MakeColor(40, 20,10 , 255)

//DrawText(gameWindow.DeviceContext,"hello",NULL,NULL,NULL);
//PlaySoundA((LPCSTR)"turret_firing.wav", NULL, SND_FILENAME | SND_ASYNC);

float Q_rsqrt(float number);


//float sqrt3(const float x);

template<typename Gtype = float>
struct Vector2D
{
	Gtype x = 0;
	Gtype y = 0;

	inline Vector2D operator+(Vector2D rhs)
	{
		return{ rhs.x + this->x, rhs.y + this->y };
	}
	inline Vector2D operator-(Vector2D rhs)
	{
		return{ this->x - rhs.x, this->y - rhs.y };
	}

	inline Vector2D operator-()
	{
		return{ -this->x, -this->y };
	}
	inline Vector2D operator-(Gtype rhs)
	{
		return{ this->x - rhs,this->y - rhs };
	}
	inline Vector2D operator*(Gtype rhs)
	{
		return{ rhs * this->x, rhs * this->y };
	}
	inline Vector2D operator/(Gtype rhs)
	{
		return{ this->x / rhs, this->y / rhs };
	}

	inline bool operator>(Vector2D rhs)
	{
		return this->x > rhs.x &&this->y > rhs.y;
	}
	inline bool operator<(Vector2D rhs)
	{
		return this->x < rhs.x &&this->y < rhs.y;
	}
	inline bool operator>=(Vector2D rhs)
	{
		return this->x >= rhs.x &&this->y >= rhs.y;
	}
	inline bool operator<=(Vector2D rhs)
	{
		return this->x <= rhs.x &&this->y <= rhs.y;
	}

	inline bool operator==(Vector2D rhs)
	{
		return this->x == rhs.x && this->y == rhs.y;
	}

	inline float dotProduct(Vector2D A)
	{
		return A.x*x + A.y*y;
	}

	inline float determinant(Vector2D A)
	{
		return x * A.y - y * A.x;
	}

	inline float square()
	{
		return x * x + y * y;
	}
	
	inline float abs()
	{
		float xa = x * x + y * y;

		union
		{
			int i;
			float x;
		} u;

		u.x = xa;
		u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
		return u.x;

	}

	inline float fastAbs()
	{
		float xa = x * x + y * y;

		union
		{
			int i;
			float x;
		} u;

		u.x = xa;
		u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
		return u.x;

		//return 
	}


	inline Vector2D perp_right()
	{
		return{ -y, x };
	}

	inline Vector2D perp_left()
	{
		return{ y, -x };
	}

	inline Vector2D negative()
	{
		return{ -x, -y };
	}

	inline Vector2D negative_y()
	{
		return{ x, -y };
	}

	inline Vector2D negative_x()
	{
		return{ -x, y };
	}

	inline bool onTheSameSide(Vector2D v)
	{
		if (this->dotProduct(v) > 0)
			return true;
	}

	inline void toUnitVect()
	{
		float len = this->abs();
		x = x / len;
		y = y / len;
	}
};




template<typename Gtype = float>
struct Vector3D
{
	Gtype x = 0;
	Gtype y = 0;
	Gtype z = 0;

	inline Vector3D operator+(Vector3D rhs)
	{
		return{ rhs.x + this->x, rhs.y + this->y, rhs.z + this->z };
	}

	inline Vector3D unitVector()
	{
		float magnitude = this->abs();
		return {this->x/magnitude, this->y / magnitude ,this->z / magnitude };
	}

	inline Vector3D operator-(Vector3D rhs)
	{
		return{ this->x - rhs.x, this->y - rhs.y, rhs.z - this->z };
	}

	inline Vector3D operator-()
	{
		return{ -this->x, -this->y, -this->z };
	}
	inline Vector3D operator*(Gtype rhs)
	{
		return{ rhs * this->x, rhs * this->y, rhs * this->z };
	}
	inline Vector3D operator/(Gtype rhs)
	{
		return{ this->x / rhs, this->y / rhs, this->z / rhs };
	}

	inline bool operator>(Vector3D rhs)
	{
		return this->abs() > rhs.abs();
	}
	inline bool operator<(Vector3D rhs)
	{
		return this->abs() < rhs.abs();
	}
	inline bool operator>=(Vector3D rhs)
	{
		return this->abs() >= rhs.abs();
	}
	inline bool operator<=(Vector3D rhs)
	{
		return this->abs() <= rhs.abs();
	}

	inline bool operator==(Vector3D rhs)
	{
		return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
	}

	inline bool operator!=(Vector3D rhs)
	{
		return !(this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
	}

	inline float abs(bool X = true, bool Y = true, bool Z = true)
	{
		float xa = x * x*X + y * y*Y + z * z*Z;
		
		union
		{
			int i;
			float x;
		} u;

		u.x = xa;
		u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
		return u.x;

		// return sqrtf(x*x*X + y * y*Y + z * z*Z);
	}

	inline float abs2()
	{
		float xa = x * x + y * y + z * z;

		union
		{
			int i;
			float x;
		} u;

		u.x = xa;
		u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
		return u.x;

	}


	inline Vector3D<> crossProduct(Vector3D<> rhs)
	{
		return {-y*rhs.z + z*rhs.y, -z*rhs.x + x*rhs.z, -x*rhs.y + y*rhs.x};
	}
	inline Gtype dotProduct(Vector3D<> rhs)
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	inline Gtype dotDivide(Vector3D<> rhs)
	{
		return x / rhs.x + y / rhs.y + z / rhs.z;
	}
	inline Gtype componentSum()
	{
		return x + y + z ;
	}

	inline Vector3D<> perp()
	{
		return ;
	}

	inline void toUnitVect()
	{
		float inverseSquareRoot = Q_rsqrt(this->x*this->x + this->y*this->y + this->z*this->z);
		x = x* inverseSquareRoot;
		y = y * inverseSquareRoot;
		z = z * inverseSquareRoot;
	}
};

struct G_COLOR32
{
	unsigned int a : 8;
	unsigned int b : 8;
	unsigned int g : 8;
	unsigned int r : 8;

	G_COLOR32 operator*(float rhs)
	{
		G_COLOR32 result;
		result.r = rhs * this->r;
		result.g = rhs * this->g;
		result.b = rhs * this->b;
		result.a = rhs * this->a;
		return result;
	}
};

struct G_SPRITE
{
	G_COLOR32 * memory = NULL;
	int width;
	int height;
};

struct G_PIXELSURFACE
{
	int width;
	int height;
	int size;
	BITMAPINFO BitmapInfo;
	int *memory;
	float *zBuffer;
};

struct G_OBJECT
{
	Vector2D<> oldPos;
	Vector2D<> pos;

	std::vector<Vector2D<>> sizes;
	std::vector<std::pair<Vector2D<>, Vector2D<>>> objBoxes;//MAX, MIN saðalt,solüst
	float radius = 0; //farthest point's distance from center
	float spriteAngle = 0;

	std::vector<G_SPRITE> images;
	unsigned int currentState = 0;

	std::vector<std::vector<Vector2D<>>> convexHulls;
	std::vector<std::vector<Vector2D<>>> concaveHulls;

	inline Vector2D<> size()
	{
		return sizes[currentState];
	}
	inline std::vector<Vector2D<>> convexHull()
	{
		return convexHulls[currentState];
	}
	inline std::vector<Vector2D<>> concaveHull()
	{
		return concaveHulls[currentState];
	}
	inline G_SPRITE* image()
	{
		return &images[currentState];
	}
	inline std::pair<Vector2D<>, Vector2D<>> objBox()
	{
		return objBoxes[currentState];
	}
	G_OBJECT()
	{
	}
	G_OBJECT(std::vector<std::string> imagePaths);
};

struct G_RECT
{
	Vector3D<> position = { 0, 10, 0 };
	float distance = 0;
	float alpha = 1;
	bool ball = false;
	bool obj = false;
	Vector3D<> size = { 0.5, 1, 1 };//<-------------------->x  |y   Oz
	G_COLOR32 color = {200,200,200};
	std::pair<Vector2D<>, Vector2D<>> getScreenCoordinates(Vector3D<> cameraPos);
	inline bool operator==(G_RECT rhs)
	{
		return this->position == rhs.position && this->alpha == rhs.alpha && this->ball == rhs.ball && this->size == rhs.size && this->distance == rhs.distance;
	}
};

struct G_TRIANGLE
{
	int pointIndex[3];
	Vector3D<> normal;
	G_TRIANGLE *neighborOfEdge0_1 = 0;
	G_TRIANGLE *neighborOfEdge1_2 = 0;
	G_TRIANGLE *neighborOfEdge2_0 = 0;
	float area;
	G_COLOR32 color;
	bool visited = false;
};

/*
Vector3D<float> normal(std::vector<Vector3D<float>> *points,G_TRIANGLE* triangle)
{
	return ((*points)[triangle->pointIndex[0]] - (*points)[triangle->pointIndex[1]]).crossProduct((*points)[triangle->pointIndex[0]] - (*points)[triangle->pointIndex[2]]);
}*/


struct CAMERA
{
	int depthBufferWidth;
	int depthBufferHeight;
	float *depthBuffer = NULL;

	Vector3D<float> position;
	Vector3D<float> upVector;
	Vector3D<float> rightVector;
	Vector3D<float> forwardVector;
	float lensRatio;
};

struct OBJECT_3D
{
	std::vector<G_TRIANGLE> triangles;
	Vector3D<float> position;
};

struct DIRECTIONAL_LIGHT
{
	Vector3D<float> direction;
	G_COLOR32 color;
	float intensity;
	float ambient = 0.14f;
};

struct POINT_LIGHT
{
	Vector3D<float> position;
	Vector3D<float> direction;
	G_COLOR32 color;
	float intensity;
	float ambient = 0.14f;
};

struct LIGHTSOURCE
{
	Vector3D<float> position = { 0,0,0 };
	Vector3D<float> direction = { 0,0,0 };
	float intensity = 1.0f;
	G_COLOR32 color;
	bool isDirectional;
	float ambient = 0.14f;
};

struct WAVE
{
	float amplitude;//A
	float speed;//S
	float wavelength;//L
	Vector3D<float> direction;//D
	float steepnes;//Q
};


struct face
{
	unsigned int a;
	unsigned int b;
	unsigned int c;
};

struct CARTESIANAXIS
{
	Vector3D<> up;
	Vector3D<> right;
	Vector3D<> forward;

	//Vector3D<> X_forward = { 1,0,0 };
	//Vector3D<> Z_up = { 0,0,1 };
	//Vector3D<> Y_right = { 0,1,0 };
};

struct OBJECT3D
{
	Vector3D<> position;
	Vector3D<> acceleration;
	Vector3D<> velocity;
	float mass;
	
	CARTESIANAXIS coordinateAxis;
	Vector3D<> forward;
	Vector3D<> up;
	Vector3D<> right;

	std::vector<Vector3D<>> originalPoints;
	std::vector<Vector3D<>> transformedPoints;
	std::vector<G_TRIANGLE> triangles;
	G_COLOR32 color;
	float glossiness;
	//point
	//triangles
	//CubicBoundary
	bool isVisible;
};