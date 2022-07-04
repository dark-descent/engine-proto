#include "framework.hpp"
#include "Vector2.hpp"

Vector2::Vector2() : x(0.0f), y(0.0f) { }

Vector2::Vector2(float xy) : x(xy), y(xy) { }

Vector2::Vector2(float x, float y) : x(x), y(y) { }

Vector2::Vector2(const Vector2& v) : x(v.x), y(v.y) { }

Vector2::Vector2(Vector2&& v) : x(std::move(v.x)), y(std::move(v.y)) { }
