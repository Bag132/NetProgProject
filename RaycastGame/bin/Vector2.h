#pragma once

#include <math.h>

class Vector2 {
public:
	float x;
	float y;

	Vector2();
	Vector2(float _x, float _y);

	Vector2 rotate(float a);
	Vector2 rotDeg(float deg);
	Vector2 flr();
	Vector2 unit();
	float length();
	float lengthSquared();
	bool oob(Vector2);

	friend Vector2 operator+(Vector2 v1, Vector2 v2);
	friend Vector2 operator+(Vector2 v, float d);

	friend Vector2 operator-(Vector2 v1, Vector2 v2);
	friend Vector2 operator-(Vector2 v, float d);

	friend Vector2 operator*(Vector2 v1, Vector2 v2);
	friend Vector2 operator*(Vector2 v, float d);

	friend Vector2 operator/(Vector2 v1, Vector2 v2);
	friend Vector2 operator/(Vector2 v, float d);

	friend bool operator==(Vector2 v1, Vector2 v2);
};

Vector2 operator+(Vector2 v1, Vector2 v2);
Vector2 operator+(Vector2 v, float d);

Vector2 operator-(Vector2 v1, Vector2 v2);
Vector2 operator-(Vector2 v, float d);

Vector2 operator*(Vector2 v1, Vector2 v2);
Vector2 operator*(Vector2 v, float d);

Vector2 operator/(Vector2 v1, Vector2 v2);
Vector2 operator/(Vector2 v, float d);

bool operator==(Vector2 v1, Vector2 v2);