#include "tiny_structs.h"

using namespace tiny3d;

tiny3d::Rect tiny3d::Clip(tiny3d::Rect a, tiny3d::Rect b)
{
	Rect r;
	r.a.x = Max(a.a.x, b.a.x);
	r.b.x = Min(a.b.x, b.b.x);
	r.a.y = Max(a.a.y, b.a.y);
	r.b.y = Min(a.b.y, b.b.y);
	return r;
}

tiny3d::URect tiny3d::Clip(tiny3d::URect a, tiny3d::URect b)
{
	URect r;
	r.a.x = Max(a.a.x, b.a.x);
	r.b.x = Min(a.b.x, b.b.x);
	r.a.y = Max(a.a.y, b.a.y);
	r.b.y = Min(a.b.y, b.b.y);
	return r;
}

const tiny3d::SInt COLOR_STEP = (TINY3D_BYTE_MAX + 1) / TINY3D_SHADES_PER_COLOR_CHANNEL; // the granularity between one step of 8 bit and one step of 5 bit in terms of 8 bit
const tiny3d::SInt COLOR_MAX  = COLOR_STEP - 1; // relies on COLOR_STEP being a multiple of 2 (which it should always be)

tiny3d::Color Dither(tiny3d::Color c, tiny3d::UPoint p, const tiny3d::Real *D, tiny3d::UInt N)
{
	// 8   -> 5   bits
	// 256 -> 32 shades

	// this is the threshhold the color needs to clear to shift one hue
	const SInt threshold = SInt(COLOR_STEP * D[(p.y % N) * N + (p.x % N)]);
	// the color ratios, i.e. how close one color is to shifting to another
	const SInt r_ratio = SInt(c.r & COLOR_MAX);
	const SInt g_ratio = SInt(c.g & COLOR_MAX);
	const SInt b_ratio = SInt(c.b & COLOR_MAX);
	// the final color
	const Color out = {
		Byte(Clamp(0, SInt(c.r) + ((r_ratio > threshold) ? COLOR_STEP : 0), 255)),
		Byte(Clamp(0, SInt(c.g) + ((g_ratio > threshold) ? COLOR_STEP : 0), 255)),
		Byte(Clamp(0, SInt(c.b) + ((b_ratio > threshold) ? COLOR_STEP : 0), 255)),
		c.blend
	};
	return out;
}


const tiny3d::Real D2x2[4] = {
	Real(1) / 4, Real(3) / 4,
	Real(4) / 4, Real(2) / 4
};

const tiny3d::Real D3x3[9] = {
	Real(1) / 9, Real(8) / 9, Real(4) / 9,
	Real(7) / 9, Real(6) / 9, Real(3) / 9,
	Real(5) / 9, Real(2) / 9, Real(9) / 9
};

const tiny3d::Real D4x4[16] = {
	Real(1)  / 16, Real(9)  / 16, Real(3)  / 16, Real(11) / 16,
	Real(13) / 16, Real(5)  / 16, Real(15) / 16, Real(7)  / 16,
	Real(4)  / 16, Real(12) / 16, Real(2)  / 16, Real(10) / 16,
	Real(16) / 16, Real(8)  / 16, Real(14) / 16, Real(6)  / 16
};

const tiny3d::Real D8x8[64] = {
	Real(1)  / 64, Real(49) / 64, Real(13) / 64, Real(61) / 64, Real(4)  / 64, Real(52) / 64, Real(16) / 64, Real(64) / 64,
	Real(33) / 64, Real(17) / 64, Real(45) / 64, Real(29) / 64, Real(36) / 64, Real(20) / 64, Real(48) / 64, Real(32) / 64,
	Real(9)  / 64, Real(57) / 64, Real(5)  / 64, Real(53) / 64, Real(12) / 64, Real(60) / 64, Real(8)  / 64, Real(56) / 64,
	Real(41) / 64, Real(25) / 64, Real(37) / 64, Real(21) / 64, Real(44) / 64, Real(28) / 64, Real(40) / 64, Real(24) / 64,
	Real(3)  / 64, Real(51) / 64, Real(15) / 64, Real(63) / 64, Real(2)  / 64, Real(50) / 64, Real(14) / 64, Real(62) / 64,
	Real(35) / 64, Real(19) / 64, Real(47) / 64, Real(31) / 64, Real(34) / 64, Real(18) / 64, Real(46) / 64, Real(30) / 64,
	Real(11) / 64, Real(59) / 64, Real(7)  / 64, Real(55) / 64, Real(10) / 64, Real(58) / 64, Real(6)  / 64, Real(54) / 64,
	Real(43) / 64, Real(27) / 64, Real(39) / 64, Real(23) / 64, Real(42) / 64, Real(26) / 64, Real(38) / 64, Real(22) / 64
};

tiny3d::Color tiny3d::operator+(tiny3d::Color l, tiny3d::Color r)
{
	l.r = Byte(tiny3d::Min(UInt(l.r) + UInt(r.r), UInt(0xff)));
	l.g = Byte(tiny3d::Min(UInt(l.g) + UInt(r.g), UInt(0xff)));
	l.b = Byte(tiny3d::Min(UInt(l.b) + UInt(r.b), UInt(0xff)));
	return l;
}

tiny3d::Color tiny3d::operator-(tiny3d::Color l, tiny3d::Color r)
{
	l.r = Byte(tiny3d::Max(SInt(l.r) - SInt(r.r), SInt(0x00)));
	l.g = Byte(tiny3d::Max(SInt(l.g) - SInt(r.g), SInt(0x00)));
	l.b = Byte(tiny3d::Max(SInt(l.b) - SInt(r.b), SInt(0x00)));
	return l;
}

tiny3d::Color tiny3d::operator*(tiny3d::Color l, tiny3d::Color r)
{
	l.r = Byte(((UXInt(l.r) + 1) * (UXInt(r.r) + 1) - 1) >> (sizeof(Byte)*CHAR_BIT));
	l.g = Byte(((UXInt(l.g) + 1) * (UXInt(r.g) + 1) - 1) >> (sizeof(Byte)*CHAR_BIT));
	l.b = Byte(((UXInt(l.b) + 1) * (UXInt(r.b) + 1) - 1) >> (sizeof(Byte)*CHAR_BIT));
	return l;
}

tiny3d::Color tiny3d::operator*(tiny3d::Color l, tiny3d::Real r)
{
	l.r = Byte(Clamp(0, SInt(SInt(l.r) * r), SInt(TINY3D_BYTE_MAX)));
	l.g = Byte(Clamp(0, SInt(SInt(l.g) * r), SInt(TINY3D_BYTE_MAX)));
	l.b = Byte(Clamp(0, SInt(SInt(l.b) * r), SInt(TINY3D_BYTE_MAX)));
	return l;
}

tiny3d::Color tiny3d::operator/(tiny3d::Color l, tiny3d::Real r)
{
	l.r = Byte(Clamp(0, SInt(SInt(l.r) / r), SInt(TINY3D_BYTE_MAX)));
	l.g = Byte(Clamp(0, SInt(SInt(l.g) / r), SInt(TINY3D_BYTE_MAX)));
	l.b = Byte(Clamp(0, SInt(SInt(l.b) / r), SInt(TINY3D_BYTE_MAX)));
	return l;
}

tiny3d::Color tiny3d::Dither2x2(tiny3d::Color c, tiny3d::UPoint p)
{
	return Dither(c, p, D2x2, 2);
}

tiny3d::Color tiny3d::Dither3x3(tiny3d::Color c, tiny3d::UPoint p)
{
	return Dither(c, p, D3x3, 3);
}

tiny3d::Color tiny3d::Dither4x4(tiny3d::Color c, tiny3d::UPoint p)
{
	return Dither(c, p, D4x4, 4);
}

tiny3d::Color tiny3d::Dither8x8(tiny3d::Color c, tiny3d::UPoint p)
{
	return Dither(c, p, D8x8, 8);
}

tiny3d::Byte tiny3d::Illum(tiny3d::Color c)
{
	// r = ~20%, g = ~70%, b = ~10%
	UInt r = UInt(c.r) * 2;
	UInt g = UInt(c.g) * 7;
	UInt b = UInt(c.b);
	UInt gray = (r + g + b) / 10;
	return Byte(gray);
}

tiny3d::WVertex tiny3d::ToW(const tiny3d::Vertex &v)
{
	WVertex iv;
	iv.p.x = SInt(v.v.x);
	iv.p.y = SInt(v.v.y);
	iv.w   = 1 / v.v.z;
	iv.t   = v.t   * iv.w;
	iv.c.x = v.c.r * iv.w;
	iv.c.y = v.c.g * iv.w;
	iv.c.z = v.c.b * iv.w;
	return iv;
}
