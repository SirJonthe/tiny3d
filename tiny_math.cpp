#include <iostream>
#include <math.h>

#include "tiny_math.h"

// all below have 16 bits of precision
#define TINY3D_FIXED_PI     205887L
#define TINY3D_FIXED_2PI    411775L
#define TINY3D_FIXED_E      178144L
#define TINY3D_FIXED_ROOT2   74804L
#define TINY3D_FIXED_ROOT3  113512L
#define TINY3D_FIXED_GOLDEN 106039L

#if (-1L << 1 == -2L)
	#define TINY3D_LSHIFT(x, y) ((x) << (y))
#elif
	#define TINY3D_LSHIFT(x, y) (((x) < 0) ? -(-(x) << (y)) : ((x) << (y)))
#endif

#if (-2L >> 1 == -1L)
	#define TINY3D_RSHIFT(x, y) ((x) >> (y))
#elif
	#define TINY3D_RSHIFT(x, y) (((x) < 0) ? -(-(x) >> (y)) : ((x) >> (y)))
#endif

#define TINY3D_REAL_UPSCALE(x) (TINY3D_LSHIFT((x), TINY3D_REAL_PRECISION))
#define TINY3D_REAL_DOWNSCALE(x) (TINY3D_RSHIFT((x), TINY3D_REAL_PRECISION))

tiny3d::UInt tiny3d::Log(tiny3d::UInt base, tiny3d::UInt x)
{
	tiny3d::UInt log = 0;
	while (x >= base) {
		x /= base;
		++log;
	}
	return log;
}

tiny3d::UInt tiny3d::Log10(tiny3d::UInt x)
{
	return tiny3d::Log(10, x);
}

tiny3d::UInt tiny3d::Log2(tiny3d::UInt x)
{
	tiny3d::UInt log = 0;
	while (x >= 2) {
		x >>= 1;
		++log;
	}
	return log;
}

tiny3d::UXInt tiny3d::Exp(tiny3d::UInt x, tiny3d::UInt n)
{
	UXInt out = 1;
	UInt y = x;
	for (UInt i = 0; i < n; ++i) {
		out = out * y;
	}
	return out;
}

bool tiny3d::IsPow2(tiny3d::UInt x)
{
	return x && !(x & (x - 1));
}

tiny3d::UInt tiny3d::Interleave16Bits(tiny3d::UHInt a, tiny3d::UHInt b)
{
	static const UInt B[] = { 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF };
	static const UInt S[] = { 1, 2, 4, 8 };

	UInt x = a;
	UInt y = b;

	x = (x | (x << S[3])) & B[3];
	x = (x | (x << S[2])) & B[2];
	x = (x | (x << S[1])) & B[1];
	x = (x | (x << S[0])) & B[0];

	y = (y | (y << S[3])) & B[3];
	y = (y | (y << S[2])) & B[2];
	y = (y | (y << S[1])) & B[1];
	y = (y | (y << S[0])) & B[0];

	return x | (y << 1);
}

tiny3d::UHInt tiny3d::Interleave8Bits(tiny3d::Byte a, tiny3d::Byte b)
{
	static const UHInt B[] = { 0x5555, 0x3333, 0x0F0F };
	static const UHInt S[] = { 1, 2, 4 };

	UHInt x = a;
	UHInt y = b;

	x = (x | (x << S[2])) & B[2];
	x = (x | (x << S[1])) & B[1];
	x = (x | (x << S[0])) & B[0];

	y = (y | (y << S[2])) & B[2];
	y = (y | (y << S[1])) & B[1];
	y = (y | (y << S[0])) & B[0];

	return UHInt(x | (y << 1));
}

tiny3d::SXInt tiny3d::Real::InfBits( void )
{
	return SXInt(Inf().x);
}

tiny3d::SXInt tiny3d::Real::NInfBits( void )
{
	return SXInt(NInf().x);
}

tiny3d::Real::Real( void ) : x(0)
{}

tiny3d::Real::Real(const tiny3d::Real &r) : x(r.x)
{}

tiny3d::Real::Real(tiny3d::SInt i) : x(SInt(TINY3D_REAL_UPSCALE(i))) {}

// ERROR: Real(-0,255) -> there is no such thing as -0
// ERROR: Real(0,0001) -> 0001 translates to 1
/*tiny3d::Real::Real(tiny3d::SInt i, tiny3d::UInt f)
{
	UXInt frac = (TINY3D_REAL_UPSCALE(f)) / Exp(10, Log10(f) + 1);
	x = SInt(TINY3D_REAL_UPSCALE(i));
	x = x >= 0 ? x + SInt(frac) : x - SInt(frac);
}*/
tiny3d::Real::Real(float f) : x(SInt(f * (1<<TINY3D_REAL_PRECISION)))
{}

tiny3d::Real tiny3d::Real::Inf( void )
{
	constexpr SInt inf = std::numeric_limits<SInt>::max();
	return *reinterpret_cast< const Real* >(&inf);
}

tiny3d::Real tiny3d::Real::NInf( void )
{
	constexpr SInt ninf = std::numeric_limits<SInt>::min() + 1;
	return *reinterpret_cast< const Real* >(&ninf);
}

tiny3d::Real tiny3d::Real::NaN( void )
{
	constexpr SInt nan = std::numeric_limits<SInt>::min();
	return *reinterpret_cast< const Real* >(&nan);
}

tiny3d::Real::operator tiny3d::SInt( void ) const
{
	return !IsNaN() ? (x >= 0 ? TINY3D_REAL_DOWNSCALE(x) : -TINY3D_REAL_DOWNSCALE(-x)) : 0;
}

bool tiny3d::Real::IsNaN( void ) const
{
	return x == NaN().x;
}

bool tiny3d::Real::IsInf( void ) const
{
	return x == Inf().x;
}

bool tiny3d::Real::IsNInf( void ) const
{
	return x == NInf().x;
}

tiny3d::Real tiny3d::Real::operator-( void ) const
{
	Real o;
	o.x = !IsNaN() ? -x : NaN().x;
	return o;
}

tiny3d::Real &tiny3d::Real::operator+=(tiny3d::Real r)
{
	if (!IsNaN() && !r.IsNaN()) {
		SXInt t = SXInt(x) + r.x;
		x = SInt(Clamp(SXInt(NInfBits()), t, SXInt(InfBits())));
	} else {
		x = NaN().x;
	}
	return *this;
}

tiny3d::Real &tiny3d::Real::operator-=(tiny3d::Real r)
{
	if (!IsNaN() && !r.IsNaN()) {
		SXInt t = SXInt(x) - r.x;
		x = SInt(Clamp(SXInt(NInfBits()), t, SXInt(InfBits())));
	} else {
		x = NaN().x;
	}
	return *this;
}

tiny3d::Real &tiny3d::Real::operator*=(tiny3d::Real r)
{
	if (!IsNaN() && !r.IsNaN()) {
		if (r.x != Inf().x && r.x != NInf().x) {
			SXInt t = SXInt(x) * r.x;
			t = TINY3D_REAL_DOWNSCALE(t);
			x = SInt(Clamp(SXInt(NInfBits()), t, SXInt(InfBits())));
		} else {
			x = x != 0 ? r.x : x;
		}
	} else {
		x = NaN().x;
	}
	return *this;
}

tiny3d::Real &tiny3d::Real::operator/=(tiny3d::Real r)
{
	if (!IsNaN() && !r.IsNaN()) {
		if (r.x != 0) {
			SXInt t = TINY3D_REAL_UPSCALE(SXInt(x)) / r.x;
			x = SInt(Clamp(SXInt(NInfBits()), t, SXInt(InfBits())));
		} else {
			x = x != 0 ? Inf().x : 0;
		}
	} else {
		x = NaN().x;
	}
	return *this;
}

tiny3d::Real &tiny3d::Real::operator+=(tiny3d::SInt r)
{
	if (!IsNaN() && !IsInf() && !IsNInf()) {
		SXInt t = SXInt(x) + TINY3D_REAL_UPSCALE(SXInt(r));
		x = SInt(Clamp(SXInt(NInfBits()), t, SXInt(InfBits())));
	}
	return *this;
}

tiny3d::Real &tiny3d::Real::operator-=(tiny3d::SInt r)
{
	if (!IsNaN() && !IsInf() && !IsNInf()) {
		SXInt t = SXInt(x) - TINY3D_REAL_UPSCALE(SXInt(r));
		x = SInt(Clamp(SXInt(NInfBits()), t, SXInt(InfBits())));
	}
	return *this;
}

tiny3d::Real &tiny3d::Real::operator*=(tiny3d::SInt r)
{
	if (!IsNaN() && !IsInf() && !IsNInf()) {
		SXInt t = SXInt(x) * r;
		x = SInt(Clamp(SXInt(NInfBits()), t, SXInt(InfBits())));
	}
	return *this;
}

tiny3d::Real &tiny3d::Real::operator/=(tiny3d::SInt r)
{
	if (!IsNaN() && !IsInf() && !IsNInf()) {
		if (r != 0) {
			SXInt t = SXInt(x) / r;
			x = SInt(Clamp(SXInt(NInfBits()), t, SXInt(InfBits())));
		} else {
			x = x != 0 ? Inf().x : 0;
		}
	}
	return *this;
}

bool tiny3d::Real::operator==(tiny3d::Real r) const
{
	return IsNaN() ? false : x == r.x;
}

bool tiny3d::Real::operator!=(tiny3d::Real r) const
{
	return IsNaN() ? false : x != r.x;
}

bool tiny3d::Real::operator<(tiny3d::Real r) const
{
	return IsNaN() ? false : x <  r.x;
}

bool tiny3d::Real::operator>(tiny3d::Real r) const
{
	return IsNaN() ? false : x >  r.x;
}

bool tiny3d::Real::operator<=(tiny3d::Real r) const
{
	return IsNaN() ? false : x <= r.x;
}

bool tiny3d::Real::operator>=(tiny3d::Real r) const
{
	return IsNaN() ? false : x >= r.x;
}

float tiny3d::Real::Debug_ToFloat( void ) const
{
	return float(x) / float(1 << TINY3D_REAL_PRECISION);
}

tiny3d::UInt  tiny3d::Real::Debug_ToBits( void ) const
{
	return UInt(x);
}

tiny3d::UInt tiny3d::Real::Precision( void )
{
	return TINY3D_REAL_PRECISION;
}

tiny3d::Real tiny3d::operator +(tiny3d::Real l, tiny3d::Real r) { return l += r; }
tiny3d::Real tiny3d::operator -(tiny3d::Real l, tiny3d::Real r) { return l -= r; }
tiny3d::Real tiny3d::operator *(tiny3d::Real l, tiny3d::Real r) { return l *= r; }
tiny3d::Real tiny3d::operator /(tiny3d::Real l, tiny3d::Real r) { return l /= r; }
tiny3d::Real tiny3d::operator +(tiny3d::Real l, tiny3d::SInt r) { return l += r; }
tiny3d::Real tiny3d::operator -(tiny3d::Real l, tiny3d::SInt r) { return l -= r; }
tiny3d::Real tiny3d::operator *(tiny3d::Real l, tiny3d::SInt r) { return l *= r; }
tiny3d::Real tiny3d::operator /(tiny3d::Real l, tiny3d::SInt r) { return l /= r; }
tiny3d::Real tiny3d::operator +(tiny3d::SInt l, tiny3d::Real r) { return r += l; }
tiny3d::Real tiny3d::operator -(tiny3d::SInt l, tiny3d::Real r) { return Real(l) -= r; }
tiny3d::Real tiny3d::operator *(tiny3d::SInt l, tiny3d::Real r) { return r *= l; }
tiny3d::Real tiny3d::operator /(tiny3d::SInt l, tiny3d::Real r) { return Real(l) /= r; }
bool         tiny3d::operator==(tiny3d::SInt l, tiny3d::Real r) { return Real(l) == r; }
bool         tiny3d::operator==(tiny3d::Real l, tiny3d::SInt r) { return l == Real(r); }
bool         tiny3d::operator!=(tiny3d::SInt l, tiny3d::Real r) { return Real(l) != r; }
bool         tiny3d::operator!=(tiny3d::Real l, tiny3d::SInt r) { return l != Real(r); }
bool         tiny3d::operator <(tiny3d::SInt l, tiny3d::Real r) { return Real(l) < r; }
bool         tiny3d::operator <(tiny3d::Real l, tiny3d::SInt r) { return l < Real(r); }
bool         tiny3d::operator >(tiny3d::SInt l, tiny3d::Real r) { return Real(l) > r; }
bool         tiny3d::operator >(tiny3d::Real l, tiny3d::SInt r) { return l > Real(r); }
bool         tiny3d::operator<=(tiny3d::SInt l, tiny3d::Real r) { return Real(l) <= r; }
bool         tiny3d::operator<=(tiny3d::Real l, tiny3d::SInt r) { return l <= Real(r); }
bool         tiny3d::operator>=(tiny3d::SInt l, tiny3d::Real r) { return Real(l) >= r; }
bool         tiny3d::operator>=(tiny3d::Real l, tiny3d::SInt r) { return l >= Real(r); }

tiny3d::Real tiny3d::Pi( void )
{
	UInt pi_bits = UInt(TINY3D_FIXED_PI);
	return *reinterpret_cast< Real* >(&pi_bits);
}

tiny3d::Real tiny3d::Tau( void )
{
	UInt pi_bits = UInt(TINY3D_FIXED_2PI);
	return *reinterpret_cast< Real* >(&pi_bits);
}

// http://www.dcs.gla.ac.uk/~jhw/cordic/index.html
tiny3d::Real tiny3d::Sin(tiny3d::Real rad)
{
	// based on: http://lab.polygonal.de/2007/07/18/fast-and-accurate-sinecosine-approximation/
	// based on: http://www.mclimatiano.com/faster-sine-approximation-using-quadratic-curve/
	const Real PI     = tiny3d::Pi();
	const Real MAGIC1 = Real(4) / PI; // 1.27323954 = 4 / PI
	const Real MAGIC2 = MAGIC1 / PI;  // 0.405284735 = 1.27323954 / PI
	const Real MAGIC3 = Real(0.225f);

	// Wrap input to valid range -pi ... pi
	rad = Wrap(-PI, rad, PI);

	// Compute sine
	Real sin1 = rad < Real(0) ?
			MAGIC1 * rad + MAGIC2 * rad * rad :
			MAGIC1 * rad - MAGIC2 * rad * rad;
	Real sin = sin1 < Real(0) ?
		MAGIC3 * (sin1 * -sin1 - sin1) + sin1 :
		MAGIC3 * (sin1 *  sin1 - sin1) + sin1;
	return sin;
}

float tiny3d::Debug_Sin(float rad)
{
	const float PI     = 3.14159265f;
	const float TAU    = 6.28318531f;
	const float MAGIC1 = 4.0f / PI;
	const float MAGIC2 = MAGIC1 / PI;
	const float MAGIC3 = 0.225f;

	while (rad < -PI) { rad += TAU; }
	while (rad >  PI) { rad -= TAU; }

	float sin = 0.0f;
	if (rad < 0) {
		sin = rad * ( MAGIC1 + MAGIC2 * rad );

		if (sin < 0) {
			sin = sin * ( -MAGIC3 * ( sin + 1 ) + 1 );
		} else {
			sin = sin * ( MAGIC3 * ( sin - 1 ) + 1 );
		}
	} else {
		sin = rad * ( MAGIC1 - MAGIC2 * rad );

		if (sin < 0) {
			sin = sin * ( -MAGIC3 * ( sin + 1 ) + 1 );
		} else {
			sin = sin * ( MAGIC3 * ( sin - 1 ) + 1 );
		}
	}

	return sin;
}

tiny3d::Real tiny3d::Cos(tiny3d::Real rad)
{
	return tiny3d::Sin(rad + (Pi() / 2));
}

float tiny3d::Debug_Cos(float rad)
{
	return Debug_Sin(rad + (3.14159265f / 2.0f));
}

// https://github.com/chmike/fpsqrt/blob/master/fpsqrt.c
// Algorithm and code Author: Christophe Meessen 1993. Initially published in: usenet comp.lang.c, Thu, 28 Jan 1993 08:35:23 GMT, Subject: Fixed point sqrt by: Meessen Christophe
tiny3d::Real tiny3d::Sqrt(tiny3d::Real x)
{
	if (x.IsNaN() || x < Real(0) || x.IsInf()) {
		return Real::NaN();
	}

	UInt t = 0;
	UInt r = *reinterpret_cast< UInt* >(&x);;
	UInt b = 0x40000000;
	UInt q = 0;
	while (b > 0x40) {
		t = q + b;
		if (r >= t) {
			r -= t;
			q = t + b; // equivalent to q += 2*b
		}
		r <<= 1;
		b >>= 1;
	}
	q >>= 8;
	return *reinterpret_cast<Real*>(&q);
}

tiny3d::Real tiny3d::Frac(tiny3d::Real r)
{
	return r - SInt(r);
}

tiny3d::SInt tiny3d::Trunc(tiny3d::Real r)
{
	return SInt(r);
}

tiny3d::SInt tiny3d::Round(tiny3d::Real r)
{
	return r > Real(0) ? SInt(r + 1 / Real(2)) : SInt(r - 1 / Real(2));
}

tiny3d::SInt tiny3d::Ceil(tiny3d::Real r)
{
	SInt ri = SInt(r);
	return r != Real(ri) ? (r.x < 0 ? ri : ri + 1) : ri;
}

tiny3d::SInt tiny3d::Floor(tiny3d::Real r)
{
	SInt ri = SInt(r);
	return r != Real(ri) ? (r.x < 0 ? ri - 1 : ri) : ri;
}

tiny3d::Vector3::Vector3( void ) :
	x(), y(), z()
{}

tiny3d::Vector3::Vector3(tiny3d::Real _x, tiny3d::Real _y, tiny3d::Real _z) :
	x(_x), y(_y), z(_z)
{}

tiny3d::Vector3::Vector3(const tiny3d::Vector3 &r) :
	x(r.x), y(r.y), z(r.z)
{}

tiny3d::Vector3 &tiny3d::Vector3::operator+=(const tiny3d::Vector3 &r)
{
	x += r.x;
	y += r.y;
	z += r.z;
	return *this;
}

tiny3d::Vector3 &tiny3d::Vector3::operator-=(const tiny3d::Vector3 &r)
{
	x -= r.x;
	y -= r.y;
	z -= r.z;
	return *this;
}

tiny3d::Vector3 &tiny3d::Vector3::operator*=(const tiny3d::Vector3 &r)
{
	x *= r.x;
	y *= r.y;
	z *= r.z;
	return *this;
}

tiny3d::Vector3 &tiny3d::Vector3::operator/=(const tiny3d::Vector3 &r)
{
	x /= r.x;
	y /= r.y;
	z /= r.z;
	return *this;
}

tiny3d::Vector3 &tiny3d::Vector3::operator*=(tiny3d::Real r)
{
	x *= r;
	y *= r;
	z *= r;
	return *this;
}

tiny3d::Vector3 &tiny3d::Vector3::operator/=(tiny3d::Real r)
{
	x /= r;
	y /= r;
	z /= r;
	return *this;
}

tiny3d::Vector3 &tiny3d::Vector3::operator*=(tiny3d::SInt r)
{
	x *= r;
	y *= r;
	z *= r;
	return *this;
}

tiny3d::Vector3 &tiny3d::Vector3::operator/=(tiny3d::SInt r)
{
	x /= r;
	y /= r;
	z /= r;
	return *this;
}

bool tiny3d::Vector3::IsNormal( void ) const
{
	return x*x + y*y + z*z == Real(1);
}

tiny3d::Real &tiny3d::Vector3::operator[](tiny3d::UInt i)
{
	return reinterpret_cast<Real*>(&x)[i];
}

const tiny3d::Real &tiny3d::Vector3::operator[](tiny3d::UInt i) const
{
	return reinterpret_cast<const Real*>(&x)[i];
}

tiny3d::Vector3 tiny3d::operator+(tiny3d::Vector3 l, const tiny3d::Vector3 &r) { return l+=r; }
tiny3d::Vector3 tiny3d::operator-(tiny3d::Vector3 l, const tiny3d::Vector3 &r) { return l-=r; }
tiny3d::Vector3 tiny3d::operator*(tiny3d::Vector3 l, const tiny3d::Vector3 &r) { return l*=r; }
tiny3d::Vector3 tiny3d::operator/(tiny3d::Vector3 l, const tiny3d::Vector3 &r) { return l/=r; }
tiny3d::Vector3 tiny3d::operator*(tiny3d::Vector3 l, tiny3d::Real r)           { return l*=r; }
tiny3d::Vector3 tiny3d::operator/(tiny3d::Vector3 l, tiny3d::Real r)           { return l/=r; }
tiny3d::Vector3 tiny3d::operator*(tiny3d::Vector3 l, tiny3d::SInt r)           { return l*=r; }
tiny3d::Vector3 tiny3d::operator/(tiny3d::Vector3 l, tiny3d::SInt r)           { return l/=r; }

tiny3d::Vector3 tiny3d::operator-(const tiny3d::Vector3 &u)
{
	return tiny3d::Vector3(
		-u.x,
		-u.y,
		-u.z
	);
}

tiny3d::Vector3 tiny3d::Min(const tiny3d::Vector3 &a, const tiny3d::Vector3 &b)
{
	return tiny3d::Vector3(
		Min(a.x, b.x),
		Min(a.y, b.y),
		Min(a.z, b.z)
	);
}

tiny3d::Vector3 tiny3d::Max(const tiny3d::Vector3 &a, const tiny3d::Vector3 &b)
{
	return tiny3d::Vector3(
		Max(a.x, b.x),
		Max(a.y, b.y),
		Max(a.z, b.z)
	);
}

tiny3d::Real tiny3d::Dot(const tiny3d::Vector3 &l, const tiny3d::Vector3 &r)
{
	return l.x*r.x + l.y*r.y + l.z*r.z;
}

tiny3d::Vector3 tiny3d::Cross(const tiny3d::Vector3 &l, const tiny3d::Vector3 &r)
{
	return tiny3d::Vector3(
		l.y*r.z - l.z*r.y,
		l.z*r.x - l.x*r.z,
		l.x*r.y - l.y*r.x
	);
}

tiny3d::Real tiny3d::Len(const tiny3d::Vector3 &v)
{
	return tiny3d::Sqrt(tiny3d::Dot(v, v));
}

tiny3d::Vector3 tiny3d::Normalize(const tiny3d::Vector3 &v)
{
	Real inv_len = tiny3d::Real(1) / tiny3d::Len(v);
	return tiny3d::Vector3(
		v.x * inv_len,
		v.y * inv_len,
		v.z * inv_len
	);
}

tiny3d::Vector2::Vector2( void ) :
	x(), y()
{}

tiny3d::Vector2::Vector2(tiny3d::Real _x, tiny3d::Real _y) :
	x(_x), y(_y)
{}

tiny3d::Vector2::Vector2(const tiny3d::Vector2 &r) :
	x(r.x), y(r.y)
{}

tiny3d::Vector2 &tiny3d::Vector2::operator+=(const tiny3d::Vector2 &r)
{
	x += r.x;
	y += r.y;
	return *this;
}

tiny3d::Vector2 &tiny3d::Vector2::operator-=(const tiny3d::Vector2 &r)
{
	x -= r.x;
	y -= r.y;
	return *this;
}

tiny3d::Vector2 &tiny3d::Vector2::operator*=(const tiny3d::Vector2 &r)
{
	x *= r.x;
	y *= r.y;
	return *this;
}

tiny3d::Vector2 &tiny3d::Vector2::operator/=(const tiny3d::Vector2 &r)
{
	x /= r.x;
	y /= r.y;
	return *this;
}

tiny3d::Vector2 &tiny3d::Vector2::operator*=(tiny3d::Real r)
{
	x *= r;
	y *= r;
	return *this;
}

tiny3d::Vector2 &tiny3d::Vector2::operator/=(tiny3d::Real r)
{
	x /= r;
	y /= r;
	return *this;
}

tiny3d::Vector2 &tiny3d::Vector2::operator*=(tiny3d::SInt r)
{
	x *= r;
	y *= r;
	return *this;
}

tiny3d::Vector2 &tiny3d::Vector2::operator/=(tiny3d::SInt r)
{
	x /= r;
	y /= r;
	return *this;
}

bool tiny3d::Vector2::IsNormal( void ) const
{
	return x*x + y*y == Real(1);
}

tiny3d::Real &tiny3d::Vector2::operator[](tiny3d::UInt i)
{
	return reinterpret_cast<Real*>(&x)[i];
}

const tiny3d::Real &tiny3d::Vector2::operator[](tiny3d::UInt i) const
{
	return reinterpret_cast<const Real*>(&x)[i];
}

tiny3d::Vector2 tiny3d::operator+(tiny3d::Vector2 l, const tiny3d::Vector2 &r) { return l+=r; }
tiny3d::Vector2 tiny3d::operator-(tiny3d::Vector2 l, const tiny3d::Vector2 &r) { return l-=r; }
tiny3d::Vector2 tiny3d::operator*(tiny3d::Vector2 l, const tiny3d::Vector2 &r) { return l*=r; }
tiny3d::Vector2 tiny3d::operator/(tiny3d::Vector2 l, const tiny3d::Vector2 &r) { return l/=r; }
tiny3d::Vector2 tiny3d::operator*(tiny3d::Vector2 l, tiny3d::Real r)           { return l*=r; }
tiny3d::Vector2 tiny3d::operator/(tiny3d::Vector2 l, tiny3d::Real r)           { return l/=r; }
tiny3d::Vector2 tiny3d::operator*(tiny3d::Vector2 l, tiny3d::SInt r)           { return l*=r; }
tiny3d::Vector2 tiny3d::operator/(tiny3d::Vector2 l, tiny3d::SInt r)           { return l/=r; }

tiny3d::Vector2 tiny3d::operator-(const tiny3d::Vector2 &u)
{
	return tiny3d::Vector2(
		-u.x,
		-u.y
	);
}

tiny3d::Vector2 tiny3d::Min(const tiny3d::Vector2 &a, const tiny3d::Vector2 &b)
{
	return tiny3d::Vector2(
		Min(a.x, b.x),
		Min(a.y, b.y)
	);
}

tiny3d::Vector2 tiny3d::Max(const tiny3d::Vector2 &a, const tiny3d::Vector2 &b)
{
	return tiny3d::Vector2(
		Max(a.x, b.x),
		Max(a.y, b.y)
	);
}

tiny3d::Real tiny3d::Dot(const tiny3d::Vector2 &l, const tiny3d::Vector2 &r)
{
	return l.x*r.x + l.y*r.y;
}

tiny3d::Real tiny3d::Len(const tiny3d::Vector2 &v)
{
	return tiny3d::Sqrt(tiny3d::Dot(v, v));
}

tiny3d::Vector2 tiny3d::Normalize(const tiny3d::Vector2 &v)
{
	Real inv_len = tiny3d::Real(1) / tiny3d::Len(v);
	return tiny3d::Vector2(
		v.x * inv_len,
		v.y * inv_len
	);
}

tiny3d::Matrix3x3::Matrix3x3( void ) :
	x(), y(), z()
{}

tiny3d::Matrix3x3::Matrix3x3(const tiny3d::Vector3 &_x, const tiny3d::Vector3 &_y, const tiny3d::Vector3 &_z) :
	x(_x), y(_y), z(_z)
{}

tiny3d::Matrix3x3::Matrix3x3(tiny3d::Real xx, tiny3d::Real xy, tiny3d::Real xz, tiny3d::Real yx, tiny3d::Real yy, tiny3d::Real yz, tiny3d::Real zx, tiny3d::Real zy, tiny3d::Real zz) :
	x(xx, xy, xz), y(yx, yy, yz), z(zx, zy, zz)
{}

tiny3d::Matrix3x3::Matrix3x3(const tiny3d::Matrix3x3 &m) :
	x(m.x), y(m.y), z(m.z)
{}

tiny3d::Vector3 &tiny3d::Matrix3x3::operator[](tiny3d::UInt row)
{
	return reinterpret_cast<Vector3*>(&x)[row];
}

const tiny3d::Vector3 &tiny3d::Matrix3x3::operator[](tiny3d::UInt row) const
{
	return reinterpret_cast<const Vector3*>(&x)[row];
}

tiny3d::Matrix3x3 tiny3d::Identity3( void )
{
	return tiny3d::Matrix3x3(
		Real(1), Real(0), Real(0),
		Real(0), Real(1), Real(0),
		Real(0), Real(0), Real(1)
	);
}

tiny3d::Matrix3x3 tiny3d::Transp(const tiny3d::Matrix3x3 &m)
{
	return tiny3d::Matrix3x3(
		m.x.x, m.y.x, m.z.x,
		m.x.y, m.y.y, m.z.y,
		m.x.z, m.y.z, m.z.z
	);
}

tiny3d::Matrix3x3 tiny3d::Inv(const tiny3d::Matrix3x3 &m)
{
	const Real det =
		m.x.x * (m.y.y * m.z.z - m.z.y * m.y.z) -
		m.x.y * (m.y.x * m.z.z - m.y.z * m.z.x) +
		m.x.z * (m.y.x * m.z.y - m.y.y * m.z.x);
	if (det == Real(0)) { return tiny3d::Identity3(); }
	const Real invdet = 1 / det;
	return tiny3d::Matrix3x3(
		// x
		(m.y.y * m.z.z - m.z.y * m.y.z) * invdet, // x
		(m.x.z * m.z.y - m.x.y * m.z.z) * invdet, // y
		(m.x.y * m.y.z - m.x.z * m.y.y) * invdet, // z
		// y
		(m.y.z * m.z.x - m.y.x * m.z.z) * invdet, // x
		(m.x.x * m.z.z - m.x.z * m.z.x) * invdet, // y
		(m.y.x * m.x.z - m.x.x * m.y.z) * invdet, // z
		// z
		(m.y.x * m.z.y - m.z.x * m.y.y) * invdet, // x
		(m.z.x * m.x.y - m.x.x * m.z.y) * invdet, // y
		(m.x.x * m.y.y - m.y.x * m.x.y) * invdet  // z
	);
}

tiny3d::Matrix3x3 tiny3d::Euler(Real head, Real pitch, Real roll)
{
	const Real SINH = tiny3d::Sin(head);
	const Real SINP = tiny3d::Sin(pitch);
	const Real SINR = tiny3d::Sin(roll);
	const Real COSH = tiny3d::Cos(head);
	const Real COSP = tiny3d::Cos(pitch);
	const Real COSR = tiny3d::Cos(roll);
	return tiny3d::Matrix3x3(
		COSR*COSH - SINR*SINP*SINH, -SINR*COSP, COSR*SINH + SINR*SINP*COSH,
		SINR*COSH + COSR*SINP*SINH, COSR*COSP, SINR*SINH - COSR*SINP*COSH,
		-COSP*SINH, SINP, COSP*COSH
	);
}

tiny3d::Matrix3x3 tiny3d::AxisAngle(Vector3 u, Real angle)
{
	const Real SIN    = Sin(angle);
	const Real COS    = Cos(angle);
	const Real ICOS   = 1 - COS;
	const Real uxSIN  = u.x*SIN;
	const Real uySIN  = u.y*SIN;
	const Real uzSIN  = u.z*SIN;
	const Real uxICOS = u.x*ICOS;
	const Real uyICOS = u.y*ICOS;
	const Real uzuxICOS = u.z*uxICOS;
	const Real uyuxICOS = u.y*uxICOS;
	const Real uzuyICOS = u.z*uyICOS;
	return Matrix3x3(
		COS + u.x*uxICOS, uyuxICOS - uzSIN, uzuxICOS + uySIN,
		uyuxICOS + uzSIN, COS + u.y*uyICOS, uzuyICOS - uxSIN,
		uzuxICOS - uySIN, uzuyICOS + uxSIN, COS + u.z*u.z*ICOS
	);
}

tiny3d::Matrix3x3 tiny3d::RotateX(tiny3d::Real ang)
{
	const Real SIN = Sin(ang);
	const Real COS = Cos(ang);
	return Matrix3x3(
		Real(1), Real(0),  Real(0),
		Real(0), COS,     -SIN,
		Real(0), SIN,      COS
	);
}

tiny3d::Matrix3x3 tiny3d::RotateY(tiny3d::Real ang)
{
	const Real SIN = Sin(ang);
	const Real COS = Cos(ang);
	return Matrix3x3(
		 COS,     Real(0), SIN,
		 Real(0), Real(1), Real(0),
		-SIN,     Real(0), COS
	);
}

tiny3d::Matrix3x3 tiny3d::RotateZ(tiny3d::Real ang)
{
	const Real SIN = Sin(ang);
	const Real COS = Cos(ang);
	return Matrix3x3(
		COS,     -SIN,     Real(0),
		SIN,      COS,     Real(0),
		Real(0),  Real(0), Real(1)
	);
}

tiny3d::Matrix3x3 tiny3d::operator*(const Matrix3x3 &l, tiny3d::Matrix3x3 r)
{
	r = tiny3d::Transp(r);
	return tiny3d::Matrix3x3(
		tiny3d::Dot(l.x, r.x), tiny3d::Dot(l.x, r.y), tiny3d::Dot(l.x, r.z),
		tiny3d::Dot(l.y, r.x), tiny3d::Dot(l.y, r.y), tiny3d::Dot(l.y, r.z),
		tiny3d::Dot(l.z, r.x), tiny3d::Dot(l.z, r.y), tiny3d::Dot(l.z, r.z)
	);
}

tiny3d::Matrix3x3 tiny3d::operator*(const Matrix3x3 &l, tiny3d::Real r)
{
	return tiny3d::Matrix3x3(
		l.x.x * r, l.x.y * r, l.x.z * r,
		l.y.x * r, l.y.y * r, l.y.z * r,
		l.z.x * r, l.z.y * r, l.z.z * r
	);
}

tiny3d::Vector3 tiny3d::operator*(const tiny3d::Vector3 &l, const tiny3d::Matrix3x3 &r)
{
	return tiny3d::Vector3(
		tiny3d::Dot(l, r.x),
		tiny3d::Dot(l, r.y),
		tiny3d::Dot(l, r.z)
	);
}

tiny3d::Matrix2x2::Matrix2x2( void ) :
	x(), y()
{}

tiny3d::Matrix2x2::Matrix2x2(const tiny3d::Vector2 &_x, const tiny3d::Vector2 &_y) :
	x(_x), y(_y)
{}

tiny3d::Matrix2x2::Matrix2x2(tiny3d::Real xx, tiny3d::Real xy, tiny3d::Real yx, tiny3d::Real yy) :
	x(xx, xy), y(yx, yy)
{}

tiny3d::Matrix2x2::Matrix2x2(const tiny3d::Matrix2x2 &m) :
	x(m.x), y(m.y)
{}

tiny3d::Vector2 &tiny3d::Matrix2x2::operator[](tiny3d::UInt row)
{
	return reinterpret_cast<Vector2*>(&x)[row];
}

const tiny3d::Vector2 &tiny3d::Matrix2x2::operator[](tiny3d::UInt row) const
{
	return reinterpret_cast<const Vector2*>(&x)[row];
}

tiny3d::Matrix2x2 tiny3d::Identity2( void )
{
	return Matrix2x2(
		Real(1), Real(0),
		Real(0), Real(1)
	);
}

tiny3d::Matrix2x2 tiny3d::Transp(const tiny3d::Matrix2x2 &m)
{
	return tiny3d::Matrix2x2(
		m.x.x, m.y.x,
		m.x.y, m.y.y
	);
}

tiny3d::Matrix2x2 tiny3d::Inv(const tiny3d::Matrix2x2 &m)
{
	const Real det = m.x.x*m.y.y - m.x.y*m.y.x;
	if (det == Real(0)) { return tiny3d::Identity2(); }
	const Real invdet = 1 / det;
	return tiny3d::Matrix2x2(
		 m.y.y * invdet, -m.x.y * invdet,
		-m.y.x * invdet,  m.x.x * invdet
	);
}

tiny3d::Matrix2x2 tiny3d::Rotate2(Real angle)
{
	const Real SIN = Sin(angle);
	const Real COS = Cos(angle);
	return Matrix2x2(
		COS, -SIN,
		SIN,  COS
	);
}

tiny3d::Matrix2x2 tiny3d::operator*(const Matrix2x2 &l, tiny3d::Matrix2x2 r)
{
	r = tiny3d::Transp(r);
	return tiny3d::Matrix2x2(
		tiny3d::Dot(l.x, r.x), tiny3d::Dot(l.x, r.y),
		tiny3d::Dot(l.y, r.x), tiny3d::Dot(l.y, r.y)
	);
}

tiny3d::Matrix2x2 tiny3d::operator*(const Matrix2x2 &l, tiny3d::Real r)
{
	return tiny3d::Matrix2x2(
		l.x.x * r, l.x.y * r,
		l.y.x * r, l.y.y * r
	);
}

tiny3d::Vector2 tiny3d::operator*(const tiny3d::Vector2 &l, const tiny3d::Matrix2x2 &r)
{
	return tiny3d::Vector2(
		tiny3d::Dot(l, r.x),
		tiny3d::Dot(l, r.y)
	);
}

tiny3d::Real tiny3d::Matrix4x4::Dot(const tiny3d::Real *l, const tiny3d::Real *r)
{
	return l[0]*r[0] + l[1]*r[1] + l[2]*r[2] + l[3]*r[3];
}

tiny3d::Matrix4x4::Matrix4x4( void ) : m{ { Real(), Real(), Real(), Real() }, { Real(), Real(), Real(), Real() }, { Real(), Real(), Real(), Real() }, { Real(), Real(), Real(), Real() } }
{}

tiny3d::Matrix4x4::Matrix4x4(tiny3d::Real xx, tiny3d::Real xy, tiny3d::Real xz, tiny3d::Real xw, tiny3d::Real yx, tiny3d::Real yy, tiny3d::Real yz, tiny3d::Real yw, tiny3d::Real zx, tiny3d::Real zy, tiny3d::Real zz, tiny3d::Real zw, tiny3d::Real wx, tiny3d::Real wy, tiny3d::Real wz, tiny3d::Real ww) : m{ {xx, xy, xz, xw}, {yx, yy, yz, yw}, {zx, zy, zz, zw}, {wx, wy, wz, ww} }
{}

tiny3d::Real *tiny3d::Matrix4x4::operator[](tiny3d::UInt row)
{
	TINY3D_ASSERT(row < 4);
	return m[row];
}

const tiny3d::Real *tiny3d::Matrix4x4::operator[](UInt row) const
{
	TINY3D_ASSERT(row < 4);
	return m[row];
}

tiny3d::Matrix4x4 tiny3d::Matrix4x4::operator*(tiny3d::Matrix4x4 r) const
{
	r = Transp(r);
	return Matrix4x4(
		Dot(m[0], r.m[0]), Dot(m[0], r.m[1]), Dot(m[0], r.m[2]), Dot(m[0], r.m[3]),
		Dot(m[1], r.m[0]), Dot(m[1], r.m[1]), Dot(m[1], r.m[2]), Dot(m[1], r.m[3]),
		Dot(m[2], r.m[0]), Dot(m[2], r.m[1]), Dot(m[2], r.m[2]), Dot(m[2], r.m[3]),
		Dot(m[3], r.m[0]), Dot(m[3], r.m[1]), Dot(m[3], r.m[2]), Dot(m[3], r.m[3])
	);
}

tiny3d::Matrix4x4 tiny3d::Matrix4x4::operator*(tiny3d::Real r) const
{
	Matrix4x4 out;
	for (UInt y = 0; y < 4; ++y) {
		for (UInt x = 0; x < 4; ++x) {
			out[y][x] = m[y][x] * r;
		}
	}
	return out;
}

tiny3d::Matrix4x4 tiny3d::Identity4( void )
{
	return Matrix4x4(
		Real(1), Real(0), Real(0), Real(0),
		Real(0), Real(1), Real(0), Real(0),
		Real(0), Real(0), Real(1), Real(0),
		Real(0), Real(0), Real(0), Real(1)
	);
}

tiny3d::Matrix4x4 tiny3d::Transp(const tiny3d::Matrix4x4 &m)
{
	return Matrix4x4(
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]
	);
}

tiny3d::Matrix4x4 tiny3d::Transform(const tiny3d::Matrix3x3 &rot, const tiny3d::Vector3 &pos)
{
	return Matrix4x4(
		rot.x.x, rot.x.y, rot.x.z, pos.x,
		rot.y.x, rot.y.y, rot.y.z, pos.y,
		rot.z.x, rot.z.y, rot.z.z, pos.z,
		Real(0), Real(0), Real(0), Real(1)
	);
}

tiny3d::Matrix3x3 tiny3d::Rotation(const tiny3d::Matrix4x4 &transform)
{
	return Matrix3x3(
		transform[0][0], transform[0][1], transform[0][2],
		transform[1][0], transform[1][1], transform[1][2],
		transform[2][0], transform[2][1], transform[2][2]
	);
}

tiny3d::Vector3 tiny3d::Translation(const tiny3d::Matrix4x4 &transform)
{
	return Vector3(transform[0][3], transform[1][3], transform[2][3]);
}

tiny3d::Matrix4x4 tiny3d::Inv(const tiny3d::Matrix4x4 &m)
{
	const Real s0 = m[0][0] * m[1][1] - m[1][0] * m[0][1];
	const Real s1 = m[0][0] * m[1][2] - m[1][0] * m[0][2];
	const Real s2 = m[0][0] * m[1][3] - m[1][0] * m[0][3];
	const Real s3 = m[0][1] * m[1][2] - m[1][1] * m[0][2];
	const Real s4 = m[0][1] * m[1][3] - m[1][1] * m[0][3];
	const Real s5 = m[0][2] * m[1][3] - m[1][2] * m[0][3];
	const Real c5 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	const Real c4 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	const Real c3 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	const Real c2 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	const Real c1 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	const Real c0 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

	const Real det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
	if (det == 0) { return Identity4(); }
	const Real invdet = 1 / det;

	Matrix4x4 i;
	i[0][0] = ( m[1][1] * c5 - m[1][2] * c4 + m[1][3] * c3) * invdet;
	i[0][1] = (-m[0][1] * c5 + m[0][2] * c4 - m[0][3] * c3) * invdet;
	i[0][2] = ( m[3][1] * s5 - m[3][2] * s4 + m[3][3] * s3) * invdet;
	i[0][3] = (-m[2][1] * s5 + m[2][2] * s4 - m[2][3] * s3) * invdet;

	i[1][0] = (-m[1][0] * c5 + m[1][2] * c2 - m[1][3] * c1) * invdet;
	i[1][1] = ( m[0][0] * c5 - m[0][2] * c2 + m[0][3] * c1) * invdet;
	i[1][2] = (-m[3][0] * s5 + m[3][2] * s2 - m[3][3] * s1) * invdet;
	i[1][3] = ( m[2][0] * s5 - m[2][2] * s2 + m[2][3] * s1) * invdet;

	i[2][0] = ( m[1][0] * c4 - m[1][1] * c2 + m[1][3] * c0) * invdet;
	i[2][1] = (-m[0][0] * c4 + m[0][1] * c2 - m[0][3] * c0) * invdet;
	i[2][2] = ( m[3][0] * s4 - m[3][1] * s2 + m[3][3] * s0) * invdet;
	i[2][3] = (-m[2][0] * s4 + m[2][1] * s2 - m[2][3] * s0) * invdet;

	i[3][0] = (-m[1][0] * c3 + m[1][1] * c1 - m[1][2] * c0) * invdet;
	i[3][1] = ( m[0][0] * c3 - m[0][1] * c1 + m[0][2] * c0) * invdet;
	i[3][2] = (-m[3][0] * s3 + m[3][1] * s1 - m[3][2] * s0) * invdet;
	i[3][3] = ( m[2][0] * s3 - m[2][1] * s1 + m[2][2] * s0) * invdet;

	return i;
}

tiny3d::Vector3 tiny3d::operator*(const tiny3d::Vector3 &r, const Matrix4x4 &l)
{
	return Vector3(
		tiny3d::Dot(r, Vector3(l[0][0], l[0][1], l[0][2])) + l[0][3],
		tiny3d::Dot(r, Vector3(l[1][0], l[1][1], l[1][2])) + l[1][3],
		tiny3d::Dot(r, Vector3(l[2][0], l[2][1], l[2][2])) + l[2][3]
	);
}
