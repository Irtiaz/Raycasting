#include "Vector.h"
#include "RaySegment.h"
#include <math.h>
#include <float.h>

#define NEAR0(float_expression) (float_expression) >= -FLT_EPSILON && (float_expression) <= FLT_EPSILON

Vector raySegmentIntersection(Segment segment, Ray ray) {
  Vector noneVector = {-1, -1};

  Vector q = segment.point1;
  Vector s = VectorSubtract(segment.point2, segment.point1);

  Vector p = ray.startPoint;
  Vector r;

  Vector qMinusP;
  float qMinusPCrossR, qMinusPCrossS;
  float rCrossS;

  float u, t;

  r.x = cos(ray.castAngle);
  r.y = sin(ray.castAngle);

  qMinusP = VectorSubtract(q, p);
  qMinusPCrossR = VectorCrossProduct(qMinusP, r);
  qMinusPCrossS = VectorCrossProduct(qMinusP, s);
  rCrossS = VectorCrossProduct(r, s);

  if (NEAR0(rCrossS)) {
    if (NEAR0(qMinusPCrossR)) {
      /* They are colinear */
      Vector qPlusSMinusP = VectorAdd(qMinusP, s);
      float u1 = VectorDivide(qMinusP, r);
      float u2 = VectorDivide(qPlusSMinusP, r);

      if (u1 < 0 && u2 < 0) {
	return noneVector;
      }

      else {
	float u;
	if (u1 < 0) u = u2;
	else if (u2 < 0) u = u1;
	else u = u1 < u2? u1 : u2;
	return VectorFollow(p, r, u);
      }
    }

    /* They are parallel */
    return noneVector;
  }

  u = qMinusPCrossR / rCrossS;
  t = qMinusPCrossS / rCrossS;

  if (t >= 0 && 0 <= u && u <= 1) {
    return VectorFollow(q, s, u);
  }

  /* They do not intersect */
  return noneVector;
}

Segment createSegment(float point1X, float point1Y, float point2X, float point2Y) {
  Segment segment;

  segment.point1.x = point1X;
  segment.point1.y = point1Y;

  segment.point2.x = point2X;
  segment.point2.y = point2Y;

  return segment;
}
