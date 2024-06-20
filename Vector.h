#ifndef VECTOR_H_4356345
#define VECTOR_H_4356345


typedef struct {
  float x;
  float y;
} Vector;

Vector VectorMultiply(Vector vector, float scalar);
float VectorDivide(Vector a, Vector b);
Vector VectorAdd(Vector a, Vector b);
Vector VectorSubtract(Vector a, Vector b);
float VectorCrossProduct(Vector a, Vector b);
Vector VectorFollow(Vector startPoint, Vector direction, float scalarMultiplier);
float VectorDistanceSquared(Vector a, Vector b);
Vector VectorNormalize(Vector vector);

#endif
