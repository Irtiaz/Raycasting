#include "Vector.h"
#include <math.h>

#define ABS(x) ((x) >= 0? (x) : -(x))

Vector VectorMultiply(Vector vector, float scalar) {
  Vector result;
  result.x = vector.x * scalar;
  result.y = vector.y * scalar;

  return result;
}

float VectorDivide(Vector a, Vector b) {
  return ABS(a.x) < ABS(a.y)? a.y / b.y : a.x / b.x;
}

Vector VectorAdd(Vector a, Vector b) {
  Vector result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;

  return result;
}

Vector VectorSubtract(Vector a, Vector b) {
  Vector result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;

  return result;
}

float VectorCrossProduct(Vector a, Vector b) {
  return a.x * b.y - a.y * b.x;
}

Vector VectorFollow(Vector startPoint, Vector direction, float scalarMultiplier) {
  Vector scaledDirectionVector = VectorMultiply(direction, scalarMultiplier);
  Vector endLocation = VectorAdd(startPoint, scaledDirectionVector);
  return endLocation;
}

float VectorDistanceSquared(Vector a, Vector b) {
  return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

Vector VectorNormalize(Vector vector) {
  float magnitude = sqrt(vector.x * vector.x + vector.y * vector.y);
  VectorMultiply(vector, 1.0 / magnitude);

  return vector;
}
