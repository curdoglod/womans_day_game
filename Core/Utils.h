#pragma once
#include "math.h"
#include <iostream>

struct Vector2 {
	float x, y;


	Vector2() : x(0), y(0) {}
	/**
   X, Y
*/
	Vector2(float x, float y) : x(x), y(y) {}


	float length() const {
		return sqrt(x * x + y * y);
	}


	Vector2 operator+(const Vector2& other) const {
		return Vector2(x + other.x, y + other.y);
	}

	Vector2& operator+=(const Vector2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	Vector2 operator-(const Vector2& other) const {
		return Vector2(x - other.x, y - other.y);
	}

	Vector2& operator-=(const Vector2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	template<typename U>
	Vector2& operator+=(const U& other) {
		x += other;
		y += other;
		return *this;
	}

	template<typename U>
	Vector2& operator-=(const U& other) {
		x -= other;
		y -= other;
		return *this;
	}

	template<typename U>
	Vector2& operator*=(const U& other) {
		x *= other;
		y *= other;
		return *this;
	}

	template<typename U>
	Vector2& operator/=(const U& other) {
		x /= other;
		y /= other;
		return *this;
	}

	template<typename U>
	Vector2 operator*(const U& other) const {
		return Vector2(x * other, y * other);
	}

	template<typename U>
	Vector2 operator/(const U& other) const {
		return Vector2(x / other, y / other);
	}

	Vector2& operator=(const Vector2& other) {
		if (this == &other)
			return *this;

		x = other.x;
		y = other.y;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
		os << '(' << vec.x << ", " << vec.y << ')';
		return os;
	}
};
