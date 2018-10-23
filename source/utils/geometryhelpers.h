#ifndef __Geometry_Helpers__
#define __Geometry_Helpers__

#include "Angel.h"
#include "common.h"
#include <functional>
#include <cmath>
const double pi = 3.141592654;
mat2 rotation(float angle);
float* findParabola(vec2 pt0, vec2 pt1, vec2 pt2);
vec2* drawCurve(float start, float end, std::function<float(float)> cur, int num_pts);
bool isInsidePolygon(vec2 pt, vec2 *polygon, int l);
bool polygonsIntersect(vec2 *polygon1, vec2 *polygon2, int l1, int l2);
vec2 *smoothCorners(vec2 *polygon, int l);
bool isConvex(int index, vec2 *polygon, int l);
int findEar(vec2 *polygon, int l);
bool testEar(int index, vec2 *polygon, int l);
vec2* splitOffEar(int index, vec2 *polygon, int l);
vec2* triangulatePolygon(vec2 *polygon, int l);
#endif