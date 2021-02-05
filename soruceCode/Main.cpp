#include "Main.h"

//global variables
bool Running = true;
G_WINDOW gameWindow;		//memory portion about the window
G_INPUT ginput;				//memory portion about input
G_PIXELSURFACE frameBuffer;
G_TIME gtime;				//memory portion about time

Vector3D<float> ORIGIN = { 0,0,0 };

CAMERA camera;
FONT arial;

OBJECT3D boat;
OBJECT3D oceanWave;
OBJECT3D sphere;

std::vector<POINT_LIGHT> pointLightSource;
std::vector<DIRECTIONAL_LIGHT> directionalLightSource;

const int nWave = 15;
WAVE wave[nWave];
const int width = 40;
const int height = 40;
float seperation = 70.0f;

G_PIXELSURFACE backGroundSurface;


Vector2D<> rotate2D(Vector2D<> vector,float angle)
{
	Vector2D<> rotationVector = {cosf(angle), sinf(angle)};
	rotationVector.toUnitVect();

	//multiply vector and rotaionVector;
	
	//(rotationVector.x + rotationVector.y) * (vector.x + vector.y);
	
	float imaginaryPart = vector.y*rotationVector.x + vector.x*rotationVector.y;
	
	float realPart = rotationVector.x*vector.x - rotationVector.y*vector.y;

	Vector2D<> result = {realPart, imaginaryPart};

	return result;
}



struct MATRIX3X3
{
	Vector3D<> a, b, c;

	inline Vector3D<> operator*(Vector3D<> rhs)
	{
		Vector3D<> result;
		result.x = a.dotProduct(rhs);
		result.y = b.dotProduct(rhs);
		result.z = c.dotProduct(rhs);
		return result;
	}
};



Vector3D<> rotateAroundX_3D(Vector3D<> originalVector, float angle)
{
	MATRIX3X3 rotationMatrix_aroundX;
	rotationMatrix_aroundX.a = {1,0,0};
	rotationMatrix_aroundX.b = { 0,cosf(angle),-sinf(angle) };
	rotationMatrix_aroundX.c = { 0,sinf(angle),cosf(angle) };

	return rotationMatrix_aroundX*originalVector;
}

Vector3D<> rotateAroundY_3D(Vector3D<> originalVector, float angle)
{
	MATRIX3X3 rotationMatrix_aroundY;
	rotationMatrix_aroundY.a = { cosf(angle),0,-sinf(angle) };
	rotationMatrix_aroundY.b = { 0,1,0 };
	rotationMatrix_aroundY.c = { sinf(angle),0,cosf(angle) };

	return rotationMatrix_aroundY * originalVector;
}

Vector3D<> rotateAroundZ_3D(Vector3D<> originalVector, float angle)
{
	MATRIX3X3 rotationMatrix_aroundZ;
	rotationMatrix_aroundZ.a = { cosf(angle),-sinf(angle),0 };
	rotationMatrix_aroundZ.b = { sinf(angle),cosf(angle),0 };
	rotationMatrix_aroundZ.c = { 0,0,1};

	return rotationMatrix_aroundZ * originalVector;
}

#define nData 3
struct MOVING_AVERAGE
{
	float dataList[nData];
	float sum = 0;
	int topIndex = 0;
	bool runLikeHell = false;
};

float movingAverage(struct MOVING_AVERAGE *movingAverage, float newData)
{
	//subtract the oldest data, and add the newest data to the sum
	movingAverage->sum -= movingAverage->dataList[movingAverage->topIndex];
	movingAverage->sum += newData;

	//overwrite the newData on the oldest data in the list
	movingAverage->dataList[movingAverage->topIndex] = newData;

	//when the list is full, enable RunLikeHell
	if (movingAverage->topIndex == nData - 1)
		movingAverage->runLikeHell = true;

	//increment the topIndex
	movingAverage->topIndex = (movingAverage->topIndex + 1) % nData;

	if (movingAverage->runLikeHell == 1)
		return(movingAverage->sum / nData);

	return(movingAverage->sum / (movingAverage->topIndex + 1));
}

/*
Vector3D<int> resultantForce = { 0,0,0 };
Vector3D<> resultantTorque = { 0,0,0 };

float momentOfInertia = 10000000*2*4;
int mass = 5;
float az = 0,vz = 0,dz=0;

float wx=0.0, wy=0.0;
float dx = 0,dy = 0;

void transformBoat(OBJECT3D *boat, OBJECT3D* oceanWave, float timeInSeconds)
{
	calculateBouyantForceAndTorque(&resultantForce, &resultantTorque, boat, oceanWave, wave,nWave, timeInSeconds);

	float ax = 0, ay = 0;
	Vector3D<> Vdown = { 0, 0, -1 };
	float torqueY = Vdown.dotProduct(resultantTorque);
	ay = torqueY / momentOfInertia;
	if (resultantTorque.dotProduct(boat->coordinateAxis.forward) > 0)
	{
		ay = ay * (-1);
	}

	wy = wy + ay * timeInSeconds;
	wy = wy*0.2f;
	//dy = dy + wy * timeInSeconds;

	//dx = sin(timeInSeconds);
	//dy = cos(timeInSeconds);

	boat->coordinateAxis.forward = { cosf(dx)*cosf(dy), sinf(dx)*cosf(dy),sinf(dy) };
	boat->coordinateAxis.right = boat->coordinateAxis.forward.crossProduct(vector3D(0, 0, 1));
	boat->coordinateAxis.up = boat->coordinateAxis.right.crossProduct(boat->coordinateAxis.forward);

	//Vector3D<> point_center = vector3D(boat->position.x, boat->position.y, waveFunction(boat->position.x, boat->position.y, timeInSeconds, wave, nWave).z);						 //problem
	//Vector3D<> point_forward = vector3D(boat->position.x + boat->coordinateAxis.forward.x * 150, boat->position.y + boat->coordinateAxis.forward.y * 150, waveFunction(boat->position.x + boat->coordinateAxis.forward.x * 150, boat->position.y + boat->coordinateAxis.forward.y * 150, timeInSeconds, wave, nWave).z);
	//Vector3D<> point_right = vector3D(boat->position.x + boat->coordinateAxis.right.x * 60, boat->position.y + boat->coordinateAxis.right.y * 60, waveFunction(boat->position.x + boat->coordinateAxis.right.x * 60, boat->position.y + boat->coordinateAxis.right.y * 60, timeInSeconds, wave, nWave).z); //problem

	//boat->coordinateAxis.forward = (point_forward - point_center);
	//boat->coordinateAxis.right = (point_right - point_center);
	//boat->coordinateAxis.up = boat->coordinateAxis.right.crossProduct(boat->coordinateAxis.forward);

	boat->coordinateAxis.right.toUnitVect();
	boat->coordinateAxis.forward.toUnitVect();
	boat->coordinateAxis.up.toUnitVect();

	alignObject(boat, &(boat->coordinateAxis));

	az = 0;
	
	static struct MOVING_AVERAGE mv;
	
	double force = (float)(resultantForce.z);
	//force = movingAverage(&mv, force);

	int m = 5;

	//if (force < 0)force = 0;
	if (resultantForce.z == 0)
	{
		volatile int tt = resultantForce.z - 3 * m;
	}

	volatile int forcei = resultantForce.z - 30 * m;
	
	az = forcei;
	az /= m*50;

	volatile int l=0;
	vz = vz + az* timeInSeconds;
	vz *= 0.01f;


	dz = dz + vz * timeInSeconds;

	boat->position = { 500,500, dz};
}

*/

Vector3D<> bouyantTorque;
float bouyantForce = 0;

float vel = 0;
float acc = 0;

//float angle = 0;

void applyBouyantForce(OBJECT3D *boat, OBJECT3D* oceanWave, float deltatimeInSeconds ,float timeInSeconds)
{
	calculateBouyantForceAndTorque(&bouyantForce, &bouyantTorque, boat, oceanWave, wave, nWave, timeInSeconds);
	
	Vector3D<> rotationAxis = unitVector(bouyantTorque);
	
	//for (int i = 0; i < boat->transformedPoints.size(); i++)
	//{
	//	boat->transformedPoints[i] = rotate3D(boat->originalPoints[i], rotationAxis , bouyantTorque.abs()/100000/2);
	//}
	

	boat->acceleration.z = bouyantForce/5;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//generate random number seed
	srand(19);

	//window
	gameWindow = createWindow(1000, 600, "MyWindow", hInstance);

	//input
	GInput_Init(&ginput);
	ShowCursor(true);

	//font
	loadFont(&arial, "fonts\\myfont.fnt");

	//camera 
	cameraInit(&camera);

	//pixel buffers
	allocateBufferMemory(&frameBuffer, gameWindow.clientAreaWidth, gameWindow.clientAreaHeight);

	allocateBufferMemory(&backGroundSurface, gameWindow.clientAreaWidth, gameWindow.clientAreaHeight);
	draw_BlueBackgound_and_GreenGround(&backGroundSurface, &gameWindow);

	//zBuffer memory allocation
	frameBuffer.zBuffer = (float*)malloc(frameBuffer.height*frameBuffer.width * sizeof(float));

	//wave
	createWaveSurface(wave, nWave, width, height, seperation, &oceanWave, blue);

	for (int i = 0; i < oceanWave.triangles.size(); i++)
		oceanWave.triangles[i].color = oceanWave.color;

	oceanWave.glossiness = 0.0f;
	int nNeighbors1 = scanAndFindNeighborTriangles(&oceanWave);

	//boat
	load3DObject("theCuteBoat.obj", &boat);
	//translateObjectOrigin(&boat, { 0,0,0.0f });
	scaleObject(&boat, 0.8f);
	int nNeighbors2 = scanAndFindNeighborTriangles(&boat);
	
	calculateNormals(&boat);

	for (int i = 0; i < boat.originalPoints.size(); i++)
		boat.transformedPoints.push_back(boat.originalPoints[i]);

	boat.position = vector3D(500, 500, 100);
	boat.color = brown;
	for (int i = 0; i < boat.triangles.size(); i++)
		boat.triangles[i].color = boat.color;

	boat.glossiness = 5.0f;

	//light
	POINT_LIGHT pointLight;
	pointLight.position = boat.position + vector3D(0,0, 350);
	pointLight.direction = { 10,10,0 };
	pointLight.direction.toUnitVect();

	pointLight.color = warmWhite;
	pointLight.intensity = 2.0f;
	pointLight.ambient = 0.3f;
	
	pointLightSource.push_back(pointLight);
	pointLightSource.push_back(pointLight);

	calculateAreaOfTriangles(&boat);


	timerInit(&gtime);

	while (Running)
	{
		HandleWindowMessage();

		//fps limiter and frame duration calculations are also perfrmed
		fixedFPS(&gtime, 60);

		//camera translation based on user input
		updateCameraInput(&frameBuffer,&boat, &ginput, &camera, 10);

		//boat color
		for (int i = 0; i < boat.triangles.size(); i++)
			boat.triangles[i].color = boat.color;

		//wave color
		for (int i = 0; i < oceanWave.triangles.size(); i++)
			oceanWave.triangles[i].color = oceanWave.color;

		//some timers
		static unsigned long proccessStartTime = gtime.processTime;
		unsigned long currentTime = gtime.processTime - proccessStartTime;
		float timeInSeconds = ((double)currentTime) / (1000.0f);

		//wave dynamics
		simulateWave(timeInSeconds, wave, height, width, nWave, seperation, &oceanWave);
		calculateNormals(&oceanWave);
		
		//boat dynamics
		boat.acceleration = {0,0,0};
		applyBouyantForce(&boat, &oceanWave, (float)gtime.frameDuration/1000*13, timeInSeconds);
		boat.acceleration.z += -9.81;

		boat.velocity = boat.velocity + boat.acceleration * (float)gtime.frameDuration / 1000*13;		
		boat.velocity = boat.velocity * 0.97; //damping

		boat.position = boat.position + boat.velocity * (float)gtime.frameDuration / 1000 * 13;
		//calculateNormals(&boat);

		//light
		pointLightSource[0].position = boat.position;

		//pre-rendering
		resetZBuffer(&frameBuffer);
		memcpy_s(frameBuffer.memory, frameBuffer.height*frameBuffer.width * 4, backGroundSurface.memory, frameBuffer.height*frameBuffer.width * 4);
		
		//rendering			
		drawObject(&frameBuffer, &camera, &boat, &pointLightSource, &directionalLightSource);
		drawObject(&frameBuffer, &camera, &oceanWave, &pointLightSource, &directionalLightSource);

		//drawNormals(&frameBuffer, &camera, &boat, black,5);

		//draw3DLine(&frameBuffer, &camera, boat.position, boat.position + resultantForce/10, red);
		//draw3DLine(&frameBuffer, &camera, boat.position, boat.position + resultantTorque / 10, red);

		static G_TIME displayedTime = gtime;
		static unsigned long int counter = gtime.processTime;
		displayStats(&frameBuffer, &arial, &camera, &gtime, &displayedTime, &counter);
		
		eprint(&frameBuffer, &arial, MakeVector(10, 260), "F: %f", bouyantForce);
		eprint(&frameBuffer, &arial, MakeVector(10, 290), "T: %f", (float)gtime.frameDuration / 1000);
		eprint(&frameBuffer, &arial, MakeVector(10, 320), "vel: %f", vel);
		eprint(&frameBuffer, &arial, MakeVector(10, 350), "acc: %f", acc);

		//render stuff
		displayBuffer(frameBuffer, gameWindow);
		
	}

	free(frameBuffer.zBuffer);	
	deallocateBufferMemory(&frameBuffer);
	deallocateBufferMemory(&backGroundSurface);

	return 0;
}

//this funtion is a magical function to handle windows message events. msg is the message received from windows abi. depending on the message, do what is needed.
//hwnd is the hanle of the window, not really important.
//wParam and lParam are used to receive more specific values from the windows abi such as mouse position, etc.
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;
	switch (msg)
	{
		case WM_CLOSE:
		{
			Running = false;
		}break;
		case WM_DESTROY:
		{
			Running = false;
		}break;

		//window size is changed in this case, so reallocate the frameBuffer to the new size wich are received as well using lParam
		case WM_SIZE:
		{
			gameWindow.clientAreaWidth = GET_X_LPARAM(lParam);
			gameWindow.clientAreaHeight = GET_Y_LPARAM(lParam);

			frameBuffer.width = gameWindow.clientAreaWidth;
			frameBuffer.height = gameWindow.clientAreaHeight;

			//backBuffer reallocation
			deallocateBufferMemory(&frameBuffer);
			allocateBufferMemory(&frameBuffer, frameBuffer.width, frameBuffer.height);
			
			//zBuffer reallocation
			free(frameBuffer.zBuffer);
			frameBuffer.zBuffer = (float*)malloc(frameBuffer.width*frameBuffer.height*sizeof(float));
			resetZBuffer(&frameBuffer);

			//background Buffer reallocation
			deallocateBufferMemory(&backGroundSurface);
			allocateBufferMemory(&backGroundSurface, gameWindow.clientAreaWidth, gameWindow.clientAreaHeight);
			draw_BlueBackgound_and_GreenGround(&backGroundSurface, &gameWindow);

			//SIZE_MAXIMIZED;
		}break;
		
		default:
		{
			//keyboard and mouse inputs are obtained 
			GetMessages(&ginput, msg, wParam, lParam);

			Result = DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	return (Result);
}






struct QUATERNION
{
	float w, x, y, z;

	inline QUATERNION operator+(QUATERNION rhs)
	{
		return { this->w + rhs.w , this->x + rhs.x ,this->y + rhs.y ,this->z + rhs.z };
	}
	inline QUATERNION operator-(QUATERNION rhs)
	{
		return { this->w - rhs.w , this->x - rhs.x ,this->y - rhs.y ,this->z - rhs.z };
	}
	inline QUATERNION conjugate()
	{
		return { this->w , -this->x ,-this->y ,-this->z };
	}

	inline Vector3D<> vector3D()
	{
		return {this->x,this->y,this->z};
	}

	inline Vector2D<> vector2D()
	{
		return { this->x,this->y};
	}

	inline QUATERNION rotate(Vector3D<> rotationAxis, float angle)
	{
		QUATERNION q;
		q.w = cosf(angle / 2);
		q.x = sinf(angle / 2)*rotationAxis.x;
		q.y = sinf(angle / 2)*rotationAxis.y;
		q.z = sinf(angle / 2)*rotationAxis.z;

	}

	inline QUATERNION operator*(QUATERNION rhs)
	{
		QUATERNION result;
		result.w = (this->w*rhs.w) - (this->x*rhs.x) - (this->y*rhs.y) - (this->z*rhs.z);
		result.x = (this->w*rhs.x) + (this->x*rhs.w) + (this->y*rhs.z) - (this->z*rhs.y);
		result.y = (this->w*rhs.y) - (this->x*rhs.z) + (this->y*rhs.w) + (this->z*rhs.x);
		result.z = (this->w*rhs.z) + (this->x*rhs.y) - (this->y*rhs.x) + (this->z*rhs.w);
		return result;
	}

	inline QUATERNION operator*(Vector3D<> rhs)
	{
		QUATERNION result;
		result.w = -(this->x*rhs.x) - (this->y*rhs.y) - (this->z*rhs.z);
		result.x = (this->w*rhs.x) + (this->y*rhs.z) - (this->z*rhs.y);
		result.y = (this->w*rhs.y) - (this->x*rhs.z) + (this->z*rhs.x);
		result.z = (this->w*rhs.z) + (this->x*rhs.y) - (this->y*rhs.x);
		return result;
	}
};

QUATERNION Quaternion(float w, float x, float y, float z)
{
	return { w,x,y,z };
}


Vector3D<> rotate3D(Vector3D<> originalVector, Vector3D<> rotationAxis, float angle)
{
	QUATERNION q;
	q.w = cosf(angle / 2);
	q.x = sinf(angle / 2)*rotationAxis.x;
	q.y = sinf(angle / 2)*rotationAxis.y;
	q.z = sinf(angle / 2)*rotationAxis.z;

	QUATERNION result;

	result = q * originalVector;
	result = result * q.conjugate();

	return result.vector3D();
}

Vector3D<> rotate3D(Vector3D<> originalVector, QUATERNION q)
{
	QUATERNION result;

	result = q * originalVector;
	result = result * q.conjugate();

	return result.vector3D();
}




struct CARTESIANAXIS_
{
	Vector3D<> X_forward = { 1,0,0 };
	Vector3D<> Z_up = { 0,0,1 };
	Vector3D<> Y_right = { 0,1,0 };

	QUATERNION aboutXaxis = { 1,0,0,0 };
	QUATERNION aboutYaxis = { 1,0,0,0 };
	QUATERNION aboutZaxis = { 1,0,0,0 };

	float angleAboutXaxis = 0;
	float angleAboutYaxis = 0;
	float angleAboutZaxis = 0;

	float scaleX = 1;
	float scaleY = 1;
	float scaleZ = 1;


	void setAngel_X(float angle)
	{
		angleAboutXaxis = angle;
		aboutXaxis.rotate({1,0,0}, angle);
	}

	void addAngle()
	{

	}

	void setScale(float xScale, float yScale, float zScale)
	{
		scaleX = xScale;
		scaleY = yScale;
		scaleZ = zScale;

		X_forward = X_forward.unitVector() * xScale;
		Y_right = Y_right.unitVector() * yScale;
		Z_up = Z_up.unitVector() * zScale;
	}

};



