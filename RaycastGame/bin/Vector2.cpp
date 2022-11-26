#include "Vector2.h"

Vector2::Vector2() {
	x = 0;
	y = 0;
}
Vector2::Vector2(float _x, float _y) {
	x = _x;
	y = _y;
}

Vector2 Vector2::rotate(float r) {
	return Vector2(cos(r) * x - sin(r) * y, sin(r) * x + cos(r) * y);
}
Vector2 Vector2::rotDeg(float deg) {
	return rotate(3.14159265359 * deg / 180.0);
}

Vector2 Vector2::flr() {
	return Vector2(floor(x), floor(y));
}

Vector2 Vector2::unit() {
	return Vector2(x, y) / length();
}

float Vector2::length() {
	return sqrt(pow(x, 2) + pow(y, 2));
}

float Vector2::lengthSquared() {
	return pow(x, 2) + pow(y, 2);
}

bool Vector2::oob(Vector2 bound) {
	return x > bound.x || y > bound.y || x < 0 || y < 0;
}

Vector2 operator+(Vector2 v1, Vector2 v2) {
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}
Vector2 operator+(Vector2 v, float d) {
	return Vector2(v.x + d, v.y + d);
}

Vector2 operator-(Vector2 v1, Vector2 v2) {
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}
Vector2 operator-(Vector2 v, float d) {
	return Vector2(v.x - d, v.y - d);
}

Vector2 operator*(Vector2 v1, Vector2 v2) {
	return Vector2(v1.x * v2.x, v1.y * v2.y);
}
Vector2 operator*(Vector2 v, float d) {
	return Vector2(v.x * d, v.y * d);
}

Vector2 operator/(Vector2 v1, Vector2 v2) {
	return Vector2(v1.x / v2.x, v1.y / v2.y);
}
Vector2 operator/(Vector2 v, float d) {
	return Vector2(v.x / d, v.y / d);
}

bool operator==(Vector2 v1, Vector2 v2) {
	return (v1.x == v2.x) && (v1.y == v2.y);
}