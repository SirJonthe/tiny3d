#ifndef TINY_MATH_H
#define TINY_MATH_H

#include <limits>
#include "tiny_system.h"

namespace tiny3d
{

/// @brief Returns the minimum number given two input values.
/// @param a, b -> Test values.
/// @return The minimum value.
template < typename num_t >  num_t  Min(num_t a, num_t b)          { return a < b ? a : b; }

/// @brief Returns the maximum number given two input values.
/// @param a, b -> Test values.
/// @return The maximum value.
template < typename num_t >  num_t  Max(num_t a, num_t b)          { return a > b ? a : b; }

/// @brief Returns the minimum number given three input values.
/// @param a, b, c -> Test values.
/// @return The minimum value.
template < typename num_t >  num_t  Min(num_t a, num_t b, num_t c) { return tiny3d::Min(tiny3d::Min(a, b), c); }

/// @brief Returns the maximum number given three input values.
/// @param a, b -> Test values.
/// @return The maximum value.
template < typename num_t >  num_t  Max(num_t a, num_t b, num_t c) { return tiny3d::Max(tiny3d::Max(a, b), c); }

/// @brief Swaps two elements.
/// @paramout a, b -> The elements to be swapped.
template < typename type_t > void   Swap(type_t &a, type_t &b)     { type_t c = a; a = b; b = c; }

/// @brief Returns the sign of the input signed number.
/// @param x -> Signed number.
/// @return The sign (-1, 1).
template < typename sint_t > sint_t Sign2(sint_t x)                { return x < 0 ? -1 : 1; }

/// @brief Returns the sign of the input signed number.
/// @param x -> Signed number
/// @return The sign (-1, 0, 1).
template < typename sint_t > sint_t Sign3(sint_t x)                { return x < 0 ? -1 : (x > 0 ? 1 : 0); }

/// @brief Returns the absolute value of the input.
/// @param x -> A value.
/// @return The absolute value.
template < typename type_t > type_t Abs(type_t x)                           { return tiny3d::Max(x, -x); }

/// @brief Contains a given input to a specified interval by saturating value at end points on over-/underflow.
/// @param
//   min, max -> The interval.
//   x -> The value.
/// @return The clamped value.
template < typename type_t > type_t Clamp(type_t min, type_t x, type_t max) { return tiny3d::Min(tiny3d::Max(x, min), max); }

/// @brief Constains a given input to a specified interval by wrapping around value at end points on over-/underflow.
/// @param
//   min, max -> The interval.
//   x -> The value.
/// @return The wrapped value.
template < typename type_t > type_t Wrap(type_t min, type_t x, type_t max)  { return min + (x + tiny3d::Abs(x) * (max - min) - min) % (max - min); }

/// @brief Returns integer log using specified base.
/// @param
//   base -> The base used to get the log.
//   x -> The input to check the log.
/// @return The log.
UInt  Log(UInt base, UInt x);

	/// @brief Returns integer log using 10 as base.
	/// @param
	//   x -> The input to check the log.
	/// @return The log.
UInt  Log10(UInt x);

/// @brief Returns integer log using 2 as base.
/// @param
//   x -> The input to check the log.
/// @return The log.
UInt  Log2(UInt x);

/// @brief Returns integer exponent computation.
/// @param
//   x -> The base.
//   n -> The exponent.
/// @return x^n.
UXInt Exp(UInt x, UInt n);

/// @brief Checks if a number is a power of 2.
/// @param
//   x -> The input to check.
/// @return TRUE if number is a power of 2.
bool  IsPow2(UInt x);

/// @brief Interleaves two 16-bit bit patterns into one 32-bit bit pattern.
/// @param
//   base -> The base used to get the log.
//   a, b -> The bit patterns to interleave.
/// @return 0bABABABABABABABABABABABABABABABAB.
UInt  Interleave16Bits(UHInt a, UHInt b);

/// @brief Interleaves two 8-bit bit patterns into one 16-bit bit pattern.
/// @param
//   base -> The base used to get the log.
//   a, b -> The bit patterns to interleave.
/// @return 0bABABABABABABABAB.
UHInt Interleave8Bits(Byte a, Byte b);

/// @brief Sets the specified bit to 1.
/// @param
//   i -> The bit index to modify.
/// @paramout
//   bits -> The bit pattern to modify.
template < typename int_t >
void SetBit(int_t &bits, tiny3d::UInt i) { bits |= (1 << i); }

/// @brief Sets the specified bit to 0.
/// @param
//   i -> The bit index to modify.
/// @paramout
//   bits -> The bit pattern to modify.
template < typename int_t >
void ClearBit(int_t &bits, tiny3d::UInt i) { bits &= (~(1 << i)); }

/// @brief Sets the specified bit to 1 if 0, or 0 if 1.
/// @param
//   i -> The bit index to modify.
/// @paramout
//   bits -> The bit pattern to modify.
template < typename int_t >
void ToggleBit(int_t &bits, tiny3d::UInt i) { bits ^= (1 << i); }

/// @brief Sets the specified bit to 1.
/// @param
//   bits -> The bit pattern to check.
//   i -> The bit index to check.
/// @return The bit state (TRUE if 1, FALSE if 0).
template < typename int_t >
bool TestBit(int_t bits, tiny3d::UInt i) { return (bits & (1 << i)) != 0; }

/// @brief Returns the bit pattern with only the state of one specified bit.
/// @param
//   bits -> The bit pattern to check.
//   i -> The bit index to check.
/// @return The bit state.
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

/// @return Pi
Real  Pi( void );

/// @return 2*pi
Real  Tau( void );

/// @param rad -> Radians.
/// @return The sine.
Real  Sin(Real rad);

/// @param rad -> Radians.
/// @return The cosine.
Real  Cos(Real rad);

/// @param x -> A value.
/// @return The square root of x.
Real  Sqrt(Real x);

/// @param r -> A value.
/// @return The fractional part of r.
Real  Frac(Real r);

/// @brief Rounds a value by truncating it.
/// @param r -> A value.
/// @return The trancated value.
SInt  Trunc(Real r);

/// @brief Rounds a value.
/// @param r -> A value.
/// @return The rounded value.
SInt  Round(Real r);

/// @paramput Rounds a value to the next nearest whole integer.
/// @param r -> A value.
/// @return The rounded value.
SInt  Ceil(Real r);

/// @brief Rounds a value to the previous nearest while integer.
/// @param r -> A value.
/// @return The rounded value.
SInt  Floor(Real r);

/// @brief Linearly interpolates a value.
/// @param
//   a, b -> The end points.
//   x -> The interpolant.
/// @return The interpolated value.
template < typename type_t, typename real_t > type_t Lerp(const type_t &a, const type_t &b, const real_t &x) { return a + (b - a) * x; }

/// @brief Does bilinear interpolation between four values.
/// @param
//  aa, ab -> Upper-left and upper-right end points.
//  ba, bb -> Lower-left and lower-right end points.
//  x -> Horizontal interpolant.
//  y -> Vertical interpolant.
/// @return The interpolated value.
template < typename type_t, typename real_t > type_t Bilerp(const type_t &aa, const type_t &ab, const type_t &ba, const type_t &bb, const real_t &x, const real_t &y)
{
	return Lerp(
		Lerp(aa, ab, x),
		Lerp(ba, bb, x),
		y
	);
}

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

/// @brief A 3D vector.
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

	/// @return TRUE if vector has unit length.
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

/// @brief Returns the minimum individual values between two input vectors.
/// @param a, b -> Input vectors.
/// @return The minimum values.
Vector3 Min(const Vector3 &a, const Vector3 &b);

/// @brief Returns the maximum individual values between two input vectors.
/// @param a, b -> Input vectors.
/// @return The maximum values.
Vector3 Max(const Vector3 &a, const Vector3 &b);

/// @brief The dot product between two vectors.
/// @param l, r -> Left and right hand side vectors.
/// @return The dot product.
Real    Dot(const Vector3 &l, const Vector3 &r);

/// @brief The cross product between two vectors.
/// @param l, r -> Left and right hand side vectors.
/// @return The cross product.
Vector3 Cross(const Vector3 &l, const Vector3 &r);

/// @param The vector to retrieve the length from.
/// @return The length of the vector.
Real    Len(const Vector3 &v);

/// @param The vector to normalize.
/// @return The normalized vector.
Vector3 Normalize(const Vector3 &v);

/// @brief The area defined by a triangle.
/// @param a, b, c -> The triangle vertices.
/// @return The area.
Real    Area(const Vector3 &a, const Vector3 &b, const Vector3 &c);

/// @brief A 2D vector.
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

	/// @return TRUE if vector has unit length.
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

/// @brief Returns the minimum individual values between two input vectors.
/// @param a, b -> Input vectors.
/// @return The minimum values.
Vector2 Min(const Vector2 &a, const Vector2 &b);

/// @brief Returns the maximum individual values between two input vectors.
/// @param a, b -> Input vectors.
/// @return The maximum values.
Vector2 Max(const Vector2 &a, const Vector2 &b);

/// @brief The dot product between two vectors.
/// @param l, r -> Left and right hand side vectors.
/// @return The dot product.
Real    Dot(const Vector2 &l, const Vector2 &r);

/// @param The vector to retrieve the length from.
/// @return The length of the vector.
Real    Len(const Vector2 &v);

/// @param The vector to normalize.
/// @return The normalized vector.
Vector2 Normalize(const Vector2 &v);

/// @brief The area defined by a triangle.
/// @param a, b, c -> The triangle vertices.
/// @return The area.
Real    Area(const Vector2 &a, const Vector2 &b, const Vector2 &c);

/// @brief A 3D matrix.
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

/// @return The identity matrix.
Matrix3x3 Identity3( void );

/// @param m -> A matrix.
/// @return The transposed matrix.
Matrix3x3 Transp(const Matrix3x3 &m);

/// @brief Returns inverse matrix. Not all matrices are invertible.
/// @param m -> A matrix.
/// @return The invertex matrix.
Matrix3x3 Inv(const Matrix3x3 &m);

/// @brief Returns an Euler rotation matrix.
/// @param head, pitch, roll -> Angles in radians.
/// @return The rotation matrix.
Matrix3x3 Euler(Real head, Real pitch, Real roll);

/// @brief Returns a rotation matrix based on an axis and an angle.
/// @param
//   axis -> The axis to rotate about.
//   angle -> The angle to rotate in radians.
/// @return The rotation matrix.
Matrix3x3 AxisAngle(Vector3 axis, Real angle);

/// @param angle -> The angle in radians.
/// @return The rotation matrix.
Matrix3x3 RotateX(Real angle);

/// @param angle -> The angle in radians.
/// @return The rotation matrix.
Matrix3x3 RotateY(Real angle);

/// @param angle -> The angle in radians.
/// @return The rotation matrix.
Matrix3x3 RotateZ(Real angle);

Matrix3x3 operator*(const Matrix3x3 &l, Matrix3x3 r);
Matrix3x3 operator*(const Matrix3x3 &l, Real r);
Vector3   operator*(const Vector3 &l, const Matrix3x3 &r);

/// @brief A 2D matrix.
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

/// @return The identity matrix.
Matrix2x2 Identity2( void );

/// @param m -> A matrix.
/// @return The transposed matrix.
Matrix2x2 Transp(const Matrix2x2 &m);

/// @brief Returns inverse matrix. Not all matrices are invertible.
/// @param m -> A matrix.
/// @return The invertex matrix.
Matrix2x2 Inv(const Matrix2x2 &m);

/// @param angle -> The angle in radians.
/// @return The rotation matrix.
Matrix2x2 Rotate2(Real angle);

Matrix2x2 operator*(const Matrix2x2 &l, Matrix2x2 r);
Matrix2x2 operator*(const Matrix2x2 &l, Real r);
Vector2   operator*(const Vector2 &l, const Matrix2x2 &r);

/// @brief A 4D matrix.
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

/// @return The identity matrix.
Matrix4x4 Identity4( void );

/// @param m -> A matrix.
/// @return The transposed matrix.
Matrix4x4 Transp(const Matrix4x4 &m);

/// @brief Constructs a transform given a basis and a position.
/// @param
//   rot -> The basis of the transform.
//   pos -> The translation of the transform.
/// @return The transform matrix.
Matrix4x4 Transform(const Matrix3x3 &rot, const Vector3 &pos);

/// @brief Returns the basis of the transform.
/// @param transform -> The transform.
/// @return The basis.
Matrix3x3 Rotation(const Matrix4x4 &transform);

/// @brief Returns the translation (position) of the transform.
/// @param transform -> The transform.
/// @return The translation.
Vector3   Translation(const Matrix4x4 &transform);

/// @brief Returns inverse matrix. Not all matrices are invertible.
/// @param m -> A matrix.
/// @return The invertex matrix.
Matrix4x4 Inv(const Matrix4x4 &m);

Vector3 operator*(const Vector3 &l, const Matrix4x4 &r);

}

#endif // TINY_MATH_H
