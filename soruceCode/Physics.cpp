#include "Physics.h"
#include <map>
#include <time.h>
#include <thread>

void scaleObject(OBJECT3D* object, float scaleFactor)
{
	int n = object->originalPoints.size();
	for (int i = 0; i < n; i++)
	{
		object->originalPoints[i].x *= scaleFactor;
		object->originalPoints[i].y *= scaleFactor;
		object->originalPoints[i].z *= scaleFactor;
	}
}

void rotateVector(Vector3D<> *vect, float horizontalAngle)
{
	vect->x = cosf(horizontalAngle);
	vect->y = sinf(horizontalAngle);

	//vect->x*cosf(horizontalAngle*3.14 / 180.0f) + vect->y*sinf(horizontalAngle*3.14 / 180.0f);
	//vect->y*cosf(horizontalAngle*3.14 / 180.0f) - vect->x*sinf(horizontalAngle*3.14 / 180.0f);
}

bool doSlowly(unsigned long int *startTime, unsigned long int currentTime, float frequency)
{
	if (currentTime - *startTime > 1000 / frequency)
	{
		*startTime = currentTime;
		return true;
	}
	return false;
}

bool animateJump(float*altitude, unsigned long startTime, unsigned long currentTime, unsigned long totalDuration, double jumpHeight)
{
	unsigned long timeOffset = currentTime - startTime;

	if (timeOffset < 0 || timeOffset > totalDuration)
	{
		*altitude = 0;
		return false;
	}

	double xoffset = sqrtf(jumpHeight);

	*altitude = -powf((double)timeOffset*(2 * xoffset / (double)totalDuration) - xoffset, 2) + jumpHeight;

	return true;
}


/**
if point M is in rect ABCD, true is returned. false otherwise
*/
bool isInRect(Vector2D<> A, Vector2D<> B, Vector2D<> C, Vector2D<> D, Vector2D<> M)
{
	B = B - A;
	D = D - A;
	C = C - A;
	M = M - A;

	if ((M.y - D.y)*(D.x) < (D.y)*(M.x - D.x) &&
		(M.y - B.y)*(D.x) > (D.y)*(M.x - B.x) &&
		(M.y - B.y)*(B.x) > (B.y)*(M.x - B.x) &&
		(M.y - D.y)*(B.x) < (B.y)*(M.x - D.x))
		return true;

	return false;
}

Vector2D<> scanline(G_SPRITE* sprite, Vector2D<> pos1, Vector2D<> centera, G_COLOR32 CA, G_COLOR32 CB)
{
	float slope = (pos1.y - centera.y) / (pos1.x - centera.x);

	Vector2D<> point;

	if (slope > 1 || slope < -1)
	{
		if (pos1.y < centera.y)
		{
			//iterate by y axis
			for (int iy = pos1.y; iy < centera.y; iy++)
			{
				int valX = 0.5f + pos1.x + (iy - pos1.y) / slope;

				if (valX < 0 || valX >= sprite->width)
					continue;

				if (iy < 0 || iy >= sprite->height)
					continue;

				G_COLOR32 pixel = sprite->memory[valX + iy * sprite->width];

				if ((pixel.a <= max(CA.a, CB.a)) && (pixel.a >= min(CA.a, CB.a)) &&
					(pixel.r <= max(CA.r, CB.r)) && (pixel.r >= min(CA.r, CB.r)) &&
					(pixel.g <= max(CA.g, CB.g)) && (pixel.g >= min(CA.g, CB.g)) &&
					(pixel.b <= max(CA.b, CB.b)) && (pixel.b >= min(CA.b, CB.b)))
					return{ (float)valX, (float)iy };
			}
		}
		else
		{
			//iterate by y axis
			for (int iy = pos1.y; iy > centera.y; iy--)
			{
				int valX = 0.5f + pos1.x + (iy - pos1.y) / slope;

				if (valX < 0 || valX >= sprite->width)
					continue;

				if (iy < 0 || iy >= sprite->height)
					continue;

				G_COLOR32 pixel = sprite->memory[valX + iy * sprite->width];

				if ((pixel.a <= max(CA.a, CB.a)) && (pixel.a >= min(CA.a, CB.a)) &&
					(pixel.r <= max(CA.r, CB.r)) && (pixel.r >= min(CA.r, CB.r)) &&
					(pixel.g <= max(CA.g, CB.g)) && (pixel.g >= min(CA.g, CB.g)) &&
					(pixel.b <= max(CA.b, CB.b)) && (pixel.b >= min(CA.b, CB.b)))
					return{ (float)valX, (float)iy };
			}
		}
	}
	else
	{
		if (pos1.x < centera.x)
		{
			//iterate by x axis
			for (int ix = pos1.x; ix < centera.x; ix++)
			{
				int valY = 0.5f + (ix - pos1.x)*slope + pos1.y;

				if (ix < 0 || ix >= sprite->width)
					continue;

				if (valY < 0 || valY >= sprite->height)
					continue;

				G_COLOR32 pixel = sprite->memory[ix + valY * sprite->width];

				if ((pixel.a <= max(CA.a, CB.a)) && (pixel.a >= min(CA.a, CB.a)) &&
					(pixel.r <= max(CA.r, CB.r)) && (pixel.r >= min(CA.r, CB.r)) &&
					(pixel.g <= max(CA.g, CB.g)) && (pixel.g >= min(CA.g, CB.g)) &&
					(pixel.b <= max(CA.b, CB.b)) && (pixel.b >= min(CA.b, CB.b)))
					return{ (float)ix, (float)valY };
			}
		}
		else
		{
			//iterate by x axis
			for (int ix = pos1.x; ix > centera.x; ix--)
			{
				int valY = 0.5f + (ix - pos1.x)*slope + pos1.y;

				if (ix < 0 || ix >= sprite->width)
					continue;

				if (valY < 0 || valY >= sprite->height)
					continue;

				G_COLOR32 pixel = sprite->memory[ix + valY * sprite->width];
				if ((pixel.a <= max(CA.a, CB.a)) && (pixel.a >= min(CA.a, CB.a)) &&
					(pixel.r <= max(CA.r, CB.r)) && (pixel.r >= min(CA.r, CB.r)) &&
					(pixel.g <= max(CA.g, CB.g)) && (pixel.g >= min(CA.g, CB.g)) &&
					(pixel.b <= max(CA.b, CB.b)) && (pixel.b >= min(CA.b, CB.b)))
					return{ (float)ix, (float)valY };
			}
		}
	}

	return{ (float)(-1), (float)(-1) };
}


std::vector<Vector2D<>> circularConcaveHullScanner(G_SPRITE* sprite, float deltaAngle, bool display_scanlines, G_COLOR32 A, G_COLOR32 B)
{
	std::vector<Vector2D<>> nodes;
	Vector2D<> point;
	Vector2D<float> centerr;
	centerr.x = sprite->width / 2;
	centerr.y = sprite->height / 2;

	float radius = sqrtf((sprite->height / 2)*(sprite->height / 2) + (sprite->width / 2)*(sprite->width / 2));

	for (float angle = 0; angle < 2 * PI; angle += deltaAngle)
	{
		Vector2D<> outerPoint;
		outerPoint.x = radius * cosf(angle);
		outerPoint.y = radius * sinf(angle);
		outerPoint = centerr + outerPoint;

		point = scanline(sprite, outerPoint, centerr, A, B);

		//DrawLine(backBuffer, { outerPoint.x,300 - outerPoint.y }, { center.x, 300 - center.y }, blue);

		if (point.x > 0 && point.y > 0)
		{
			nodes.push_back(point);
		}
	}

	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i].y = sprite->height - nodes[i].y;
		nodes[i] = nodes[i] - centerr;
	}

	return nodes;
}

std::vector<Vector2D<>> convertConcaveToConvexHull(std::vector<Vector2D<>> Vertices)
{
	//find the greatest y valued point in Vertices list
	Vector2D<> referencePoint = { 0, -INFINITY }; //greatest oordinate value

	int referencePointIndex;
	for (int i = 0; i < Vertices.size(); i++)
	{
		if (Vertices[i].y > referencePoint.y)
		{
			referencePoint = Vertices[i];
			referencePointIndex = i;
		}
	}

	for (int i = 0; i <= referencePointIndex; i++)
	{
		Vertices.push_back(Vertices[0]);
		Vertices.erase(Vertices.begin());
	}

	std::vector<Vector2D<>> convexHull;

	if (Vertices.size() < 2)
	{
		return Vertices;
	}
	else
	{
		convexHull.push_back(Vertices[0]);
		convexHull.push_back(Vertices[1]);
		convexHull.push_back(Vertices[2]);

		int listSize = Vertices.size();

		for (int i = 3; i < listSize; i++)
		{
			while ((Vertices[i].y - convexHull[convexHull.size() - 1].y) *	(convexHull[convexHull.size() - 1].x - convexHull[convexHull.size() - 2].x) >=
				(convexHull[convexHull.size() - 1].y - convexHull[convexHull.size() - 2].y) * (Vertices[i].x - convexHull[convexHull.size() - 1].x))
			{
				convexHull.pop_back();

				if (convexHull.size() < 2)
					break;

			}

			convexHull.push_back(Vertices[i]);
		}
	}

	return convexHull;
}

Vector2D<> Support(G_OBJECT *object_A, G_OBJECT *object_B, Vector2D<> D)
{
	std::map<float, Vector2D<>> points1, points2;
	for (Vector2D<> &point : object_A->convexHull())
		points1.insert(std::make_pair((point).dotProduct(D), (point + object_A->pos)));
	for (Vector2D<> &point : object_B->convexHull())
		points2.insert(std::make_pair((point).dotProduct(-D), (point + object_B->pos)));
	return points1.rbegin()->second - points2.rbegin()->second;
}

inline static bool insideTriangle(Vector2D<> *p1, Vector2D<> *p2, Vector2D<> *p3)
{
	bool a = ((p1->x*(p1->y - p2->y) + p1->y*(p2->x - p1->x)) > 0);
	bool b = ((p2->x*(p2->y - p3->y) + p2->y*(p3->x - p2->x)) > 0);
	bool c = ((p3->x*(p3->y - p1->y) + p3->y*(p1->x - p3->x)) > 0);
	return (a == b) && (b == c);
}


bool GJK(G_OBJECT *object_A, G_OBJECT *object_B)
{
	auto D = object_B->pos - object_A->pos;
	Vector2D<> list[3] = { Support(object_A, object_B, D), Support(object_A, object_B, -D), Vector2D<>() };

	if (list[0].dotProduct(list[1]) > 0)
		return false;

	auto AB = (list[1] - list[0]);
	Vector2D<> temp = AB.perp_right()*((AB.determinant(-list[0]) > 0) * 2 - 1);
	list[2] = Support(object_A, object_B, temp);
	while (!insideTriangle(&list[0], &list[1], &list[2]))
	{
		Vector2D<> BC = list[2] - list[1];///BC
		Vector2D<> AC = list[2] - list[0];///AC

		if (list[2].dotProduct(list[0]) < 0)///COA > 90
		{
			list[1] = list[2];
			D = AC.perp_right()*((AC.determinant(-list[0]) > 0) * 2 - 1);//turnToOrigin
			list[2] = Support(object_A, object_B, D);
			if (list[1] == list[2] || D.dotProduct(list[2]) < 0 || AC.determinant(-list[0]) == 0) return false;
		}
		else if (list[2].dotProduct(list[1]) < 0) ///COB > 90
		{
			list[0] = list[2];
			D = BC.perp_right()*((BC.determinant(-list[1]) > 0) * 2 - 1);
			list[2] = Support(object_A, object_B, D);
			if (list[0] == list[2] || D.dotProduct(list[2]) < 0 || BC.determinant(-list[1]) == 0) return false;
		}
		else
			return false;
	}
	return true;
}

bool EEE(G_OBJECT *object_A, G_OBJECT *object_B)
{
	auto movementVectorA = object_A->pos - object_A->oldPos;
	auto movementVectorB = object_B->pos - object_B->oldPos;
	auto BRelativeToA = movementVectorB - movementVectorA;
	if (BRelativeToA.x == 0 && BRelativeToA.y == 0) return false;
	if (Support(object_A, object_B, -BRelativeToA).dotProduct(-BRelativeToA) < 0) return true;
	else return false;
}

/*
//make the list unique
std::unique(Vertices.begin(), Vertices.end(), areEqual);

//find the greatest y valued point in Vertices list
referencePoint = {0,0}; //greatest oordinate value

int referencePointIndex;
for (int i = 0; i < Vertices.size(); i++)
{
if (Vertices[i].y > referencePoint.y)
{
referencePoint = Vertices[i];
referencePointIndex = i;
}
}

//remove the referece point from Vertices list
Vertices.erase(Vertices.begin() + referencePointIndex);

//sort the list using reference point
std::sort(Vertices.begin(), Vertices.end(),y_IsGreater);

*/

Vector2D<> support(Vector2D<> normal, std::vector<Vector2D<>> A, std::vector<Vector2D<>> B, int* index_A = NULL, int* index_B = NULL)
{
	int maxIndex_A = 0;
	float maxDotProduct = normal.dotProduct(A[0]);

	for (int i = 1; i < A.size(); i++)
	{
		float dotProduct = normal.dotProduct(A[i]);

		if (dotProduct > maxDotProduct)
		{
			maxIndex_A = i;
			maxDotProduct = dotProduct;
		}
	}

	if (index_A != NULL)
		*index_A = maxIndex_A;

	int maxIndex_B = 0;
	maxDotProduct = normal.dotProduct(B[0]);

	for (int i = 1; i < B.size(); i++)
	{
		float dotProduct = normal.dotProduct(B[i]);

		if (dotProduct < maxDotProduct)
		{
			maxIndex_B = i;
			maxDotProduct = dotProduct;
		}
	}

	if (index_B != NULL)
		*index_B = maxIndex_B;

	Vector2D<> point = A[maxIndex_A] - B[maxIndex_B];

	return point;
}

int findFurthestPoint(std::vector<Vector2D<>> *A, Vector2D<> normal)
{
	int maxDistIndex = 0;
	float maxDist = A->at(0).dotProduct(normal);

	int listSize = A->size();
	for (int i = 1; i < listSize; i++)
	{
		float dist = A->at(i).dotProduct(normal);

		if (dist > maxDist)
		{
			maxDist = dist;
			maxDistIndex = i;
		}
	}

	return maxDistIndex;
}

bool funct(Vector2D<> A, Vector2D<> B)
{
	return (A.abs() < B.abs());
}


Vector2D<> intersectionOfLines(Vector2D<> c, Vector2D<> b, Vector2D<> point)
{
	Vector2D<> m;

	if (b.x - c.x == 0)
	{
		m.x = b.x;
		m.y = b.x* point.y / point.x;
		return m;
	}

	if (c.y - b.y == 0)
	{
		m.y = b.y;
		m.x = b.y * point.x / point.y;
		return m;
	}

	float partA = point.x * (b.x*c.y - b.y*c.x);
	float partB = ((b.x - c.x)*point.y + (c.y - b.y)*point.x);


	m.x = partA / partB;

	if (partB == 0)
	{
		int yy = 0;
	}

	if (point.x == 0)
	{
		m.y = point.y;
	}
	else
	{
		m.y = point.y*m.x / point.x;
	}

	return m;
}

//#define G_DEBUG

void collisionResponse(G_PIXELSURFACE backBuffer, G_OBJECT *object_A, G_OBJECT *object_B)
{
	Vector2D<> intr;

	Vector2D<> dist_A = object_A->pos - object_A->oldPos;
	Vector2D<> dist_B = object_B->pos - object_B->oldPos;
	Vector2D<> dist_R = dist_A - dist_B;

	if (dist_R.x == 0 && dist_R.y == 0)
		return;

	int size_A = object_A->convexHull().size();
	int size_B = object_B->convexHull().size();

	int indexA, indexB;
	Vector2D<> firstScanPoint = support(dist_R, object_A->convexHull(), object_B->convexHull(), &indexA, &indexB);

	std::vector<Vector2D<>> B = object_B->convexHull();
	for (int i = 0; i < size_B; i++)
		B[i] = B[i].negative();


#ifdef G_DEBUG
	Vector2D<> rr = { 340, 240 };
	DrawLine(backBuffer, dist_R + rr, rr, blue);

	Vector2D<> origin = object_A->pos - object_B->pos + rr;

	displayPolygon(backBuffer, object_A->convexHull, origin, white, white, 1, 4);
	displayPolygon(backBuffer, B, origin + object_A->convexHull[indexA], white, white, 1, 4);
	//DrawCircle(backBuffer, origin + firstScanPoint , 4, blue, true);
	//DrawCircle(backBuffer, origin + object_A->convexHull[indexA], 4, green, true);
#endif

	float regionTest = (object_A->pos - object_B->pos + firstScanPoint).dotProduct(dist_R.perp_right());

	if (regionTest >= 0)//go to left
	{
		Vector2D<> a1;
		if (indexA == 0)
			a1 = object_A->convexHull()[size_A - 1];
		else
			a1 = object_A->convexHull()[(indexA - 1) % size_A];

		Vector2D<> a2 = object_A->convexHull()[(indexA) % size_A];

		Vector2D<> normal = (a2 - a1).perp_right();

		int index_B_first = findFurthestPoint(&B, normal);
		int index_B_last;

#ifdef G_DEBUG
		DrawCircle(backBuffer, origin + object_A->convexHull[indexA] + B[index_B_first], 6, white, true);
		DrawCircle(backBuffer, origin + a1, 6, white, true);
		DrawCircle(backBuffer, origin + a2, 6, black, true);
#endif
		Vector2D<> oldPoint = firstScanPoint;
		Vector2D<> newPoint;

		for (int i = indexA; i < size_A + indexA; i++)
		{
			//edge i: a1->a2
			a1 = object_A->convexHull().at((i) % (size_A));
			a2 = object_A->convexHull().at((i + 1) % (size_A));

			//normal of edge i
			normal = (a2 - a1).perp_right();

			index_B_last = findFurthestPoint(&B, normal);

			int target = index_B_last;

			if (target < index_B_first)
				target += size_B;

			for (int t = index_B_first; t <= target; t++)
			{
				newPoint = object_A->pos - object_B->pos + a1 + B.at(t%size_B);

				regionTest = (newPoint).dotProduct(dist_R.perp_right());

				if (regionTest < 0)
					goto outOfLoops1;

				oldPoint = newPoint;
			}

			index_B_first = index_B_last;
		}

	outOfLoops1:

		intr = intersectionOfLines(newPoint, oldPoint, dist_R);
		Vector2D<> surface = newPoint - oldPoint;

		float verticalDepth = intr.dotProduct(surface.perp_right());

		if (verticalDepth < 0)
		{
			//no collision
		//	return;
		}

		float surafeLength = surface.abs();

		verticalDepth /= surafeLength;

		Vector2D<> pen = (surface.perp_right());
		pen.toUnitVect();
		pen = pen * verticalDepth;

		object_B->pos = object_B->pos + pen;

#ifdef G_DEBUG
		DrawCircle(backBuffer, rr + oldPoint, 4, red, true);
		DrawCircle(backBuffer, rr + newPoint, 4, white, true);
		DrawCircle(backBuffer, rr + intr, 4, white, true);
#endif
	}
	else if (regionTest < 0)//go to right
	{
		//DrawCircle(backBuffer, origin, 15, green, true);

		Vector2D<> a1 = object_A->convexHull()[(indexA + 1) % size_A];
		Vector2D<> a2 = object_A->convexHull()[(indexA) % size_A];

		//DrawCircle(backBuffer, origin + a1, 6, white, true);
		//DrawCircle(backBuffer, origin + a2, 6, black, true);

		Vector2D<> normal = (a2 - a1).perp_left();

		int index_B_first = findFurthestPoint(&B, normal);
		int index_B_last;

		//DrawCircle(backBuffer, origin + object_A->convexHull[indexA] + B[index_B_first], 6, white, true);

		Vector2D<> oldPoint = firstScanPoint;
		Vector2D<> newPoint;

		for (int i = indexA + size_A; i > indexA; i--)
		{
			//edge i: a1->a2
			int k2 = i;
			while (k2 < 0) k2 += size_A;

			a1 = object_A->convexHull().at(k2 % (size_A));

			k2 = i - 1;
			while (k2 < 0) k2 += size_A;

			a2 = object_A->convexHull().at(k2 % (size_A));

			//normal of edge i
			normal = (a2 - a1).perp_left();

			index_B_last = findFurthestPoint(&B, normal);

			int target = index_B_last;

			if (target > index_B_first)
				target = target - size_B;

			for (int t = index_B_first; t >= target; t--)
			{
				int k = t;
				while (k < 0) k += size_B;

				newPoint = object_A->pos - object_B->pos + B.at(k%size_B) + a1;
				//DrawCircle(backBuffer, rr + newPoint , 4, black, true);

				regionTest = (newPoint).dotProduct(dist_R.perp_left());

				if (regionTest < 0)
					goto outOfLoops2;

				oldPoint = newPoint;
			}
			index_B_first = index_B_last;
		}
	outOfLoops2:

		intr = intersectionOfLines(newPoint, oldPoint, dist_R);

		Vector2D<> surface = newPoint - oldPoint;

		float verticalDepth = intr.dotProduct(surface.perp_right());

		if (verticalDepth > 0)
		{
			//no collision
		//	return;
		}

		float surafeLength = surface.abs();

		verticalDepth /= surafeLength;

		Vector2D<> pen = (surface.perp_right());

		pen.toUnitVect();

		pen = pen * verticalDepth;
		object_B->pos = object_B->pos + pen;

#ifdef G_DEBUG
		DrawCircle(backBuffer, rr + newPoint, 4, green, true);
		DrawCircle(backBuffer, rr + oldPoint, 4, red, true);
		DrawCircle(backBuffer, rr + intr, 4, white, true);
#endif
	}
	else
	{
		//firstScanPoint;
		return;
	}
}

/*
std::vector<Vector2D<>> minskowskiSum(std::vector<Vector2D<>> *A, std::vector<Vector2D<>> *B) // B around A
{
	std::vector<Vector2D<>> list;

	int size_A = A->size();
	int size_B = B->size();

	Vector2D<> a1 = A->at(size_A - 1);
	Vector2D<> a2 = A->at(0);

	Vector2D<> normal = (a2 - a1).perp_right();

	int index_B_first, index_B_last;
	index_B_first = findFurthestPoint(B, normal);

	for (int i = 0; i < size_A; i++)
	{
		a1 = A->at((i) % (size_A));
		a2 = A->at((i + 1) % (size_A));

		normal = (a2 - a1).perp_right();

		index_B_last = findFurthestPoint(B, normal);

		int target = index_B_last;

		if (target < index_B_first)
			target += size_B;

		for (int t = index_B_first; t <= target; t++)
		{
			Vector2D<> point = B->at(t%size_B) + a1;
			list.push_back(point);
		}

		index_B_first = index_B_last;
	}

	return list;
}

std::vector<Vector2D<>> minskowskiDifference(std::vector<Vector2D<>> *A, std::vector<Vector2D<>> *B) // B around A
{
	std::vector<Vector2D<>> list;

	std::vector<Vector2D<>> B_modified = *B;

	int size_A = A->size();
	int size_B = B_modified.size();

	for (int i = 0; i < size_B; i++)
		B_modified.at(i) = B_modified.at(i).negative();

	Vector2D<> a1 = A->at(size_A - 1);
	Vector2D<> a2 = A->at(0);

	Vector2D<> normal = (a2 - a1).perp_right();

	int index_B_first, index_B_last;
	index_B_first = findFurthestPoint(&B_modified, normal);

	for (int i = 0; i < size_A; i++)
	{
		//edge i: a1->a2
		a1 = A->at((i) % (size_A));
		a2 = A->at((i + 1) % (size_A));

		//normal of edge i
		normal = (a2 - a1).perp_right();

		index_B_last = findFurthestPoint(&B_modified, normal);

		int target = index_B_last;

		if (target < index_B_first)
			target += size_B;

		for (int t = index_B_first; t <= target; t++)
		{
			Vector2D<> point = B_modified.at(t%size_B) + a1;
			list.push_back(point);
		}

		index_B_first = index_B_last;
	}

	return list;
}
*/
void setPosition(G_OBJECT *object, Vector2D<> newPosition)
{
	object->oldPos = object->pos;
	object->pos = newPosition;
}

inline void move(G_OBJECT *object, Vector2D<> displacement)
{
	object->oldPos = object->pos;
	object->pos = object->pos + displacement;
}

Vector2D<> randPos(int screenWidth, int screenHeight)
{
	float y = rand() % (screenHeight + 1), x = rand() % (screenWidth + 1);
	return{ x,y };
}


std::vector<G_RECT> mergeSort(std::vector<G_RECT> list)
{
	if (list.size() == 1)
		return list;
	std::vector<G_RECT> sortedGroupA(list.begin(), list.begin() + (int)(list.size() / 2));
	std::vector<G_RECT> sortedGroupB(list.begin() + (int)(list.size() / 2), list.end());
	sortedGroupA = mergeSort(sortedGroupA);
	sortedGroupB = mergeSort(sortedGroupB);
	std::vector<G_RECT> ans;
	while (sortedGroupA.size() > 0 && sortedGroupB.size() > 0)
	{
		if (sortedGroupA[0].distance > sortedGroupB[0].distance)
		{
			ans.push_back(sortedGroupA[0]);
			sortedGroupA.erase(sortedGroupA.begin());
		}
		else
		{
			ans.push_back(sortedGroupB[0]);
			sortedGroupB.erase(sortedGroupB.begin());
		}
	}
	ans.insert(ans.end(), sortedGroupA.begin(), sortedGroupA.end());
	ans.insert(ans.end(), sortedGroupB.begin(), sortedGroupB.end());
	return ans;
}

std::vector<G_RECT>  preMergeSort(std::vector<G_RECT> list, Vector3D<> cameraPos)
{
	for (auto &x : list)
		x.distance = (x.position - cameraPos).abs();
	return mergeSort(list);
}

Vector2D<> make_Vector2D(float x, float y)
{
	return { x, y };
}

std::pair<Vector2D<>, Vector2D<>> G_RECT::getScreenCoordinates(Vector3D<> cameraPos)
{
	Vector2D<> min = { 0 };
	Vector2D<> max = { 0 };
	if (position.y < cameraPos.y)
		return std::make_pair(min, max);
	float distance = (position - cameraPos).abs(true, true, false);
	alpha = EYE_CONST / distance;
	Vector3D<> newSize = size * alpha;
	Vector3D<> FPV3DPosition = (position - cameraPos)*alpha;
	Vector2D<> FPV2DPosition = { FPV3DPosition.x, -FPV3DPosition.y*tanf(WORLD_SLOPE) - FPV3DPosition.z };
	Vector2D<> centerb;
	min = FPV2DPosition - make_Vector2D(newSize.x, newSize.z) / 2 + centerb;
	max = FPV2DPosition + make_Vector2D(newSize.x, newSize.z) / 2 + centerb;
	return std::make_pair(min, max);
}

G_OBJECT::G_OBJECT(std::vector<std::string> imagePaths)
{
	for (int i = 0; i < imagePaths.size(); i++)
	{
		images.push_back(loadSprite(imagePaths.at(i).c_str()));
		sizes.push_back(floatVector(images[i].width, images[i].height));
		//concaveHulls.push_back(circularConcaveHullScanner(&(images.at(i)), 0.02f, false, MakeColor(0, 0, 200, 255), MakeColor(111, 111, 255, 255)));
		//convexHulls.push_back(convertConcaveToConvexHull(concaveHulls.at(i)));
		double max1 = 0, max2 = 0;
		Vector2D<> maxpoint1, maxpoint2;
		/*
		for (auto &point : convexHulls.at(i))
		{
		if (point.dotProduct({ 1, 1 }) > max1)
		{
		max1 = point.dotProduct({ 1, 1 });
		maxpoint1 = point;
		}
		if (point.dotProduct({ -1, -1 }) > max2)
		{
		max2 = point.dotProduct({ -1, -1 });
		maxpoint2 = point;
		}
		radius = point.abs() > radius ? point.abs() : radius;
		}
		*/
		objBoxes.push_back(std::make_pair(maxpoint1, maxpoint2));
	}
}



void drawObj(G_PIXELSURFACE frameBuffer, Vector3D<> position, Vector3D<> cameraPos, std::vector<G_TRIANGLE> triangles)
{
	/*
	float distance = (position - cameraPos).abs(true, true, false);
	auto alpha = EYE_CONST / distance;
	extern Vector2D<> center;
	for (auto &x : triangles)
	{
		if (position.y < cameraPos.y)
			break;
		float t = 0.5;
		extern float objOffset;
		auto ref = center + make_Vector2D(0, objOffset*alpha);
		auto p1 = x.point[0] * alpha*t;
		auto p2 = x.point[1] * alpha*t;
		auto p3 = x.point[2] * alpha*t;
		auto A = abs(p1.x*p2.y + p2.x*p3.y + p3.x*p1.y - p1.x*p3.y - p2.x*p1.y - p3.x*p2.y)*alpha*alpha;
		if (A < 3)
			continue;
		
		Vector3D<> temp = { 0,0,1 };
		auto N = (p2 - p1).crossProduct(p3 - p1);
		if (temp.dotProduct(N) < 0)
			continue;
		Vector3D<> FPV3DPosition = (position - cameraPos)*alpha;
		Vector2D<> FPV2DPosition = { FPV3DPosition.x, -FPV3DPosition.y*tanf(WORLD_SLOPE) - FPV3DPosition.z };
		DrawTriangle(&frameBuffer,NULL,NULL,
		FPV2DPosition + make_Vector2D(p1.x, -p1.z*tanf(WORLD_SLOPE) - p1.y)*alpha + ref,
		FPV2DPosition + make_Vector2D(p2.x, -p2.z*tanf(WORLD_SLOPE) - p2.y)*alpha + ref,
		FPV2DPosition + make_Vector2D(p3.x, -p3.z*tanf(WORLD_SLOPE) - p3.y)*alpha + ref, red);

		
	}*/
}

