#pragma once

struct Vector2
{
	float x = 0.0f;
	float y = 0.0f;

	Vector2();
	Vector2(float both);
	Vector2(float x, float y);
	Vector2(const Vector2& v);
	Vector2(Vector2&& v);
};