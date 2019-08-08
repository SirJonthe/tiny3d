#ifndef TINY_MATH_H
#define TINY_MATH_H

#include <limits>
#include "tiny_system.h"

namespace tiny3d
{

template < typename num_t >  num_t  Min(num_t a, num_t b)          { return a < b ? a : b; }
template < typename num_t >  num_t  Max(num_t a, num_t b)          { return a > b ? a : b; }
template < typename num_t >  num_t  Min(num_t a, num_t b, num_t c) { return tiny3d::Min(tiny3d::Min(a, b), c); }
template < typename num_t >  num_t  Max(num_t a, num_t b, num_t c) { return tiny3d::Max(tiny3d::Max(a, b), c); }
template < typename type_t > void   Swap(type_t &a, type_t &b)     { type_t c = a; a = b; b = c; }
template < typename sint_t > sint_t Sign2(sint_t x)                { return x < 0 ? -1 : 1; }
template < typename sint_t > sint_t Sign3(sint_t x)                { return x < 0 ? -1 : (x > 0 ? 1 : 0); }

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

template < typename int_t >
void SetBit(int_t &bits, tiny3d::UInt i) { bits |= (1 << i); }
template < typename int_t >
void ClearBit(int_t &bits, tiny3d::UInt i) { bits &= (~(1 << i)); }
template < typename int_t >
void ToggleBit(int_t &bits, tiny3d::UInt i) { bits ^= (1 << i); }
template < typename int_t >
bool TestBit(int_t bits, tiny3d::UInt i) { return (bits & (1 << i)) != 0; }
template < typename int_t >
int_t ReadBit(int_t bits, tiny3d::UInt i) { return (bits >> i) & 1; }

/*class XReal;

// if 1 unit = 1 meter, where standard 16 gives us a range of -16 - 16 kms (32 km) with a precision of 1/65 mm
class Real
{
private:
	friend class XReal;
	typedef SInt real_t;
	static constexpr real_t S_INF_BIT  = std::numeric_limits<real_t>::max();
	static constexpr real_t S_NINF_BIT = std::numeric_limits<real_t>::min() + 1;
	static constexpr real_t S_NAN_BIT  = std::numeric_limits<real_t>::min();
	static constexpr real_t S_PREC     = (CHAR_BIT * sizeof(real_t)) / 2;
	static constexpr real_t S_DEC_STEP = 1;
	static constexpr real_t S_INT_STEP = 1 << S_PREC;

private:
	real_t x;

public:
	Real( void );
	Real(const Real &r);
	explicit Real(SInt i);
	explicit Real(float f);
	explicit operator SInt( void ) const;

	static Real Inf( void );
	static Real NInf( void );
	static Real NaN( void );
	static Real SmallestInt( void );
	static Real SmallestFrac( void );
	static Real Max( void );
	static Real Min( void );
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

	float ToFloat( void ) const;
	UInt  ToBits( void )  const;

	static constexpr UInt Precision( void ) { return UInt(S_PREC); }

	friend Real Frac(Real r);
	friend SInt Trunc(Real r);
	friend SInt Round(Real r);
	friend SInt Ceil(Real r);
	friend SInt Floor(Real r);
};

Real operator +(Real l, Real r);
Real operator -(Real l, Real r);
Real operator *(Real l, Real r);
Real operator /(Real l, Real r);
Real operator +(Real l, SInt r);
Real operator -(Real l, SInt r);
Real operator *(Real l, SInt r);
Real operator /(Real l, SInt r);
Real operator +(SInt l, Real r);
Real operator -(SInt l, Real r);
Real operator *(SInt l, Real r);
Real operator /(SInt l, Real r);
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
bool operator>=(Real l, SInt r);*/

/*template < typename type_t = Real >
Real Wrap(Real min, Real x, Real max)
{
	SXInt smin    = SXInt(*reinterpret_cast<SInt*>(&min));
	SXInt y       = SXInt(*reinterpret_cast<SInt*>(&x));
	SXInt smax    = SXInt(*reinterpret_cast<SInt*>(&max));
	SXInt span    = smax - smin;
	SInt  wrapped = SInt(smin + (y + (tiny3d::Abs(y) * span) - smin) % span);
	return *reinterpret_cast<Real*>(&wrapped);
}*/

Real  Pi( void );
Real  Tau( void );
Real  Sin(Real rad);
Real  Cos(Real rad);
Real  Sqrt(Real x);
Real  Frac(Real r);
SInt  Trunc(Real r);
SInt  Round(Real r);
SInt  Ceil(Real r);
SInt  Floor(Real r);
template < typename type_t > type_t Lerp(const type_t &a, const type_t &b, Real x) { return a + (b - a) * x; }

/*class XReal
{
private:
	typedef SXInt real_t;
	static constexpr real_t S_INF_BIT  = std::numeric_limits<real_t>::max();
	static constexpr real_t S_NINF_BIT = std::numeric_limits<real_t>::min() + 1;
	static constexpr real_t S_NAN_BIT  = std::numeric_limits<real_t>::min();
	static constexpr real_t S_PREC     = (CHAR_BIT * sizeof(real_t)) / 2;
	static constexpr real_t S_DEC_STEP = 1;
	static constexpr real_t S_INT_STEP = real_t(1) << S_PREC;

private:
	real_t x;

public:
	XReal( void );
	XReal(const XReal &r);
	explicit XReal(SInt i);
	explicit XReal(Real r);
	explicit operator SInt( void ) const;
	explicit operator Real( void ) const;

	static XReal Inf( void );
	static XReal NInf( void );
	static XReal NaN( void );
	static XReal SmallestInt( void );
	static XReal SmallestFrac( void );
	static XReal Max( void );
	static XReal Min( void );
	bool IsNaN( void )  const;
	bool IsInf( void )  const;
	bool IsNInf( void ) const;

	XReal operator-( void ) const;
	XReal &operator+=(XReal r);
	XReal &operator-=(XReal r);
	XReal &operator*=(XReal r);
	XReal &operator+=(SInt r);
	XReal &operator-=(SInt r);
	XReal &operator*=(SInt r);
	XReal &operator/=(SInt r);

	bool operator ==(XReal r) const;
	bool operator !=(XReal r) const;
	bool operator  <(XReal r) const;
	bool operator  >(XReal r) const;
	bool operator <=(XReal r) const;
	bool operator >=(XReal r) const;

	UXInt ToBits( void )  const;

	static constexpr UInt Precision( void ) { return UInt(S_PREC); }
};

XReal operator +(XReal l, XReal r);
XReal operator -(XReal l, XReal r);
XReal operator *(XReal l, XReal r);
XReal operator +(XReal l, SInt  r);
XReal operator -(XReal l, SInt  r);
XReal operator *(XReal l, SInt  r);
XReal operator /(XReal l, SInt  r);
XReal operator +(SInt  l, XReal r);
XReal operator -(SInt  l, XReal r);
XReal operator *(SInt  l, XReal r);
bool  operator==(SInt  l, XReal r);
bool  operator==(XReal l, SInt  r);
bool  operator!=(SInt  l, XReal r);
bool  operator!=(XReal l, SInt  r);
bool  operator <(SInt  l, XReal r);
bool  operator <(XReal l, SInt  r);
bool  operator >(SInt  l, XReal r);
bool  operator >(XReal l, SInt  r);
bool  operator<=(SInt  l, XReal r);
bool  operator<=(XReal l, SInt  r);
bool  operator>=(SInt  l, XReal r);
bool  operator>=(XReal l, SInt  r);*/

class Vector3
{
public:
	Real x, y, z;

public:
	Vector3( void );
	Vector3(Real _x, Real _y, Real _z);
	Vector3(const Vector3 &r) = default;
	Vector3 &operator=(const Vector3 &r) = default;

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
Real    Area(const Vector3 &a, const Vector3 &b, const Vector3 &c);

class Vector2
{
public:
	Real x, y;

public:
	Vector2( void );
	Vector2(Real _x, Real _y);
	Vector2(const Vector2 &r) = default;
	Vector2 &operator=(const Vector2 &r) = default;

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
Real    Area(const Vector2 &a, const Vector2 &b, const Vector2 &c);

class Matrix3x3
{
public:
	Vector3 x, y, z;

public:
	Matrix3x3( void );
	Matrix3x3(const Vector3 &_x, const Vector3 &_y, const Vector3 &_z);
	Matrix3x3(Real xx, Real xy, Real xz, Real yx, Real yy, Real yz, Real zx, Real zy, Real zz);
	Matrix3x3(const Matrix3x3 &m) = default;
	Matrix3x3 &operator=(const Matrix3x3 &m) = default;

	Vector3 &operator[](UInt row);
	const Vector3 &operator[](UInt row) const;
};

Matrix3x3 Identity3( void );
Matrix3x3 Transp(const Matrix3x3 &m);
Matrix3x3 Inv(const Matrix3x3 &m);
Matrix3x3 Euler(Real head, Real pitch, Real roll);
Matrix3x3 AxisAngle(Vector3 axis, Real angle);
Matrix3x3 RotateX(Real angle);
Matrix3x3 RotateY(Real angle);
Matrix3x3 RotateZ(Real angle);

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
	Matrix2x2(const Matrix2x2 &m) = default;
	Matrix2x2 &operator=(const Matrix2x2 &m) = default;

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

class Matrix4x4
{
private:
	Real m[4][4];

private:
	static Real Dot(const Real *l, const Real *r);

public:
	Matrix4x4( void );
	Matrix4x4(Real xx, Real xy, Real xz, Real xw, Real yx, Real yy, Real yz, Real yw, Real zx, Real zy, Real zz, Real zw, Real wx, Real wy, Real wz, Real ww);
	Matrix4x4(const Matrix4x4 &m) = default;
	Matrix4x4 &operator=(const Matrix4x4 &m) = default;

	Real *operator[](UInt row);
	const Real *operator[](UInt row) const;

	Matrix4x4 operator*(Matrix4x4 r) const;
	Matrix4x4 operator*(Real r) const;
};

Matrix4x4 Identity4( void );
Matrix4x4 Transp(const Matrix4x4 &m);
Matrix4x4 Transform(const Matrix3x3 &rot, const Vector3 &pos);
Matrix3x3 Rotation(const Matrix4x4 &transform);
Vector3   Translation(const Matrix4x4 &transform);
Matrix4x4 Inv(const Matrix4x4 &m);

Vector3 operator*(const Vector3 &l, const Matrix4x4 &r);

}

#endif // TINY_MATH_H
