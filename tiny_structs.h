#ifndef TINY_STRUCTS_H
#define TINY_STRUCTS_H

#include "tiny_system.h"
#include "tiny_math.h"

namespace tiny3d
{

struct Point
{
	SInt x, y;
};

struct UPoint
{
	UInt x, y;
};

struct Rect
{
	Point a, b;
};

Rect Clip(Rect a, Rect b);

struct URect
{
	UPoint a, b;
};

URect Clip(URect a, URect b);

struct Color
{
	enum BlendMode
	{
		Transparent,     // don't render the pixel
		Emissive,        // render the pixel solid with lighting
		AddAlpha,        // additive alpha
		Solid,           // don't add lighting to the pixel
		EmissiveAddAlpha // emissive additive alpha
	};

	Byte r, g, b, blend;
};

Color operator+(Color l, Color r);
Color operator-(Color l, Color r);
Color operator*(Color l, Color r);
Color operator*(Color l, Real r);
Color operator/(Color l, Real r);

Color Dither2x2(Color c, UPoint p);
Color Dither3x3(Color c, UPoint p);
Color Dither4x4(Color c, UPoint p);
Color Dither8x8(Color c, UPoint p);

Byte Illum(Color c);

struct Vertex
{
	Vector3 v;
	Vector2 t;
	Color   c;
};

template < typename type_t >
class Array
{
private:
	type_t *m_arr;
	UInt    m_size;

public:
	 Array( void )                 : m_arr(nullptr), m_size(0) {}
	 Array(const Array<type_t> &a) : Array()                   { Copy(a); }
	 explicit Array(UInt num)      : Array()                   { Create(num); }
	~Array( void )                                             { delete [] m_arr; }
	void Create(UInt num)
	{
		if (num == m_size) { return; }
		delete [] m_arr;
		m_arr = new type_t[num];
		m_size = num;
	}
	void Destroy( void )
	{
		delete [] m_arr;
		m_arr = nullptr;
		m_size = 0;
	}
	void Copy(const Array<type_t> &a)
	{
		if (this == &a) { return; }
		Create(a.m_size);
		for (UInt i = 0; i < m_size; ++i) {
			m_arr[i] = a.m_arr[i];
		}
	}
	Array<type_t> &operator=(const Array<type_t> &a)
	{
		Copy(a);
		return *this;
	}
	type_t &operator[](UInt i)
	{
		TINY3D_ASSERT(i < m_size);
		return m_arr[i];
	}
	const type_t &operator[](UInt i) const
	{
		TINY3D_ASSERT(i < m_size);
		return m_arr[i];
	}
	operator type_t*( void )             { return m_arr; }
	operator const type_t*( void ) const { return m_arr; }
	UInt GetSize( void ) const { return m_size; }
};

}

#endif // TINY_STRUCTS_H
