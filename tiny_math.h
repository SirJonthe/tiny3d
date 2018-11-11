#ifndef TINY_MATH_H
#define TINY_MATH_H

#include <limits>

#include "tiny_system.h"

#define TINY3D_REAL_PRECISION 16

namespace tiny3d
{

template < typename type_t > type_t Min(type_t a, type_t b)           { return a < b ? a : b; }
template < typename type_t > type_t Max(type_t a, type_t b)           { return a > b ? a : b; }
template < typename type_t > type_t Min(type_t a, type_t b, type_t c) { return tiny3d::Min(tiny3d::Min(a, b), c); }
template < typename type_t > type_t Max(type_t a, type_t b, type_t c) { return tiny3d::Max(tiny3d::Max(a, b), c); }

template < typename type_t > type_t Abs(type_t x)                           { return tiny3d::Max(x, -x); }
template < typename type_t > type_t Clamp(type_t min, type_t x, type_t max) { return tiny3d::Min(tiny3d::Max(x, min), max); }
template < typename type_t > type_t Wrap(type_t min, type_t x, type_t max)  { return min + (x + tiny3d::Abs(x) * (max - min) - min) % (max - min); }

UInt  Log(UInt base, UInt x);
UInt  Log10(UInt x);
UInt  Log2(UInt x);
UXInt Exp(UInt x, UInt n);
bool  IsPow2(UInt x);
UInt  Interleave16Bits(UHInt a, UHInt b);
UHInt Interleave8Bits(Byte a, Byte b);

class Random
{
private:
	mutable tiny3d::UXInt m_old_state;
	mutable tiny3d::UXInt m_state;
	tiny3d::UXInt         m_inc;

private:
	UInt Roll( void ) const;

public:
	Random( void );
	explicit Random(tiny3d::UXInt state, tiny3d::UXInt inc = 1);
	explicit operator UInt( void ) const;
};

// if 1 unit = 1 meter, where standard 16 gives us a range of -16 - 16 kms (32 km) with a precision of 1/65 mm
class Real
{
private:
	SInt x;

public:
	static SXInt InfBits( void );
	static SXInt NInfBits( void );

public:
	Real( void );
	Real(const Real &r);
	explicit Real(SInt i);
//	Real(SInt i, UInt f); // Too many issues with this
	explicit Real(float f);

	static Real Inf( void );
	static Real NInf( void );
	static Real NaN( void );

	explicit operator SInt( void ) const;

	bool IsNaN( void )  const;
	bool IsInf( void )  const;
	bool IsNInf( void ) const;

	Real operator-( void ) const;
	Real &operator+=(Real r);
	Real &operator-=(Real r);
	Real &operator*=(Real r);
	Real &operator/=(Real r);
	Real &operator+=(SInt r);
	Real &operator-=(SInt r);
	Real &operator*=(SInt r);
	Real &operator/=(SInt r);

	bool operator ==(Real r) const;
	bool operator !=(Real r) const;
	bool operator  <(Real r) const;
	bool operator  >(Real r) const;
	bool operator <=(Real r) const;
	bool operator >=(Real r) const;

	float Debug_ToFloat( void ) const;
	UInt  Debug_ToBits( void )  const;

	static UInt Precision( void );

	friend Real Frac(Real r);
	friend SInt Trunc(Real r);
	friend SInt Round(Real r);
	friend SInt Ceil(Real r);
	friend SInt Floor(Real r);
};

Real operator+(Real l, Real r);
Real operator-(Real l, Real r);
Real operator*(Real l, Real r);
Real operator/(Real l, Real r);
Real operator+(Real l, SInt r);
Real operator-(Real l, SInt r);
Real operator*(Real l, SInt r);
Real operator/(Real l, SInt r);
Real operator+(SInt l, Real r);
Real operator-(SInt l, Real r);
Real operator*(SInt l, Real r);
Real operator/(SInt l, Real r);
bool operator==(SInt l, Real r);
bool operator==(Real l, SInt r);
bool operator!=(SInt l, Real r);
bool operator!=(Real l, SInt r);
bool operator <(SInt l, Real r);
bool operator <(Real l, SInt r);
bool operator >(SInt l, Real r);
bool operator >(Real l, SInt r);
bool operator<=(SInt l, Real r);
bool operator<=(Real l, SInt r);
bool operator>=(SInt l, Real r);
bool operator>=(Real l, SInt r);

template < typename type_t = Real >
Real Wrap(Real min, Real x, Real max)
{
	SXInt smin    = SXInt(*reinterpret_cast<SInt*>(&min));
	SXInt y       = SXInt(*reinterpret_cast<SInt*>(&x));
	SXInt smax    = SXInt(*reinterpret_cast<SInt*>(&max));
	SXInt span    = smax - smin;
	SInt  wrapped = SInt(smin + (y + (tiny3d::Abs(y) * span) - smin) % span);
	return *reinterpret_cast<Real*>(&wrapped);
}

Real  Pi( void );
Real  Tau( void );
Real  Sin(Real rad);
float Debug_Sin(float rad);
Real  Cos(Real rad);
float Debug_Cos(float rad);
Real  Sqrt(Real x);
Real  Frac(Real r);
SInt  Trunc(Real r);
SInt  Round(Real r);
SInt  Ceil(Real r);
SInt  Floor(Real r);

class Vector3
{
public:
	Real x, y, z;

public:
	Vector3( void );
	Vector3(Real _x, Real _y, Real _z);
	Vector3(const Vector3 &r);

	Vector3 &operator+=(const Vector3 &r);
	Vector3 &operator-=(const Vector3 &r);
	Vector3 &operator*=(const Vector3 &r);
	Vector3 &operator/=(const Vector3 &r);
	Vector3 &operator*=(Real r);
	Vector3 &operator/=(Real r);
	Vector3 &operator*=(SInt r);
	Vector3 &operator/=(SInt r);

	bool IsNormal( void ) const;

	Real &operator[](UInt i);
	const Real &operator[](UInt i) const;
};

Vector3 operator+(Vector3 l, const Vector3 &r);
Vector3 operator-(Vector3 l, const Vector3 &r);
Vector3 operator*(Vector3 l, const Vector3 &r);
Vector3 operator/(Vector3 l, const Vector3 &r);
Vector3 operator*(Vector3 l, Real r);
Vector3 operator/(Vector3 l, Real r);
Vector3 operator-(const Vector3 &u);
Vector3 operator*(Vector3 l, SInt r);
Vector3 operator/(Vector3 l, SInt r);

Vector3 Min(const Vector3 &a, const Vector3 &b);
Vector3 Max(const Vector3 &a, const Vector3 &b);
Real    Dot(const Vector3 &l, const Vector3 &r);
Vector3 Cross(const Vector3 &l, const Vector3 &r);
Real    Len(const Vector3 &v);
Vector3 Normalize(const Vector3 &v);

class Vector2
{
public:
	Real x, y;

public:
	Vector2( void );
	Vector2(Real _x, Real _y);
	Vector2(const Vector2 &r);

	Vector2 &operator+=(const Vector2 &r);
	Vector2 &operator-=(const Vector2 &r);
	Vector2 &operator*=(const Vector2 &r);
	Vector2 &operator/=(const Vector2 &r);
	Vector2 &operator*=(Real r);
	Vector2 &operator/=(Real r);
	Vector2 &operator*=(SInt r);
	Vector2 &operator/=(SInt r);

	bool IsNormal( void ) const;

	Real &operator[](UInt i);
	const Real &operator[](UInt i) const;
};

Vector2 operator+(Vector2 l, const Vector2 &r);
Vector2 operator-(Vector2 l, const Vector2 &r);
Vector2 operator*(Vector2 l, const Vector2 &r);
Vector2 operator/(Vector2 l, const Vector2 &r);
Vector2 operator*(Vector2 l, Real r);
Vector2 operator/(Vector2 l, Real r);
Vector2 operator-(const Vector2 &u);
Vector2 operator*(Vector2 l, SInt r);
Vector2 operator/(Vector2 l, SInt r);

Vector2 Min(const Vector2 &a, const Vector2 &b);
Vector2 Max(const Vector2 &a, const Vector2 &b);
Real    Dot(const Vector2 &l, const Vector2 &r);
Real    Len(const Vector2 &v);
Vector2 Normalize(const Vector2 &v);

class Matrix3x3
{
public:
	Vector3 x, y, z;

public:
	Matrix3x3( void );
	Matrix3x3(const Vector3 &_x, const Vector3 &_y, const Vector3 &_z);
	Matrix3x3(Real xx, Real xy, Real xz, Real yx, Real yy, Real yz, Real zx, Real zy, Real zz);
	Matrix3x3(const Matrix3x3 &m);

	Vector3 &operator[](UInt row);
	const Vector3 &operator[](UInt row) const;
};

Matrix3x3 Identity3( void );
Matrix3x3 Transp(const Matrix3x3 &m);
Matrix3x3 Inv(const Matrix3x3 &m);
Matrix3x3 Euler(Real head, Real pitch, Real roll);
Matrix3x3 AxisAngle(Vector3 axis, Real angle);

Matrix3x3 operator*(const Matrix3x3 &l, Matrix3x3 r);
Matrix3x3 operator*(const Matrix3x3 &l, Real r);
Vector3   operator*(const Vector3 &l, const Matrix3x3 &r);

class Matrix2x2
{
public:
	Vector2 x, y;

public:
	Matrix2x2( void );
	Matrix2x2(const Vector2 &_x, const Vector2 &_y);
	Matrix2x2(Real xx, Real xy, Real yx, Real yy);
	Matrix2x2(const Matrix2x2 &m);

	Vector2 &operator[](UInt row);
	const Vector2 &operator[](UInt row) const;
};

Matrix2x2 Identity2( void );
Matrix2x2 Transp(const Matrix2x2 &m);
Matrix2x2 Inv(const Matrix2x2 &m);
Matrix2x2 Rotate2(Real angle);

Matrix2x2 operator*(const Matrix2x2 &l, Matrix2x2 r);
Matrix2x2 operator*(const Matrix2x2 &l, Real r);
Vector2   operator*(const Vector2 &l, const Matrix2x2 &r);

}

#endif // TINY_MATH_H
