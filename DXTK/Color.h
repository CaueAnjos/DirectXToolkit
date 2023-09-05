#pragma once
#include "Types.h"

struct ColorRGB
{
	uint8_t r, g, b;

	ColorRGB()
		: r(0), g(0), b(0)
	{}

	ColorRGB(uint8_t r, uint8_t g, uint8_t b)
		: r(r), g(g), b(b)
	{}

	COLORREF operator()()
	{
		return RGB(r, g, b);
	}
};

struct ColorRGBA
{
	float r, g, b, a;

	ColorRGBA()
		: r(0.f), g(0.f), b(0.f), a(0.f)
	{}

	ColorRGBA(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{}

	COLORREF operator()()
	{
		ColorRGB c;
		c.r = (uint8_t)(r * 255.f * a);
		c.g = (uint8_t)(r * 255.f * a);
		c.b = (uint8_t)(r * 255.f * a);
		return c();
	}
};

namespace dxtk
{
	typedef ColorRGB Color;
}