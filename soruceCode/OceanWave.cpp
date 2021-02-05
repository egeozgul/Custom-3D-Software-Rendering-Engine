#include "OceanWave.h"

#undef min
#undef max

//x,y,z,d ;z is up, d is color
Vector3D<float> waveFunction(float x, float y, float time, WAVE* wave, int nWave)
{
	Vector3D<float> result = { 0,0,0 };

	result.x += x;
	result.y += y;

	result.z -= 6;

	for (int i = 0; i < nWave; i++)
	{
		float phaseConstant = wave[i].speed * 2 / wave[i].wavelength;
		
		float theta = wave[i].direction.dotProduct({ x,y,0 })*(2 / wave[i].wavelength) - (double)time * phaseConstant;
		float constant = wave[i].amplitude * cosf(theta);
		result.x += wave[i].steepnes * (float)(wave[i].direction.x*constant);
		result.y += wave[i].steepnes * (float)(wave[i].direction.y*constant);
		result.z += constant;
	}

	return result;
}

void simulateWave(float time, WAVE*wave, int height,int width,int nWave, float seperation, OBJECT3D *oceanWave)
{
	for (int iy = 0; iy < height; iy++)
	{
		for (int ix = 0; ix < width; ix++)
		{
			float tx = ix * seperation;
			float ty = iy * seperation;
			
			oceanWave->transformedPoints[ix + iy * width] = waveFunction(tx, ty, time, wave, nWave);
		}
	}
}

void createWaveSurface(WAVE* wave, int nWave, int width, int height,float seperation, OBJECT3D *objectWave,G_COLOR32 color)
{
	objectWave->color = color;
	int waveTriangleCount = (height - 1)*(width - 1) * 2;
	objectWave->triangles.resize(waveTriangleCount);

	for (int i = 0; i < nWave; i++)
	{
		wave[i].amplitude = 12 * (float)random(10, 40) / 10;
		float angle = (float)random(0, 90)*3.14 / 180;
		wave[i].direction = { sinf(angle),cosf(angle),0 };
		wave[i].speed = 50 * (float)random(1, 5);
		wave[i].steepnes = 0.6* ((float)random(10, 20)) / 20;
		wave[i].wavelength = 2*((float)random(20, 100*64)) / 10;
	}

	objectWave->transformedPoints.reserve(width*height);

	for (int iy = 0; iy < height - 1; iy++)
	{
		for (int ix = 0; ix < width - 1; ix++)
		{
			G_TRIANGLE triangleA = { (ix + iy * width),(ix + 1 + iy * width),(ix + (iy + 1) * width) };
			G_TRIANGLE triangleB = { (ix + (iy + 1) * width), (ix + 1 + iy * width),(ix + 1 + (iy + 1) * width) };

			objectWave->triangles[2 * ix + iy * 2 * (width - 1)] = triangleA;
			objectWave->triangles[2 * ix + 1 + iy * 2 * (width - 1)] = triangleB;
		}
	}

	Vector2D<float> position = { -width * seperation / 2,-height * seperation / 2 };

	for (int iy = 0; iy < height; iy++)
	{
		for (int ix = 0; ix < width; ix++)
		{
			float tx = ix * seperation + position.x;
			float ty = iy * seperation + position.y;

			Vector3D<> t = { 0,0,0 };
			objectWave->transformedPoints.push_back(t);
		}
	}
}

float areaOfTriangle(std::vector<Vector3D<>> *points, G_TRIANGLE *tri)
{
	return (((*points)[tri->pointIndex[0]]) - ((*points)[tri->pointIndex[1]])).crossProduct(((*points)[tri->pointIndex[0]]) - ((*points)[tri->pointIndex[2]])).abs()/2;
}

float areaOfTriangle(Vector3D<> p1, Vector3D<> p2, Vector3D<> p3)
{
	Vector3D<> v1 = p1 - p2;
	Vector3D<> v2 = p3 - p2;
	float area = v1.crossProduct(v2).abs()/2;

	return area;
}


bool calculateAreaOfTriangles(OBJECT3D *obj)
{
	if (obj->transformedPoints.size() == 0)
	{

		return false;
	}
	for (int i = 0; i < obj->triangles.size(); i++)
		obj->triangles[i].area = areaOfTriangle(&(obj->transformedPoints),&(obj->triangles[i]));
	return true;
}

int scanAndFindNeighborTriangles(OBJECT3D *object)
{
	int nNeigborsFound = 0;

	//visiting each triagnle
	for (int i1 = 0; i1 < object->triangles.size(); i1++)
	{
		G_TRIANGLE *currentTriangle = &(object->triangles[i1]);

		//neighbor search
		for (int i3 = 0; i3 < object->triangles.size(); i3++)
		{
			//avoiding self neigbor testing
			if (i3 == i1)
				continue;

			//this is the neighbors we are going to be testing below
			G_TRIANGLE *theNeighbor = &(object->triangles[i3]);
			
			int nSame = 0;
			//checking side 0_1
			for (int i = 0; i < 3; i++)
			{
				if (currentTriangle->pointIndex[0] == theNeighbor->pointIndex[i])
					nSame++;
				else if (currentTriangle->pointIndex[1] == theNeighbor->pointIndex[i])
					nSame++;
			}

			if (nSame == 2)
			{
				object->triangles[i1].neighborOfEdge0_1 = theNeighbor;
				nNeigborsFound++;
				continue;
			}

			nSame = 0;
			//checking side 1_2
			for (int i = 0; i < 3; i++)
			{
				if (currentTriangle->pointIndex[1] == theNeighbor->pointIndex[i])
					nSame++;
				else if (currentTriangle->pointIndex[2] == theNeighbor->pointIndex[i])
					nSame++;
			}

			if (nSame == 2)
			{
				object->triangles[i1].neighborOfEdge1_2 = theNeighbor;
				nNeigborsFound++;
				continue;
			}

			nSame = 0;
			//checking side 2_0
			for (int i = 0; i < 3; i++)
			{
				if (currentTriangle->pointIndex[2] == theNeighbor->pointIndex[i])
					nSame++;
				else if (currentTriangle->pointIndex[0] == theNeighbor->pointIndex[i])
					nSame++;
			}

			if (nSame == 2)
			{
				object->triangles[i1].neighborOfEdge2_0 = theNeighbor;
				nNeigborsFound++;
				continue;
			}

		}

	}
	return nNeigborsFound;
}



Vector3D<> greedySearch_Point;
float distanceSquared(OBJECT3D *object, G_TRIANGLE* current)
{
	//current->color = red;
	Vector3D<> center = object->transformedPoints[current->pointIndex[0]] + object->transformedPoints[current->pointIndex[1]] + object->transformedPoints[current->pointIndex[2]];
	center = center / 3;
	Vector3D<> diff = center - greedySearch_Point;

	return (diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
}

struct LINE
{
	Vector3D<> direction;
	Vector3D<> point;
} greedySearch_Line;

float modd(float a)
{
	if (a < 0)
		return -a;

	return a;
}

float distanceToLine(OBJECT3D *object, G_TRIANGLE* current)
{
	Vector3D<>  center = object->transformedPoints[current->pointIndex[0]];
	center = center + object->transformedPoints[current->pointIndex[1]];
	center = center + object->transformedPoints[current->pointIndex[2]];
	center = center / 3;

	Vector3D<> tempNormal = (center - greedySearch_Line.point).crossProduct(greedySearch_Line.direction);

	Vector3D<> projectionVector = tempNormal.crossProduct(greedySearch_Line.direction);

	float projection = projectionVector.dotProduct(center - greedySearch_Line.point);

	return modd(projection);
}

struct PLANE
{
	Vector3D<> normal;
	Vector3D<> point;
}greedySearch_Plane;

float distanceToPlane(OBJECT3D *object, G_TRIANGLE* current)
{
	Vector3D<>  center = object->transformedPoints[current->pointIndex[0]];
	center = center + object->transformedPoints[current->pointIndex[1]];
	center = center + object->transformedPoints[current->pointIndex[2]];
	center = center / 3;

	float projection = greedySearch_Plane.normal.dotProduct(center - greedySearch_Plane.point);

	return modd(projection);
}

G_TRIANGLE* greedySearch_byShortestDistance(OBJECT3D *boat, G_TRIANGLE *currentTriangle, float(*distance)(OBJECT3D *, G_TRIANGLE*))
{
	float minimumDistance = INFINITY;

	while (true)
	{
		G_TRIANGLE *closestNeighbor = NULL;
		float closestNeighborDistance = INFINITY;

		float resultA = INFINITY;
		if (currentTriangle->neighborOfEdge0_1 != NULL)
		{
			resultA = distance(boat, currentTriangle->neighborOfEdge0_1);
			closestNeighbor = currentTriangle->neighborOfEdge0_1;
			closestNeighborDistance = resultA;
		}

		float resultB = INFINITY;
		if (currentTriangle->neighborOfEdge1_2 != NULL)
		{
			resultB = distance(boat, currentTriangle->neighborOfEdge1_2);

			if (resultB < resultA)
			{
				closestNeighborDistance = resultB;
				closestNeighbor = currentTriangle->neighborOfEdge1_2;
			}
		}

		float resultC = INFINITY;
		if (currentTriangle->neighborOfEdge2_0 != NULL)
		{
			resultC = distance(boat, currentTriangle->neighborOfEdge2_0);
			if (resultC < closestNeighborDistance)
			{
				closestNeighborDistance = resultC;
				closestNeighbor = currentTriangle->neighborOfEdge2_0;
			}
		}

		if (closestNeighborDistance <= minimumDistance)
		{
			minimumDistance = closestNeighborDistance;
			currentTriangle = closestNeighbor;
		}
		else
		{
			break;
		}
	}

	//currentTriangle->color = white;
	return currentTriangle;
}

G_TRIANGLE* linearSearch_byShortestDisitance(OBJECT3D *boat, float(*distance)(OBJECT3D *, G_TRIANGLE*))
{
	G_TRIANGLE *currentTriangle = &(boat->triangles[0]);

	for (int i = 1; i < boat->triangles.size(); i++)
	{
		float distanceA = distance(boat, currentTriangle);
		float distanceB = distance(boat, &(boat->triangles[i]));

		if (distanceB < distanceA)
		{
			currentTriangle = &(boat->triangles[i]);
		}
	}

	return currentTriangle;
}


Vector3D<> intersectPoint(Vector3D<> rayVector, Vector3D<> rayPoint, Vector3D<> planeNormal, Vector3D<> planePoint)
{
	Vector3D<> diff = rayPoint - planePoint;
	double prod1 = diff.dotProduct(planeNormal);
	double prod2 = rayVector.dotProduct(planeNormal);
	double prod3 = prod1 / prod2;
	return rayPoint - rayVector * prod3;
}



bool planeLineIntersection(Vector3D<> * intersection, Vector3D<> pointL, Vector3D<> dir, Vector3D<> pointP, Vector3D<> normal)
{
	if (dir.dotProduct(normal) == 0)
		if ((pointP - pointL).dotProduct(normal) == 0)
			return true;
		else
			return false;
	double d = (pointP - pointL).dotProduct(normal) / dir.dotProduct(normal);
	(*intersection) = dir * d + pointL;
	return true;
}


struct SUBMERGE_DATA
{
	Vector3D<> submergedVertex;
	PLANE waveTriangle;
};

void findSubmergedTriangles2(Vector3D<> *resultantForce, Vector3D<> *resultantTorque, OBJECT3D *boat, OBJECT3D *wave, G_TRIANGLE *currentBoatTriangle, G_TRIANGLE *currentWaveTriangle)
{
	if (currentBoatTriangle->visited)
		return;

	currentBoatTriangle->visited = true;

	int nSubmerged = 0;
	int nNotSubmerged = 0;

	SUBMERGE_DATA submergeInfo[3];

	float submergeDepth = 0;

	for (int i = 0; i < 3; i++)
	{
		//Find the wave triangle that is closest to the normal line of the boat's triangle
		greedySearch_Line.direction = currentBoatTriangle->normal;
		greedySearch_Line.point = boat->transformedPoints[currentBoatTriangle->pointIndex[i]] + boat->position;
		currentWaveTriangle = greedySearch_byShortestDistance(wave, currentWaveTriangle, distanceToLine);
		//found triangle: currentWaveTriangle

		Vector3D<> waveTriangleCenter = wave->transformedPoints[currentWaveTriangle->pointIndex[0]];
		waveTriangleCenter = waveTriangleCenter + wave->transformedPoints[currentWaveTriangle->pointIndex[1]];
		waveTriangleCenter = waveTriangleCenter + wave->transformedPoints[currentWaveTriangle->pointIndex[2]];
		waveTriangleCenter = waveTriangleCenter / 3 + wave->position;

		Vector3D<> boatToWaveVector = waveTriangleCenter - (boat->transformedPoints[currentBoatTriangle->pointIndex[i]] + boat->position);
		
		submergeDepth = boatToWaveVector.z;
		if (submergeDepth > 0)return;

		float dotProduct = currentWaveTriangle->normal.dotProduct(boatToWaveVector);
		
		bool pointIsSubmerged = (dotProduct < 0);

		//check if normals of wave and boat triagnles are not facing the same directoin
		//pointIsSubmerged &= (currentBoatTriangle->normal.dotProduct(currentWaveTriangle->normal) < 0);

		if (pointIsSubmerged)
		{
			//point is submerged
			submergeInfo[nSubmerged].submergedVertex = (boat->transformedPoints[currentBoatTriangle->pointIndex[i]] + boat->position);//absolute position
			submergeInfo[nSubmerged].waveTriangle.normal = currentWaveTriangle->normal;
			submergeInfo[nSubmerged].waveTriangle.point = waveTriangleCenter; //absolute position
			
			nSubmerged++;
		}
		else if(pointIsSubmerged)
		{
			//point is not submerged
			submergeInfo[2 - nNotSubmerged].submergedVertex = (boat->transformedPoints[currentBoatTriangle->pointIndex[i]] + boat->position);//absolute position
			submergeInfo[2 - nNotSubmerged].waveTriangle.normal = currentWaveTriangle->normal;
			submergeInfo[2 - nNotSubmerged].waveTriangle.point = waveTriangleCenter;//absolute position
			
			nNotSubmerged++;
		}
	}

	if (nSubmerged == 0)
	{
		currentBoatTriangle->color = red;
		return;
	}

	//BOUYANT FORCE CALCULATION
	if (currentBoatTriangle->normal.z < 0)
	{
		float submergedArea = 0;
		 
		if (nSubmerged == 1)
		{
			Vector3D<> intersectionA;
			Vector3D<> intersectionB;
			//intersectionA = intersectPoint(submergeInfo[1].submergedVertex- submergeInfo[0].submergedVertex ,submergeInfo[0].submergedVertex, submergeInfo[0].waveTriangle.normal, submergeInfo[0].waveTriangle.point);
			//intersectionB = intersectPoint(submergeInfo[2].submergedVertex - submergeInfo[0].submergedVertex, submergeInfo[0].submergedVertex, submergeInfo[0].waveTriangle.normal, submergeInfo[0].waveTriangle.point);
			
			bool flag = planeLineIntersection(&intersectionA, submergeInfo[0].submergedVertex, submergeInfo[1].submergedVertex - submergeInfo[0].submergedVertex, submergeInfo[0].waveTriangle.point,submergeInfo[0].waveTriangle.normal);
			flag &= planeLineIntersection(&intersectionB, submergeInfo[0].submergedVertex, submergeInfo[2].submergedVertex - submergeInfo[0].submergedVertex, submergeInfo[0].waveTriangle.point,submergeInfo[0].waveTriangle.normal);

			if (flag)
			{

				submergedArea = areaOfTriangle(intersectionA, intersectionB, submergeInfo[0].submergedVertex);
				assert(submergedArea <= currentBoatTriangle->area);

				currentBoatTriangle->color = black;
			}
		}
		else if (nSubmerged == 2)
		{
			//Vector3D<> intersectionA = intersectPoint(submergeInfo[0].submergedVertex - submergeInfo[2].submergedVertex, submergeInfo[2].submergedVertex, submergeInfo[2].waveTriangle.normal, submergeInfo[2].waveTriangle.point);
			//Vector3D<> intersectionB = intersectPoint(submergeInfo[1].submergedVertex - submergeInfo[2].submergedVertex, submergeInfo[2].submergedVertex, submergeInfo[2].waveTriangle.normal, submergeInfo[2].waveTriangle.point);

			Vector3D<> intersectionA; 
			Vector3D<> intersectionB;

			bool flag = planeLineIntersection(&intersectionA, submergeInfo[2].submergedVertex, submergeInfo[0].submergedVertex - submergeInfo[2].submergedVertex, submergeInfo[2].waveTriangle.point, submergeInfo[2].waveTriangle.normal);
			flag &= planeLineIntersection(&intersectionA, submergeInfo[2].submergedVertex, submergeInfo[1].submergedVertex - submergeInfo[2].submergedVertex, submergeInfo[2].waveTriangle.point,submergeInfo[2].waveTriangle.normal);

			if (flag)
			{
				submergedArea = areaOfTriangle(intersectionA, intersectionB, submergeInfo[0].submergedVertex);
				assert(submergedArea <= currentBoatTriangle->area);

				currentBoatTriangle->color = black;
			}
		}
		else if (nSubmerged == 3)
		{
			submergedArea = currentBoatTriangle->area;

			currentBoatTriangle->color = white;
		}
		
		if (submergedArea <= currentBoatTriangle->area)
		{
			//float projectionArea = 10*submergedArea * (1-(currentBoatTriangle->normal.dotProduct(vector3D(0, 0, -1)))/(currentBoatTriangle->normal.abs()));
			//*resultantForce = *resultantForce + vector3D(0, 0, (currentBoatTriangle->normal.z))*projectionArea*submergeDepth;
			float force = (currentBoatTriangle->normal.z)*submergedArea*submergeDepth;

			resultantForce->z = resultantForce->z + force;

			int redness = force/50*255;
			currentBoatTriangle->color = MakeColor((unsigned int)redness,0,0,255);
			//TORQUE CALCULATION:

			Vector3D<> center = boat->transformedPoints[currentBoatTriangle->pointIndex[0]];
			center = center + boat->transformedPoints[currentBoatTriangle->pointIndex[1]];
			center = center + boat->transformedPoints[currentBoatTriangle->pointIndex[2]];
			center = center / 3;

			float theta = acosf(center.dotProduct(currentBoatTriangle->normal) / (center.abs()*currentBoatTriangle->normal.abs()));
			Vector3D<> dTorque = (currentBoatTriangle->normal*currentBoatTriangle->area*center.abs())*sinf(theta);

			*resultantTorque = *resultantTorque + dTorque;
		}
	}

	findSubmergedTriangles2(resultantForce, resultantTorque, boat, wave, currentBoatTriangle->neighborOfEdge0_1, currentWaveTriangle);
	findSubmergedTriangles2(resultantForce, resultantTorque, boat, wave, currentBoatTriangle->neighborOfEdge1_2, currentWaveTriangle);
	findSubmergedTriangles2(resultantForce, resultantTorque, boat, wave, currentBoatTriangle->neighborOfEdge2_0, currentWaveTriangle);

	return;
}


/*
point 
direction

normal
corner

*/

void findSubmergedTriangles(WAVE* wave,int nWave, float time,float *resultantForce, Vector3D<> *resultantTorque, OBJECT3D *boat, G_TRIANGLE *currentBoatTriangle)
{
	if (currentBoatTriangle->visited)
		return;

	currentBoatTriangle->visited = true;

	Vector3D<> boatTriangleCenter = boat->transformedPoints[currentBoatTriangle->pointIndex[0]];
	boatTriangleCenter = boatTriangleCenter + boat->transformedPoints[currentBoatTriangle->pointIndex[1]];
	boatTriangleCenter = boatTriangleCenter + boat->transformedPoints[currentBoatTriangle->pointIndex[2]];
	boatTriangleCenter = boatTriangleCenter / 3;
	boatTriangleCenter = boatTriangleCenter + boat->position;
	 
	float waveAltitude = waveFunction(boatTriangleCenter.x, boatTriangleCenter.y,time,wave,nWave).z;
	float triangleAltitude = boatTriangleCenter.z;

	float submergeDepth = waveAltitude - boatTriangleCenter.z;

	currentBoatTriangle->color = black;

	if (submergeDepth > 0 && (currentBoatTriangle->normal.z < 0))
	{
		//submerged

		//force calculation
		float force = submergeDepth/100;
		*resultantForce += force;
		
		// torque calculation
		
		Vector3D<> F = vector3D(0, 0, force);
		
		float angle = acosf(boatTriangleCenter.dotProduct(F)/(boatTriangleCenter.abs()*F.abs()));

		Vector3D<> T = boatTriangleCenter.crossProduct(F)*sinf(angle);

		*resultantTorque = *resultantTorque + T;

		//color effect
		int redness = force * 100;
		currentBoatTriangle->color = MakeColor((unsigned int)redness, 0, 0, 255);
	}

	findSubmergedTriangles(wave,nWave, time,resultantForce, resultantTorque, boat, currentBoatTriangle->neighborOfEdge0_1);
	findSubmergedTriangles(wave, nWave, time,resultantForce , resultantTorque, boat,currentBoatTriangle->neighborOfEdge1_2);
	findSubmergedTriangles(wave, nWave, time,resultantForce , resultantTorque, boat, currentBoatTriangle->neighborOfEdge2_0);
}


void calculateBouyantForceAndTorque(float *resultantForce, Vector3D<> *resultantTorque, OBJECT3D *boat, OBJECT3D *oceanWave,WAVE*wave,int nWave,float time)
{
	//find the lowest triangle of the boat using linear search
	greedySearch_Plane.point = vector3D(0, 0, std::numeric_limits<float>::min());
	greedySearch_Plane.normal = vector3D(0, 0, 1);

	//first find using linear search
	static G_TRIANGLE *boatLowestTriangle = linearSearch_byShortestDisitance(boat, distanceToPlane);
	//then continue using greedy search which makes it much faster
	boatLowestTriangle = greedySearch_byShortestDistance(boat, boatLowestTriangle, distanceToPlane);

	G_TRIANGLE *boatLowestTriangle2 = boatLowestTriangle;

	//find the closest wave triagnle to the previously found triangle
	greedySearch_Point = boat->position + boat->transformedPoints[boatLowestTriangle2->pointIndex[0]];
	static G_TRIANGLE *closestTriangle = &(oceanWave->triangles[40]);
	closestTriangle = greedySearch_byShortestDistance(oceanWave, closestTriangle, distanceSquared);

	for (int i = 0; i < boat->triangles.size(); i++)
		boat->triangles[i].visited = false;

	// now we have 2 triangles. We need to use the neighbor pointers of each of them in order to find the list of triangles of the boat that are submerged

	*resultantForce = 0;
	*resultantTorque = {0,0,0};
	
	findSubmergedTriangles(wave,nWave,time,resultantForce, resultantTorque,boat, boatLowestTriangle2->neighborOfEdge0_1);
	findSubmergedTriangles(wave, nWave, time,resultantForce, resultantTorque,boat, boatLowestTriangle2->neighborOfEdge1_2);
	findSubmergedTriangles(wave, nWave, time,resultantForce, resultantTorque,boat, boatLowestTriangle2->neighborOfEdge2_0);
	//findSubmergedTriangles(resultantForce, resultantTorque, boat, oceanWave, boatLowestTriangle2->neighborOfEdge2_0->neighborOfEdge1_2, closestTriangle);
	//findSubmergedTriangles(resultantForce, resultantTorque, boat, oceanWave, boatLowestTriangle2->neighborOfEdge2_0->neighborOfEdge1_2->neighborOfEdge2_0, closestTriangle);
	//findSubmergedTriangles(resultantForce, resultantTorque, boat, oceanWave, boatLowestTriangle2->neighborOfEdge2_0->neighborOfEdge1_2->neighborOfEdge2_0->neighborOfEdge2_0, closestTriangle);
}
