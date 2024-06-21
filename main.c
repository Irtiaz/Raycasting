#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

#include "Vector.h"
#include "RaySegment.h"

#define M_PI 3.14159265358979323846

#define WIDTH 400
#define HEIGHT 400
#define FPS 60.0

#define FIELD_OF_VIEW M_PI / 3
#define NUMBER_OF_RAYS_IN_VISION_CONE 100

#define WALL_HEIGHT HEIGHT / 3
#define VISION_PLANE_DISTANCE 100

#define ANGLE_CHANGE_SPEED 0.01
#define MOVEMENT_SPEED 2

#define TILE_SIZE 100
#define COLUMNS 4
#define ROWS 4

#define WALL 1
#define EMPTY !WALL

void addSegment(Segment **segments, int *segmentsLength, int *segmentsCapacity, Segment segment);
Vector rayCast(Ray ray, Segment *segments, int segmentsLength);

void populateSegments(Segment **segments, int *segmentsLength, int *segmentsCapacity, int tilemap[ROWS][COLUMNS]);

int main(void) {
  SDL_Window *window;
  SDL_Renderer *renderer;

  int tilemap[ROWS][COLUMNS] = {
    {WALL, WALL, WALL, WALL},
    {WALL, EMPTY, EMPTY, WALL},
    {WALL, EMPTY, EMPTY, WALL},
    {WALL, WALL, WALL, WALL},
  };

  Vector previousClickPoint = {-1, -1};

  Vector position = {COLUMNS * TILE_SIZE / 2, ROWS * TILE_SIZE / 2};
  double heading = 0;

  char currentMode2D = 1;
  float currentAngleChangeSpeed = 0;
  float currentMovementSpeed = 0;

  Segment *segments;
  int segmentsLength = 0;
  int segmentsCapacity = 10;
  segments = malloc(segmentsCapacity * sizeof(Segment));

  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("RayCasting", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, -1, 0);

  populateSegments(&segments, &segmentsLength, &segmentsCapacity, tilemap);

  while (1) {
    SDL_Event event;

    char exit = 0;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
	exit = 1;
	break;
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN && currentMode2D) {
	Vector mouse;
	mouse.x = event.button.x;
	mouse.y = event.button.y;

	if (previousClickPoint.x == -1) {
	  previousClickPoint = mouse;
	}
	else {
	  Segment newSegment;
	  newSegment.point1 = previousClickPoint;
	  newSegment.point2 = mouse;
	  addSegment(&segments, &segmentsLength, &segmentsCapacity, newSegment);

	  previousClickPoint.x = -1;
	  previousClickPoint.y = -1;
	}
      }

      else if (event.type == SDL_KEYDOWN) {
	SDL_Keycode keyCode = event.key.keysym.sym;
	if (keyCode == SDLK_RIGHT) currentAngleChangeSpeed = ANGLE_CHANGE_SPEED;
	else if (keyCode == SDLK_LEFT) currentAngleChangeSpeed = -ANGLE_CHANGE_SPEED;
	else if (keyCode == SDLK_SPACE) {
	  currentMode2D = !currentMode2D;
	}

	else if (keyCode == SDLK_UP) {
	  currentMovementSpeed = MOVEMENT_SPEED;
	}
      }

      else if (event.type == SDL_KEYUP) {
	SDL_Keycode keyCode = event.key.keysym.sym;
	if (keyCode == SDLK_RIGHT || keyCode == SDLK_LEFT) currentAngleChangeSpeed = 0;
	else if (keyCode == SDLK_UP) currentMovementSpeed = 0;
      }
    }
    if (exit) break;

    heading += currentAngleChangeSpeed;
    {
      Vector headingVector;
      headingVector.x = cos(heading);
      headingVector.y = sin(heading);
      headingVector = VectorMultiply(headingVector, currentMovementSpeed);

      if (headingVector.x != 0 || headingVector.y != 0) {
	Ray ray;
	Vector closestIntersectionPoint;

	ray.startPoint = position;
	ray.castAngle = heading;

	closestIntersectionPoint = rayCast(ray, segments, segmentsLength);
	
	if (VectorDistanceSquared(closestIntersectionPoint, position) > currentMovementSpeed * currentMovementSpeed) position = VectorAdd(position, headingVector);
	else {
	  Vector correctionVector = VectorNormalize(headingVector);
	  correctionVector = VectorMultiply(correctionVector, -TILE_SIZE / 10);
	  position = VectorAdd(closestIntersectionPoint, correctionVector);
	}
      }
    }

    SDL_SetRenderDrawColor(renderer, 51, 51, 51, 255);
    SDL_RenderClear(renderer);

    if (currentMode2D) {
      SDL_SetRenderDrawColor(renderer, 0, 137, 255, 255);
      {
	SDL_Rect wall;
	int row;
	wall.w = wall.h = TILE_SIZE;
	for (row = 0; row < ROWS; ++row) {
	  int column;
	  for (column = 0; column < COLUMNS; ++column) {
	    if (tilemap[row][column] == WALL) {
	      wall.x = column * TILE_SIZE - position.x + WIDTH / 2;
	      wall.y = row * TILE_SIZE - position.y + HEIGHT / 2;

	      SDL_RenderFillRect(renderer, &wall);
	    }
	  }
	}
      }

      {
	Ray ray;
	int rayCounter;
	float dTheta = FIELD_OF_VIEW / NUMBER_OF_RAYS_IN_VISION_CONE;

	ray.startPoint = position;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (ray.castAngle = heading - FIELD_OF_VIEW / 2, rayCounter = 0; ray.castAngle <  heading + FIELD_OF_VIEW / 2; ray.castAngle += dTheta, ++rayCounter) {
	  Vector closestIntersectionPoint = rayCast(ray, segments, segmentsLength);
	  SDL_RenderDrawLine(renderer, WIDTH / 2, HEIGHT / 2, closestIntersectionPoint.x - position.x + WIDTH / 2, closestIntersectionPoint.y - position.y + HEIGHT / 2);
	}
      }
    }

    else {
      Ray ray;
      int rayCounter;
      int segmentCounter;
      float dTheta = FIELD_OF_VIEW / NUMBER_OF_RAYS_IN_VISION_CONE;
      float shortestDistanceSquared;

      ray.startPoint = position;

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

      shortestDistanceSquared = -1;

      for (segmentCounter = 0; segmentCounter < segmentsLength; ++segmentCounter) {
	for (ray.castAngle = heading - FIELD_OF_VIEW / 2, rayCounter = 0; ray.castAngle < heading + FIELD_OF_VIEW / 2; ray.castAngle += dTheta, ++rayCounter) {
	  Vector intersectionPoint = raySegmentIntersection(segments[segmentCounter], ray);
	  if (intersectionPoint.x != -1) {
	    float distanceSquared = VectorDistanceSquared(ray.startPoint, intersectionPoint);
	    if (shortestDistanceSquared == -1 || distanceSquared < shortestDistanceSquared) shortestDistanceSquared = distanceSquared;
	  }
	}

	for (ray.castAngle = heading - FIELD_OF_VIEW / 2, rayCounter = 0; ray.castAngle < heading + FIELD_OF_VIEW / 2; ray.castAngle += dTheta, ++rayCounter) {
	  Vector intersectionPoint = raySegmentIntersection(segments[segmentCounter], ray);
	  if (intersectionPoint.x != -1) {
	    float distance = sqrt(shortestDistanceSquared);
	    float heightOfStrip = VISION_PLANE_DISTANCE * WALL_HEIGHT / distance;
	    float widthOfStrip = WIDTH / NUMBER_OF_RAYS_IN_VISION_CONE;

	    SDL_FRect strip;
	    strip.x = widthOfStrip * rayCounter;
	    strip.y = HEIGHT / 2 - heightOfStrip / 2;
	    strip.w = widthOfStrip;
	    strip.h = heightOfStrip;
	    SDL_RenderFillRectF(renderer, &strip);
	  }
	}
      }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(1000.0 / FPS);
  }

  free(segments);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

void addSegment(Segment **segments, int *segmentsLength, int *segmentsCapacity, Segment segment) {
  memcpy(&((*segments)[*segmentsLength]), &segment, sizeof(Segment));
  ++(*segmentsLength);
  if (*segmentsLength * 2 >= *segmentsCapacity) {
    *segmentsCapacity *= 2;
    *segments = realloc(*segments, (*segmentsCapacity) * sizeof(Segment));
  }
}

Vector rayCast(Ray ray, Segment *segments, int segmentsLength) {
  int i;
  float closestDistanceSquared = -1;
  Vector closestIntersectionPoint;
  for (i = 0; i < segmentsLength; ++i) {
    Vector intersectionPoint = raySegmentIntersection(segments[i], ray);
    if (intersectionPoint.x != -1) {
      float distance = VectorDistanceSquared(ray.startPoint, intersectionPoint);
      if (distance < closestDistanceSquared || closestDistanceSquared == -1) {
	closestDistanceSquared = distance;
	closestIntersectionPoint = intersectionPoint;
      }
    }
  }
  return closestIntersectionPoint;
}

void populateSegments(Segment **segments, int *segmentsLength, int *segmentsCapacity, int tilemap[ROWS][COLUMNS]) {
  int row;
  for (row = 0; row < ROWS; ++row) {
    int column;
    for (column = 0; column < COLUMNS; ++column) {
      if (tilemap[row][column] == WALL) {
	float x = TILE_SIZE * column;
	float y = TILE_SIZE * row;
	addSegment(segments, segmentsLength, segmentsCapacity, createSegment(x, y, x + TILE_SIZE, y));
	addSegment(segments, segmentsLength, segmentsCapacity, createSegment(x, y, x, y + TILE_SIZE));
	addSegment(segments, segmentsLength, segmentsCapacity, createSegment(x, y + TILE_SIZE, x + TILE_SIZE, y + TILE_SIZE));
	addSegment(segments, segmentsLength, segmentsCapacity, createSegment(x + TILE_SIZE, y + TILE_SIZE, x + TILE_SIZE, y));
      }
    }
  }
}
