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

tiny3d::Color Dither(tiny3d::Color c, tiny3d::UPoint p, const tiny3d::SInt *D, tiny3d::UInt N)
{
	// the dither value to offset the current color by
	const SInt color_offset = D[(p.y % N) * N + (p.x % N)];
	// the final color (NOTE: not truncated here, instead the Texture and Image class converts from 8 bit to 5 bit)
	const Color out = {
		Byte(Clamp(0, SInt(c.r) + color_offset, 255)),
		Byte(Clamp(0, SInt(c.g) + color_offset, 255)),
		Byte(Clamp(0, SInt(c.b) + color_offset, 255)),
		c.blend
	};
	return out;
}


const tiny3d::SInt D2x2[4] = {
	SInt(COLOR_STEP * (Real(0) / 4)), SInt(COLOR_STEP * (Real(2) / 4)),
	SInt(COLOR_STEP * (Real(3) / 4)), SInt(COLOR_STEP * (Real(1) / 4))
};

const tiny3d::SInt D3x3[9] = {
	SInt(COLOR_STEP * (Real(0) / 9)), SInt(COLOR_STEP * (Real(7) / 9)), SInt(COLOR_STEP * (Real(3) / 9)),
	SInt(COLOR_STEP * (Real(6) / 9)), SInt(COLOR_STEP * (Real(5) / 9)), SInt(COLOR_STEP * (Real(2) / 9)),
	SInt(COLOR_STEP * (Real(4) / 9)), SInt(COLOR_STEP * (Real(1) / 9)), SInt(COLOR_STEP * (Real(8) / 9))
};

const tiny3d::SInt D4x4[16] = {
	SInt(COLOR_STEP * (Real(0)  / 16)), SInt(COLOR_STEP * (Real(8)  / 16)), SInt(COLOR_STEP * (Real(2)  / 16)), SInt(COLOR_STEP * (Real(10) / 16)),
	SInt(COLOR_STEP * (Real(12) / 16)), SInt(COLOR_STEP * (Real(4)  / 16)), SInt(COLOR_STEP * (Real(14) / 16)), SInt(COLOR_STEP * (Real(6)  / 16)),
	SInt(COLOR_STEP * (Real(3)  / 16)), SInt(COLOR_STEP * (Real(11) / 16)), SInt(COLOR_STEP * (Real(1)  / 16)), SInt(COLOR_STEP * (Real(9)  / 16)),
	SInt(COLOR_STEP * (Real(15) / 16)), SInt(COLOR_STEP * (Real(7)  / 16)), SInt(COLOR_STEP * (Real(13) / 16)), SInt(COLOR_STEP * (Real(5)  / 16))
};

const tiny3d::SInt D8x8[64] = {
	SInt(COLOR_STEP * (Real(0)  / 64)), SInt(COLOR_STEP * (Real(48) / 64)), SInt(COLOR_STEP * (Real(12) / 64)), SInt(COLOR_STEP * (Real(60) / 64)), SInt(COLOR_STEP * (Real(3)  / 64)), SInt(COLOR_STEP * (Real(51) / 64)), SInt(COLOR_STEP * (Real(15) / 64)), SInt(COLOR_STEP * (Real(63) / 64)),
	SInt(COLOR_STEP * (Real(32) / 64)), SInt(COLOR_STEP * (Real(16) / 64)), SInt(COLOR_STEP * (Real(44) / 64)), SInt(COLOR_STEP * (Real(28) / 64)), SInt(COLOR_STEP * (Real(35) / 64)), SInt(COLOR_STEP * (Real(19) / 64)), SInt(COLOR_STEP * (Real(47) / 64)), SInt(COLOR_STEP * (Real(31) / 64)),
	SInt(COLOR_STEP * (Real(8)  / 64)), SInt(COLOR_STEP * (Real(56) / 64)), SInt(COLOR_STEP * (Real(4)  / 64)), SInt(COLOR_STEP * (Real(52) / 64)), SInt(COLOR_STEP * (Real(11) / 64)), SInt(COLOR_STEP * (Real(59) / 64)), SInt(COLOR_STEP * (Real(7)  / 64)), SInt(COLOR_STEP * (Real(55) / 64)),
	SInt(COLOR_STEP * (Real(40) / 64)), SInt(COLOR_STEP * (Real(24) / 64)), SInt(COLOR_STEP * (Real(36) / 64)), SInt(COLOR_STEP * (Real(20) / 64)), SInt(COLOR_STEP * (Real(43) / 64)), SInt(COLOR_STEP * (Real(27) / 64)), SInt(COLOR_STEP * (Real(39) / 64)), SInt(COLOR_STEP * (Real(23) / 64)),
	SInt(COLOR_STEP * (Real(2)  / 64)), SInt(COLOR_STEP * (Real(50) / 64)), SInt(COLOR_STEP * (Real(14) / 64)), SInt(COLOR_STEP * (Real(62) / 64)), SInt(COLOR_STEP * (Real(1)  / 64)), SInt(COLOR_STEP * (Real(49) / 64)), SInt(COLOR_STEP * (Real(13) / 64)), SInt(COLOR_STEP * (Real(61) / 64)),
	SInt(COLOR_STEP * (Real(34) / 64)), SInt(COLOR_STEP * (Real(18) / 64)), SInt(COLOR_STEP * (Real(46) / 64)), SInt(COLOR_STEP * (Real(30) / 64)), SInt(COLOR_STEP * (Real(33) / 64)), SInt(COLOR_STEP * (Real(17) / 64)), SInt(COLOR_STEP * (Real(45) / 64)), SInt(COLOR_STEP * (Real(29) / 64)),
	SInt(COLOR_STEP * (Real(10) / 64)), SInt(COLOR_STEP * (Real(58) / 64)), SInt(COLOR_STEP * (Real(6)  / 64)), SInt(COLOR_STEP * (Real(54) / 64)), SInt(COLOR_STEP * (Real(9)  / 64)), SInt(COLOR_STEP * (Real(57) / 64)), SInt(COLOR_STEP * (Real(5)  / 64)), SInt(COLOR_STEP * (Real(53) / 64)),
	SInt(COLOR_STEP * (Real(42) / 64)), SInt(COLOR_STEP * (Real(26) / 64)), SInt(COLOR_STEP * (Real(38) / 64)), SInt(COLOR_STEP * (Real(22) / 64)), SInt(COLOR_STEP * (Real(41) / 64)), SInt(COLOR_STEP * (Real(25) / 64)), SInt(COLOR_STEP * (Real(37) / 64)), SInt(COLOR_STEP * (Real(21) / 64))
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

tiny3d::XWVertex tiny3d::ToXW(const tiny3d::Vertex &v)
{
	XWVertex iv;
	iv.p.x = SInt(v.v.x);
	iv.p.y = SInt(v.v.y);
	iv.w = XReal::Inverse(v.v.z);
	iv.u = XReal(v.t.x) * iv.w;
	iv.v = XReal(v.t.y) * iv.w;
	iv.r = XReal(SInt(v.c.r)) * iv.w;
	iv.g = XReal(SInt(v.c.g)) * iv.w;
	iv.b = XReal(SInt(v.c.b)) * iv.w;
	return iv;
}
