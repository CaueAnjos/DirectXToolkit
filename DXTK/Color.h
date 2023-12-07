#pragma once
#include <DirectXColors.h>
#include <DirectXMath.h>
#include "Types.h"

namespace dxtk
{
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

	typedef ColorRGB Color;

	class ColorEx
	{
	public:
		ColorEx() = default;
		ColorEx(const DirectX::XMFLOAT4& color)
			: vec4(color) {}

		ColorEx(const ColorRGB& color)
		{
			*this = color;
		}

		ColorEx(const ColorRGBA& color)
		{
			*this = color;
		}

		ColorEx operator=(const ColorRGB& color)
		{
			vec4.x = color.r / 255.f;
			vec4.y = color.g / 255.f;
			vec4.z = color.b / 255.f;
			vec4.w = 1.f;
			return *this;
		}

		ColorEx operator=(const ColorRGBA& color)
		{
			vec4.x = color.r;
			vec4.y = color.g;
			vec4.z = color.b;
			vec4.w = color.a;
			return *this;
		}

		ColorRGB getRGB() const
		{
			ColorRGB c;
			c.r = (uint8_t)(vec4.x * 255.f * vec4.w);
			c.g = (uint8_t)(vec4.y * 255.f * vec4.w);
			c.b = (uint8_t)(vec4.z * 255.f * vec4.w);
			return c;
		}

		ColorRGBA getRGBA() const
		{
			ColorRGBA c;
			c.r = vec4.x;
			c.g = vec4.y;
			c.b = vec4.z;
			c.a = vec4.w;
			return c;
		}

		DirectX::XMFLOAT4 vec4;
	};
}
