#include "Main.h"


void alignObject(OBJECT3D *object, CARTESIANAXIS* cartesianAxis)
{
	for (int i = 0; i < object->originalPoints.size(); i++)
		object->transformedPoints[i] = AlignVector_x(cartesianAxis->up, cartesianAxis->right, cartesianAxis->forward, object->originalPoints[i]);
}


void updateCameraInput(G_PIXELSURFACE *frameBuffer, OBJECT3D* boat,G_INPUT *ginput, CAMERA *camera, float walkSpeed)
{
	Vector3D<> negativeForwardVector = camera->forwardVector * walkSpeed;
	negativeForwardVector = negativeForwardVector * (-1);
	negativeForwardVector.z *= (-1);

	Vector3D<> negativeRightVector = camera->rightVector* walkSpeed;
	negativeRightVector = negativeRightVector * (-1);

	float t = camera->position.z;

	if (ginput->keyboard.isDown('w'))
		camera->position = camera->position + negativeForwardVector;

	Vector3D<float> vectt = negativeForwardVector * (-1);
	if (ginput->keyboard.isDown('s'))
		camera->position = camera->position + vectt;

	if (ginput->keyboard.isDown('a'))
		camera->position = camera->position + negativeRightVector;
	if (ginput->keyboard.isDown('d'))
		camera->position = camera->position + camera->rightVector * walkSpeed;

	if (ginput->keyboard.isDown('e'))
		camera->position.z += walkSpeed;
	if (ginput->keyboard.isDown('q'))
		camera->position.z -= walkSpeed;

	//mouse
	ginput->mouse.absolutePos.x += ginput->mouse.currentPos.x - frameBuffer->width / 2;
	ginput->mouse.absolutePos.y -= ginput->mouse.currentPos.y - frameBuffer->height / 2;

	float horizontalAngle = -3.14f*2.0f*ginput->mouse.absolutePos.x / frameBuffer->width;
	float verticalAngle = 3.14f*2.0f*ginput->mouse.absolutePos.y / frameBuffer->height;

	Vector3D<> upVect = { 0,0,1 };
	camera->forwardVector = camera->position - vector3D(500,500,0);
	//camera->forwardVector = { cosf(horizontalAngle)*cosf(verticalAngle), sinf(horizontalAngle)*cosf(verticalAngle),sinf(verticalAngle) };
	camera->rightVector = upVect.crossProduct(camera->forwardVector);
	camera->upVector = camera->forwardVector.crossProduct(camera->rightVector);

	camera->upVector.toUnitVect();
	camera->rightVector.toUnitVect();
	camera->forwardVector.toUnitVect();
}

void displayStats(G_PIXELSURFACE* frameBuffer,FONT *arial, CAMERA* camera, G_TIME * gtime, G_TIME * displayedTime, unsigned long int *lastTime)
{
	if (doSlowly(lastTime, gtime->processTime, 5.0f))
	{
		*displayedTime = *gtime;
	}

	//print the prepared fps text using eprint hoca
	eprint(frameBuffer, arial, MakeVector(10, 10), "FPS: %d", (int)displayedTime->currentFPS);
	eprint(frameBuffer, arial, MakeVector(10, 40), "Frame Dur: %d ms", displayedTime->frameDuration);
	eprint(frameBuffer, arial, MakeVector(10, 100), "x: %d", (int)camera->position.x);
	eprint(frameBuffer, arial, MakeVector(10, 130), "y: %d", (int)camera->position.y);
	eprint(frameBuffer, arial, MakeVector(10, 160), "z: %d", (int)camera->position.z);
}


Vector3D<> unitVector(Vector3D<> vector)
{
	return vector / vector.abs();
}

Vector3D<> vector3D(float x, float y, float z)
{
	return { x,y,z };
}

Vector3D<> AlignVector_x(Vector3D<> up, Vector3D<> right, Vector3D<> forward, Vector3D<> alignedVector) //align the x to stationalry vector
{
	
	Vector3D<> result = (right * alignedVector.y + forward * alignedVector.x + up * alignedVector.z);

	return result;
}

bool load3DObject(const char* filename, OBJECT3D *object)
{
	std::ifstream objFile(filename);
	if (!objFile)
	{
		debugPrint("Unable to open resource file");
		return false;
	}

	std::string command;
	std::vector<Vector3D<>> vertexes;

	std::string s;
	while (!objFile.eof())
	{
		objFile >> s;
		if (s[0] != 'v' && s[0] != 'f')
			continue;
		if (s[0] == 'v' && s.size() == 1)
		{
			Vector3D<> vertex;
			objFile >> vertex.x >> vertex.y >> vertex.z;

			object->originalPoints.push_back(vertex);

		}
		else if (s[0] == 'f' && s.size() == 1)
		{
			int id1 = 0, id2 = 0, id3 = 0;
			objFile >> id1 >> id2 >> id3;
			G_TRIANGLE tri;
			tri.pointIndex[0] = id1 - 1;
			tri.pointIndex[1] = id2 - 1;
			tri.pointIndex[2] = id3 - 1;
			tri.normal = (object->originalPoints[tri.pointIndex[2]] - object->originalPoints[tri.pointIndex[0]]).crossProduct(object->originalPoints[tri.pointIndex[1]] - object->originalPoints[tri.pointIndex[0]]);
			tri.normal.toUnitVect();
			
			object->triangles.push_back(tri);
		}
	}

	return true;
}

//std::vector<POINT_LIGHT> pointLightSource;
	//std::vector<DIRECTIONAL_LIGHT> directionalLightSource;

void drawObject(G_PIXELSURFACE *frameBuffer, CAMERA* camera, OBJECT3D *object, std::vector<POINT_LIGHT> *pointLightSource ,std::vector<DIRECTIONAL_LIGHT> *directionalLightSource)
{
	for (int i = 0; i < object->triangles.size(); i++)
	{
		//assert(!(object->triangles[i].pointIndex[0] >= object->transformedPoints.size() || object->triangles[i].pointIndex[0] < 0));
		//assert(!(object->triangles[i].pointIndex[1] >= object->transformedPoints.size() || object->triangles[i].pointIndex[1] < 0));
		//assert(!(object->triangles[i].pointIndex[2] >= object->transformedPoints.size() || object->triangles[i].pointIndex[2] < 0));
		
		draw3DTriangleMaster(frameBuffer, camera, object->triangles[i], &(object->transformedPoints), object->position, object->color, pointLightSource, directionalLightSource, object->glossiness);
	}
}

void translateObjectOrigin(OBJECT3D* object, Vector3D<> offset)
{
	for (int i = 0; i < object->originalPoints.size(); i++)
	{
		object->originalPoints[i] = object->originalPoints[i] + offset;
	}
}

void resetZBuffer(G_PIXELSURFACE *frameBuffer)
{
	for (int y = 0; y < frameBuffer->height; y++)
		for (int x = 0; x < frameBuffer->width; x++)
			frameBuffer->zBuffer[x + y * frameBuffer->width] = INFINITY;
}



void calculateNormals(OBJECT3D* obj)
{
	if (obj->transformedPoints.size() == 0)
		return;

	for (int i = 0; i < obj->triangles.size(); i++)
	{
		Vector3D<> pointA = obj->transformedPoints[obj->triangles[i].pointIndex[0]];
		Vector3D<> pointB = obj->transformedPoints[obj->triangles[i].pointIndex[1]];
		Vector3D<> pointC = obj->transformedPoints[obj->triangles[i].pointIndex[2]];

		obj->triangles[i].normal = (pointC - pointA).crossProduct(pointB - pointA);
		obj->triangles[i].normal.toUnitVect();
	}
}

