#ifndef RAY_H_2345
#define RAY_H_2345

#include "Vector.h"

typedef struct {
  Vector startPoint;
  double castAngle;
} Ray;

typedef struct {
  Vector point1;
  Vector point2;
} Segment;

Vector raySegmentIntersection(Segment segment, Ray ray);
Segment createSegment(float point1X, float point1Y, float point2X, float point2Y);

#endif
