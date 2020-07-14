#include "tiny_draw.h"
#include "tiny_math.h"
#include "tiny_simd.h"
#include "tiny_overlay.h"

using namespace tiny3d;

namespace internal_impl
{
	struct IVertex
	{
		Point p;
		float u, v;    // 1/tcoord (non-normalized)
		float r, g, b; // 1/color
		float w;       // 1/z
	};

	IVertex operator+(IVertex a, IVertex b) { return IVertex{ { a.p.x + b.p.x, a.p.y + b.p.y }, a.u + b.u, a.v + b.v, a.r + b.r, a.g + b.g, a.b + b.b, a.w + b.w }; }
	IVertex operator-(IVertex a, IVertex b) { return IVertex{ { a.p.x - b.p.x, a.p.y - b.p.y }, a.u - b.u, a.v - b.v, a.r - b.r, a.g - b.g, a.b - b.b, a.w - b.w }; }
	IVertex operator*(IVertex a, Real b)    { return IVertex{ { SInt(a.p.x * b), SInt(a.p.y * b) }, a.u * b, a.v * b, a.r * b, a.g * b, a.b * b, a.w * b }; }

	struct ILVertex
	{
		Point p;
		float u, v;   // 1/tcoord (non-normalized)
		float lu, lv; // 1/lcoord (non-normalized)
		float w;      // 1/z
	};

	void DrawLine(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, internal_impl::IVertex a, internal_impl::IVertex b, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect);
	void DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::IVertex &a, const internal_impl::IVertex &b, const internal_impl::IVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect);
	void DrawTriangle_Fast(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::IVertex &a, const internal_impl::IVertex &b, const internal_impl::IVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect);
	void DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::ILVertex &a, const internal_impl::ILVertex &b, const internal_impl::ILVertex &c, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap, const tiny3d::URect *dst_rect);
	void DrawTriangle_Fast(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::ILVertex &a, const internal_impl::ILVertex &b, const internal_impl::ILVertex &c, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap, const tiny3d::URect *dst_rect);
	template < typename src_t >
	void DrawRegion(tiny3d::Image &dst, tiny3d::Rect dst_region, const src_t &src, tiny3d::Rect src_region, const tiny3d::URect *dst_rect);
	tiny3d::Point DrawChars(tiny3d::Image &dst, tiny3d::Point p, const char *ch, tiny3d::UInt ch_num, tiny3d::Color color, tiny3d::UInt scale, const tiny3d::URect *dst_rect);
}

float BLerp(float a, float b, float c, float l0, float l1, float l2)
{
	return a * l0 + b * l1 + c * l2;
}

internal_impl::IVertex ToI(const tiny3d::Vertex &v, const tiny3d::Texture *tex)
{
	internal_impl::IVertex iv;
	iv.p.x = SInt(v.v.x);
	iv.p.y = SInt(v.v.y);
//	iv.w   = 1.0f / v.v.z.ToFloat();
	iv.w   = 1.0f / v.v.z;
//	iv.u = v.t.x.ToFloat() * (tex != nullptr ? float(tex->GetWidth()) : 1.0f) * iv.w;
//	iv.v = (1.0f - v.t.y.ToFloat()) * (tex != nullptr ? float(tex->GetHeight()) : 1.0f) * iv.w;
	iv.u = v.t.x * (tex != nullptr ? float(tex->GetWidth()) : 1.0f) * iv.w;
	iv.v = (1.0f - v.t.y) * (tex != nullptr ? float(tex->GetHeight()) : 1.0f) * iv.w;
	iv.r = float(v.c.r) * iv.w;
	iv.g = float(v.c.g) * iv.w;
	iv.b = float(v.c.b) * iv.w;
	return iv;
}

internal_impl::ILVertex ToI(const tiny3d::LVertex &v, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap)
{
	internal_impl::ILVertex iv;
	iv.p.x = SInt(v.v.x);
	iv.p.y = SInt(v.v.y);
//	iv.w   = 1.0f / v.v.z.ToFloat();
	iv.w   = 1.0f / v.v.z;
//	iv.u = v.t.x.ToFloat() * (tex != nullptr ? float(tex->GetWidth()) : 1.0f) * iv.w;
//	iv.v = (1.0f - v.t.y.ToFloat()) * (tex != nullptr ? float(tex->GetHeight()) : 1.0f) * iv.w;
	iv.u = v.t.x * (tex != nullptr ? float(tex->GetWidth()) : 1.0f) * iv.w;
	iv.v = (1.0f - v.t.y) * (tex != nullptr ? float(tex->GetHeight()) : 1.0f) * iv.w;
	iv.lu = v.l.x * float(lightmap.GetWidth()) * iv.w;
	iv.lv = (1.0f - v.l.y) * float(lightmap.GetHeight()) * iv.w;
	return iv;
}

internal_impl::IVertex ToI(const tiny3d::Vertex &v)
{
	return ToI(v, nullptr);
}

void tiny3d::DrawPoint(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::Vertex &a, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	const URect srect = URect{ { 0, 0 }, { UInt(dst.GetWidth()), UInt(dst.GetHeight()) } };
	const URect rect = (dst_rect != nullptr) ? tiny3d::Clip(*dst_rect, srect) : srect;
	if (SInt(a.v.x) >= SInt(rect.a.x) && SInt(a.v.y) >= SInt(rect.a.y) && SInt(a.v.x) < SInt(rect.b.x) && SInt(a.v.y) < SInt(rect.b.y)) {

		const UPoint q     = { UInt(SInt(a.v.x)), UInt(SInt(a.v.y)) };
		const Color  pixel = dst.GetColor(q);
		const UInt   zi    = q.x + q.y * dst.GetWidth();
//		const float  sz    = a.v.z.ToFloat();
		const float  sz    = a.v.z;
		const float  dz    = (zread != nullptr) ? (*zread)[zi] : std::numeric_limits<float>::infinity();

		if (sz <= dz && pixel.blend != Color::Transparent) {

			const Color col = a.c;
			const Color texel = (tex != nullptr) ? tex->GetColor(a.t) : Color{ 255, 255, 255, Color::Solid };

			switch (texel.blend)
			{
			case Color::Solid:
				dst.SetColor(q, Dither2x2(texel * col, q));
				if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
				break;
			case Color::AddAlpha:
				dst.SetColor(q, Dither2x2(dst.GetColor(q) + texel * col, q));
				break;
			case Color::Emissive:
				dst.SetColor(q, texel);
				if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
				break;
			case Color::EmissiveAddAlpha:
				dst.SetColor(q, Dither2x2(dst.GetColor(q) + texel, q));
				break;
			default: break;
			}
		}
	}
}

void internal_impl::DrawLine(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, internal_impl::IVertex a, internal_impl::IVertex b, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	SInt min_x = 0;
	SInt max_x = SInt(dst.GetWidth()) - 1;
	SInt min_y = 0;
	SInt max_y = SInt(dst.GetHeight()) - 1;
	if (dst_rect != nullptr) {
		min_y = tiny3d::Max(min_y, SInt(dst_rect->a.y));
		max_y = tiny3d::Min(max_y, SInt(dst_rect->b.y) - 1);
		min_x = tiny3d::Max(min_x, SInt(dst_rect->a.x));
		max_x = tiny3d::Min(max_x, SInt(dst_rect->b.x) - 1);
	}

	// NOTE: Discard or clip lines.
	// BUG: Clipping is off by a few pixels here and there at times.
	if (a.p.x > b.p.x) { tiny3d::Swap(a, b); }
	if (a.p.x > max_x || b.p.x < min_x) { return; }
	if (a.p.x <  min_x && b.p.x >= min_x) { a = tiny3d::Lerp(a, b, Real(min_x - a.p.x) / Real(b.p.x - a.p.x)); } // NOTE: Line is clipping left border -> Clip and store in A.
	if (a.p.x <= max_x && b.p.x >  max_x) { b = tiny3d::Lerp(a, b, Real(max_x - a.p.x) / Real(b.p.x - a.p.x)); } // NOTE: Line is clipping right border -> Clip and store in B.
	if (a.p.y > b.p.y) { tiny3d::Swap(a, b); }
	if (a.p.y > max_y || b.p.y < min_y) { return; }
	if (a.p.y <  min_y && b.p.y >= min_y) { a = tiny3d::Lerp(a, b, Real(min_y - a.p.y) / Real(b.p.y - a.p.y)); } // NOTE: Line is clipping upper border -> Clip and store in A.
	if (a.p.y <= max_y && b.p.y >  max_y) { b = tiny3d::Lerp(a, b, Real(max_y - a.p.y) / Real(b.p.y - a.p.y)); } // NOTE: Line is clipping lower border -> Clip and store in B.

	const SInt dx = b.p.x - a.p.x;
	const SInt dy = b.p.y - a.p.y;
	const SInt steps = Abs(dx) > Abs(dy) ? Abs(dx) : Abs(dy);
	const float x_inc = float(dx) / steps;
	const float y_inc = float(dy) / steps;
	const float w_inc = (b.w - a.w) / steps;
	const float u_inc = (b.u - a.u) / steps;
	const float v_inc = (b.v - a.v) / steps;
	const float r_inc = (b.r - a.r) / steps;
	const float g_inc = (b.g - a.g) / steps;
	const float b_inc = (b.b - a.b) / steps;

	float X = a.p.x;
	float Y = a.p.y;
	float W = a.w;
	float U = a.u;
	float V = a.v;
	float R = a.r;
	float G = a.g;
	float B = a.b;
	for (SInt i = 0; i <= steps; i++) {
		Point p = { SInt(X), SInt(Y) };
		if (p.x >= min_x && p.x <= max_x && p.y >= min_y && p.y <= max_y) {

			const UPoint q     = { UInt(p.x), UInt(p.y) };
			const Color  pixel = dst.GetColor(q);
			const UInt   zi    = q.x + q.y * dst.GetWidth();
			const float  sz    = 1 / W;
			const float  dz    = (zread != nullptr) ? (*zread)[zi] : std::numeric_limits<float>::infinity();

			if (sz <= dz && pixel.blend != Color::Transparent) { // use transparency bit as a 1-bit stencil

				const Color col = {
					Byte(R * sz),
					Byte(G * sz),
					Byte(B * sz),
					Color::Solid
				};

				const Color texel = (tex != nullptr) ? tex->GetColor(UPoint{ UInt(U * sz), UInt(V * sz) }) : Color{ 255, 255, 255, Color::Solid };

				switch (texel.blend)
				{
				case Color::Solid:
					dst.SetColor(q, Dither2x2(texel * col, q));
					if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
					break;
				case Color::AddAlpha:
					dst.SetColor(q, Dither2x2(dst.GetColor(q) + texel * col, q));
					break;
				case Color::Emissive:
					dst.SetColor(q, texel);
					if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
					break;
				case Color::EmissiveAddAlpha:
					dst.SetColor(q, Dither2x2(dst.GetColor(q) + texel, q));
					break;
				default: break;
				}
			}

		}
		X += x_inc;
		Y += y_inc;
		W += w_inc;
		U += u_inc;
		V += v_inc;
		R += r_inc;
		G += g_inc;
		B += b_inc;
	}
}

void tiny3d::DrawLine(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	internal_impl::DrawLine(dst, zread, zwrite, ToI(a, tex), ToI(b, tex), tex, dst_rect);
}

struct WidePoint
{
	WideSInt x, y;
};
struct WideColor
{
	WideSInt r, g, b, blend;
};

WideColor operator+(WideColor l, const WideColor &r)
{
	l.r += r.r;
	l.g += r.g;
	l.b += r.b;
	return l;
}

WideColor operator-(WideColor l, const WideColor &r)
{
	l.r -= r.r;
	l.g -= r.g;
	l.b -= r.b;
	return l;
}

WideColor operator*(WideColor l, const WideReal &r)
{
	return WideColor{
		WideSInt(WideReal(l.r) * r),
		WideSInt(WideReal(l.g) * r),
		WideSInt(WideReal(l.b) * r),
		l.blend
	};
}

tiny3d::SXInt DetermineHalfspace(tiny3d::Point a, tiny3d::Point b, tiny3d::Point point)
{
	return tiny3d::SXInt(b.x - a.x) * tiny3d::SXInt(point.y - a.y) - tiny3d::SXInt(b.y - a.y) * tiny3d::SXInt(point.x - a.x);
}

tiny3d::WideSInt DetermineHalfspace_Fast(tiny3d::Point a, tiny3d::Point b, const WidePoint &point)
{
	return WideSInt(b.x - a.x) * (point.y - WideSInt(a.y)) - WideSInt(b.y - a.y) * (point.x - WideSInt(a.x));
}

bool IsTopLeft(tiny3d::Point a, tiny3d::Point b)
{
	// strictly connected to winding order
	return (a.x < b.x && b.y == a.y) || (a.y > b.y);
}

/*bool ShouldDivide(SXInt req_prec)
{
	return req_prec > (SXInt(1) << Real::Precision());
}*/

internal_impl::IVertex MidVertex(const internal_impl::IVertex &a, const internal_impl::IVertex &b)
{
	internal_impl::IVertex ab;
	ab.p.x = a.p.x / 2 + b.p.x / 2;
	ab.p.y = a.p.y / 2 + b.p.y / 2;
	ab.u   = a.u   / 2 + b.u   / 2;
	ab.v   = a.v   / 2 + b.v   / 2;
	ab.r   = a.r   / 2 + b.r   / 2;
	ab.g   = a.g   / 2 + b.g   / 2;
	ab.b   = a.b   / 2 + b.b   / 2;
	ab.w   = a.w   / 2 + b.w   / 2;
	return ab;
}

void DrawSubdivTri(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::IVertex &a, const internal_impl::IVertex &b, const internal_impl::IVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	internal_impl::IVertex ab = MidVertex(a, b);
	internal_impl::IVertex bc = MidVertex(b, c);
	internal_impl::IVertex ca = MidVertex(c, a);
	internal_impl::DrawTriangle(dst, zread, zwrite, a,  ab, ca, tex, dst_rect);
	internal_impl::DrawTriangle(dst, zread, zwrite, ab, b,  bc, tex, dst_rect);
	internal_impl::DrawTriangle(dst, zread, zwrite, ca, bc, c,  tex, dst_rect);
	internal_impl::DrawTriangle(dst, zread, zwrite, ca, ab, bc, tex, dst_rect);
}

void internal_impl::DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::IVertex &a, const internal_impl::IVertex &b, const internal_impl::IVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	// AABB Clipping
	SInt min_y = tiny3d::Max(tiny3d::Min(a.p.y, b.p.y, c.p.y), SInt(0));
	SInt max_y = tiny3d::Min(tiny3d::Max(a.p.y, b.p.y, c.p.y), SInt(dst.GetHeight() - 1));
	if (max_y - min_y <= 0) { return; }
	SInt min_x = tiny3d::Max(tiny3d::Min(a.p.x, b.p.x, c.p.x), SInt(0));
	SInt max_x = tiny3d::Min(tiny3d::Max(a.p.x, b.p.x, c.p.x), SInt(dst.GetWidth() - 1));
	if (max_x - min_x <= 0) { return; }

	if (dst_rect != nullptr) {
		min_y = SInt(tiny3d::Max(UInt(min_y), dst_rect->a.y));
		max_y = SInt(tiny3d::Min(UInt(max_y), dst_rect->b.y - 1));
		min_x = SInt(tiny3d::Max(UInt(min_x), dst_rect->a.x));
		max_x = SInt(tiny3d::Min(UInt(max_x), dst_rect->b.x - 1));
	}

	// Triangle setup
	tiny3d::Point p    = { min_x, min_y };
	SXInt         w0_y = DetermineHalfspace(b.p, c.p, p);
	SXInt         w1_y = DetermineHalfspace(c.p, a.p, p);
	SXInt         w2_y = DetermineHalfspace(a.p, b.p, p);

//	if (ShouldDivide(w0_y + w1_y + w2_y)) {
//		DrawSubdivTri(dst, zbuf, a, b, c, tex, dst_rect);
//		return;
//	}

	// Interpolation/triangle setup
	const SInt w2_x_inc        = a.p.y - b.p.y;
	const SInt w2_y_inc        = b.p.x - a.p.x;
	const SInt w0_x_inc        = b.p.y - c.p.y;
	const SInt w0_y_inc        = c.p.x - b.p.x;
	const SInt w1_x_inc        = c.p.y - a.p.y;
	const SInt w1_y_inc        = a.p.x - c.p.x;
	const float sum_inv_area_x2 = 1.0f / SInt(w0_y + w1_y + w2_y);
	float l0_y           = SInt(w0_y) * sum_inv_area_x2;
	float l1_y           = SInt(w1_y) * sum_inv_area_x2;
	float l2_y           = SInt(w2_y) * sum_inv_area_x2;
	const float l0_x_inc = w0_x_inc * sum_inv_area_x2;
	const float l1_x_inc = w1_x_inc * sum_inv_area_x2;
	const float l2_x_inc = w2_x_inc * sum_inv_area_x2;
	const float l0_y_inc = w0_y_inc * sum_inv_area_x2;
	const float l1_y_inc = w1_y_inc * sum_inv_area_x2;
	const float l2_y_inc = w2_y_inc * sum_inv_area_x2;

	w0_y += IsTopLeft(b.p, c.p) ? 0 : -1; // add offsets to coordinates to enforce fill convention
	w1_y += IsTopLeft(c.p, a.p) ? 0 : -1;
	w2_y += IsTopLeft(a.p, b.p) ? 0 : -1;

	for (p.y = min_y; p.y <= max_y; ++p.y) {

		SInt w0 = SInt(w0_y);
		SInt w1 = SInt(w1_y);
		SInt w2 = SInt(w2_y);

		float l0 = l0_y;
		float l1 = l1_y;
		float l2 = l2_y;

		for (p.x = min_x; p.x <= max_x; ++p.x) {

			if ((w0 | w1 | w2) >= 0) {

				const UPoint q     = { UInt(p.x), UInt(p.y) };
				const Color  pixel = dst.GetColor(q);
				const UInt   zi    = q.x + q.y * dst.GetWidth();
				const float  sz    = 1.0f / (a.w * l0 + b.w * l1 + c.w * l2);
				const float  dz    = (zread != nullptr) ? (*zread)[zi] : std::numeric_limits<float>::infinity();

				if (sz <= dz && pixel.blend != Color::Transparent) { // use transparency bit as a 1-bit stencil

					const float L0 = l0 * sz;
					const float L1 = l1 * sz;
					const float L2 = l2 * sz;

					const Color col = {
						Byte(a.r * L0 + b.r * L1 + c.r * L2),
						Byte(a.g * L0 + b.g * L1 + c.g * L2),
						Byte(a.b * L0 + b.b * L1 + c.b * L2),
						Color::Solid
					};

					const Color texel = (tex != nullptr) ? tex->GetColor(UPoint{ UInt(a.u * L0 + b.u * L1 + c.u * L2), UInt(a.v * L0 + b.v * L1 + c.v * L2) }) : Color{ 255, 255, 255, Color::Solid };
//					const Color texel = (tex != nullptr) ? tex->GetColor(Dither2x2(Vector2{ a.u * L0 + b.u * L1 + c.u * L2, a.v * L0 + b.v * L1 + c.v * L2}, q)) : Color{ 255, 255, 255, Color::Solid }; // Dithered texture filtering (can look good if texture is relatively high resolution)

					switch (texel.blend)
					{
					case Color::Solid:
						dst.SetColor(q, Dither2x2(texel * col, q));
						if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
						break;
					case Color::AddAlpha:
						dst.SetColor(q, Dither2x2(pixel + texel * col, q));
						break;
					case Color::Emissive:
						dst.SetColor(q, texel);
						if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
						break;
					case Color::EmissiveAddAlpha:
						dst.SetColor(q, Dither2x2(pixel + texel, q));
						break;
					default: break;
					}
				}
			}

			w0 += w0_x_inc;
			w1 += w1_x_inc;
			w2 += w2_x_inc;

			l0 += l0_x_inc;
			l1 += l1_x_inc;
			l2 += l2_x_inc;
		}

		w0_y += w0_y_inc;
		w1_y += w1_y_inc;
		w2_y += w2_y_inc;

		l0_y += l0_y_inc;
		l1_y += l1_y_inc;
		l2_y += l2_y_inc;
	}
}
#include <iostream>
void internal_impl::DrawTriangle_Fast(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::IVertex &a, const internal_impl::IVertex &b, const internal_impl::IVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	static constexpr SInt SIMD_X_TILE      = TINY_WIDTH;
	static constexpr SInt SIMD_Y_TILE      = 1;
	static constexpr SInt X_COORD_OFFSET[] = TINY_OFFSETS;
	static constexpr SInt Y_COORD_OFFSET[] = TINY_NO_OFFSETS;

	// AABB Clipping
	SInt min_y = tiny3d::Max(tiny3d::Min(a.p.y, b.p.y, c.p.y), SInt(0));
	SInt max_y = tiny3d::Min(tiny3d::Max(a.p.y, b.p.y, c.p.y), SInt(dst.GetHeight() - 1));
	if (max_y - min_y <= 0) { return; }
	SInt min_x = TINY_FLOOR(tiny3d::Max(tiny3d::Min(a.p.x, b.p.x, c.p.x), SInt(0)));
	SInt max_x = tiny3d::Min(tiny3d::Max(a.p.x, b.p.x, c.p.x), SInt(dst.GetWidth() - 1));
	if (max_x - min_x <= 0) { return; }

	if (dst_rect != nullptr) {
		min_y = SInt(tiny3d::Max(UInt(min_y), dst_rect->a.y));
		max_y = SInt(tiny3d::Min(UInt(max_y), dst_rect->b.y - 1));
		min_x = SInt(tiny3d::Max(UInt(min_x), dst_rect->a.x));
		max_x = SInt(tiny3d::Min(UInt(max_x), dst_rect->b.x - 1));
	}

	// Interpolation/triangle setup
	const WidePoint p        = { WideSInt(min_x) + WideSInt(X_COORD_OFFSET), WideSInt(min_y) + WideSInt(Y_COORD_OFFSET) };
	WidePoint       q        = p;
	WideSInt        w0_y     = DetermineHalfspace_Fast(b.p, c.p, p);
	WideSInt        w1_y     = DetermineHalfspace_Fast(c.p, a.p, p);
	WideSInt        w2_y     = DetermineHalfspace_Fast(a.p, b.p, p);
	const WideSInt  w2_x_inc = (a.p.y - b.p.y) * SIMD_X_TILE;
	const WideSInt  w2_y_inc = (b.p.x - a.p.x) * SIMD_Y_TILE;
	const WideSInt  w0_x_inc = (b.p.y - c.p.y) * SIMD_X_TILE;
	const WideSInt  w0_y_inc = (c.p.x - b.p.x) * SIMD_Y_TILE;
	const WideSInt  w1_x_inc = (c.p.y - a.p.y) * SIMD_X_TILE;
	const WideSInt  w1_y_inc = (a.p.x - c.p.x) * SIMD_Y_TILE;
	WideReal        l0_y     = WideReal(w0_y);
	WideReal        l1_y     = WideReal(w1_y);
	WideReal        l2_y     = WideReal(w2_y);
	const WideReal  l0_x_inc = WideReal(w0_x_inc);
	const WideReal  l1_x_inc = WideReal(w1_x_inc);
	const WideReal  l2_x_inc = WideReal(w2_x_inc);
	const WideReal  l0_y_inc = WideReal(w0_y_inc);
	const WideReal  l1_y_inc = WideReal(w1_y_inc);
	const WideReal  l2_y_inc = WideReal(w2_y_inc);
	const WideReal  waw      = a.w;
	const WideReal  wbw      = b.w;
	const WideReal  wcw      = c.w;

	w0_y += IsTopLeft(b.p, c.p) ? WideSInt(0) : WideSInt(-1); // add offsets to coordinates to enforce fill convention
	w1_y += IsTopLeft(c.p, a.p) ? WideSInt(0) : WideSInt(-1);
	w2_y += IsTopLeft(a.p, b.p) ? WideSInt(0) : WideSInt(-1);

	const float *zread_offset  = zread  != nullptr ? &((*zread)[UInt(min_x) + dst.GetWidth() * UInt(min_y)])  : nullptr;
	float       *zwrite_offset = zwrite != nullptr ? &((*zwrite)[UInt(min_x) + dst.GetWidth() * UInt(min_y)]) : nullptr;

	for (int y = min_y; y <= max_y; y += SIMD_Y_TILE) {

		WideSInt w0 = w0_y;
		WideSInt w1 = w1_y;
		WideSInt w2 = w2_y;

		WideReal l0 = l0_y;
		WideReal l1 = l1_y;
		WideReal l2 = l2_y;

		const float *zr = zread_offset;
		float       *zw = zwrite_offset;

		for (int x = min_x; x <= max_x; x += SIMD_X_TILE) {

			WideBool fragment_mask = (w0 | w1 | w2) >= 0;

			if (fragment_mask.all_fail() == false) {

				const WideReal sz = WideReal(1.0f) / (waw * l0 + wbw * l1 + wcw * l2);
				const WideReal dz = (zr) ? WideReal(zr) : std::numeric_limits<float>::infinity();

				fragment_mask = fragment_mask & (sz <= dz);

				if (fragment_mask.all_fail() == false) {

					WideColor pixel;
					for (int i = 0; i < TINY_WIDTH; ++i) {
						Color o = dst.GetColor(UPoint{ UInt(reinterpret_cast<SInt*>(&q.x)[i]), UInt(reinterpret_cast<SInt*>(&q.y)[i]) });
						reinterpret_cast<SInt*>(&pixel.r)[i]     = o.r;
						reinterpret_cast<SInt*>(&pixel.g)[i]     = o.g;
						reinterpret_cast<SInt*>(&pixel.b)[i]     = o.b;
						reinterpret_cast<SInt*>(&pixel.blend)[i] = o.blend;
					}

					fragment_mask = fragment_mask & (pixel.blend != WideSInt(Color::Transparent));

					if (fragment_mask.all_fail() == false) { // use transparency bit as a 1-bit stencil

						const WideReal L0 = l0 * sz;
						const WideReal L1 = l1 * sz;
						const WideReal L2 = l2 * sz;

						WideSInt u = WideSInt(WideReal(a.u) * L0 + WideReal(b.u) * L1 + WideReal(c.u) * L2);
						WideSInt v = WideSInt(WideReal(a.v) * L0 + WideReal(b.v) * L1 + WideReal(c.v) * L2);

						const WideColor col = {
							WideSInt(WideReal(a.r) * L0 + WideReal(b.r) * L1 + WideReal(c.r) * L2),
							WideSInt(WideReal(a.g) * L0 + WideReal(b.g) * L1 + WideReal(c.g) * L2),
							WideSInt(WideReal(a.b) * L0 + WideReal(b.b) * L1 + WideReal(c.b) * L2),
							WideSInt(Color::Solid)
						};

						for (int i = 0; i < TINY_WIDTH; ++i) {

							if (reinterpret_cast<const UInt*>(&fragment_mask)[i] == 0) { continue; }

							const Color texel = (tex) ? tex->GetColor(UPoint{ UInt(reinterpret_cast<SInt*>(&u)[i]), UInt(reinterpret_cast<SInt*>(&v)[i]) }) : Color{ 255, 255, 255, Color::Solid };
							const Color cx = Color{
								Byte(reinterpret_cast<const SInt*>(&col.r)[i]),
								Byte(reinterpret_cast<const SInt*>(&col.g)[i]),
								Byte(reinterpret_cast<const SInt*>(&col.b)[i]),
								Color::Solid
							};
							const UPoint pt = UPoint{ UInt(reinterpret_cast<SInt*>(&q.x)[i]), UInt(reinterpret_cast<SInt*>(&q.y)[i]) };
							switch (texel.blend)
							{
							case Color::Solid:
								dst.SetColor(pt, Dither2x2(texel * cx, pt));
								if (zw) { zw[i] = reinterpret_cast<const float*>(&sz)[i]; }
								break;
							case Color::AddAlpha:
								dst.SetColor(pt, Dither2x2(dst.GetColor(pt) + texel * cx, pt));
								break;
							case Color::Emissive:
								dst.SetColor(pt, texel);
								if (zw) { zw[i] = reinterpret_cast<const float*>(&sz)[i]; }
								break;
							case Color::EmissiveAddAlpha:
								dst.SetColor(pt, Dither2x2(dst.GetColor(pt) + texel, pt));
								break;
							default: break;
							}
						}
					}
				}
			}

			w0 += w0_x_inc;
			w1 += w1_x_inc;
			w2 += w2_x_inc;

			l0 += l0_x_inc;
			l1 += l1_x_inc;
			l2 += l2_x_inc;

			q.x += SIMD_X_TILE;

			if (zr) { zr += SIMD_X_TILE; }
			if (zw) { zw += SIMD_X_TILE; }
		}

		w0_y += w0_y_inc;
		w1_y += w1_y_inc;
		w2_y += w2_y_inc;

		l0_y += l0_y_inc;
		l1_y += l1_y_inc;
		l2_y += l2_y_inc;

		q.x = p.x;
		q.y += SIMD_Y_TILE;

		if (zread_offset)  { zread_offset  += dst.GetWidth() * SIMD_Y_TILE; }
		if (zwrite_offset) { zwrite_offset += dst.GetWidth() * SIMD_Y_TILE; }
	}
}

void tiny3d::DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Vertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	internal_impl::DrawTriangle(dst, zread, zwrite, ToI(a, tex), ToI(b, tex), ToI(c, tex), tex, dst_rect);
}

void tiny3d::DrawTriangle_Fast(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Vertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	internal_impl::DrawTriangle_Fast(dst, zread, zwrite, ToI(a, tex), ToI(b, tex), ToI(c, tex), tex, dst_rect);
}

void internal_impl::DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::ILVertex &a, const internal_impl::ILVertex &b, const internal_impl::ILVertex &c, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap, const tiny3d::URect *dst_rect)
{
	// AABB Clipping
	SInt min_y = tiny3d::Max(tiny3d::Min(a.p.y, b.p.y, c.p.y), SInt(0));
	SInt max_y = tiny3d::Min(tiny3d::Max(a.p.y, b.p.y, c.p.y), SInt(dst.GetHeight() - 1));
	if (max_y - min_y <= 0) { return; }
	SInt min_x = tiny3d::Max(tiny3d::Min(a.p.x, b.p.x, c.p.x), SInt(0));
	SInt max_x = tiny3d::Min(tiny3d::Max(a.p.x, b.p.x, c.p.x), SInt(dst.GetWidth() - 1));
	if (max_x - min_x <= 0) { return; }

	if (dst_rect != nullptr) {
		min_y = SInt(tiny3d::Max(UInt(min_y), dst_rect->a.y));
		max_y = SInt(tiny3d::Min(UInt(max_y), dst_rect->b.y - 1));
		min_x = SInt(tiny3d::Max(UInt(min_x), dst_rect->a.x));
		max_x = SInt(tiny3d::Min(UInt(max_x), dst_rect->b.x - 1));
	}

	// Triangle setup
	tiny3d::Point p    = { min_x, min_y };
	SXInt         w0_y = DetermineHalfspace(b.p, c.p, p);
	SXInt         w1_y = DetermineHalfspace(c.p, a.p, p);
	SXInt         w2_y = DetermineHalfspace(a.p, b.p, p);

//	if (ShouldDivide(w0_y + w1_y + w2_y)) {
//		DrawSubdivTri(dst, zbuf, a, b, c, tex, dst_rect);
//		return;
//	}

	// Interpolation/triangle setup
	const SInt w2_x_inc        = a.p.y - b.p.y;
	const SInt w2_y_inc        = b.p.x - a.p.x;
	const SInt w0_x_inc        = b.p.y - c.p.y;
	const SInt w0_y_inc        = c.p.x - b.p.x;
	const SInt w1_x_inc        = c.p.y - a.p.y;
	const SInt w1_y_inc        = a.p.x - c.p.x;
	const float sum_inv_area_x2 = 1.0f / SInt(w0_y + w1_y + w2_y);
	float l0_y           = SInt(w0_y) * sum_inv_area_x2;
	float l1_y           = SInt(w1_y) * sum_inv_area_x2;
	float l2_y           = SInt(w2_y) * sum_inv_area_x2;
	const float l0_x_inc = w0_x_inc * sum_inv_area_x2;
	const float l1_x_inc = w1_x_inc * sum_inv_area_x2;
	const float l2_x_inc = w2_x_inc * sum_inv_area_x2;
	const float l0_y_inc = w0_y_inc * sum_inv_area_x2;
	const float l1_y_inc = w1_y_inc * sum_inv_area_x2;
	const float l2_y_inc = w2_y_inc * sum_inv_area_x2;

	w0_y += IsTopLeft(b.p, c.p) ? 0 : -1; // add offsets to coordinates to enforce fill convention
	w1_y += IsTopLeft(c.p, a.p) ? 0 : -1;
	w2_y += IsTopLeft(a.p, b.p) ? 0 : -1;

	for (p.y = min_y; p.y <= max_y; ++p.y) {

		SInt w0 = SInt(w0_y);
		SInt w1 = SInt(w1_y);
		SInt w2 = SInt(w2_y);

		float l0 = l0_y;
		float l1 = l1_y;
		float l2 = l2_y;

		for (p.x = min_x; p.x <= max_x; ++p.x) {

			if ((w0 | w1 | w2) >= 0) {

				const UPoint q     = { UInt(p.x), UInt(p.y) };
				const Color  pixel = dst.GetColor(q);
				const UInt   zi    = q.x + q.y * dst.GetWidth();
				const float  sz    = 1.0f / (a.w * l0 + b.w * l1 + c.w * l2);
				const float  dz    = (zread != nullptr) ? (*zread)[zi] : std::numeric_limits<float>::infinity();

				if (sz <= dz && pixel.blend != Color::Transparent) { // use transparency bit as a 1-bit stencil

					const float L0 = l0 * sz;
					const float L1 = l1 * sz;
					const float L2 = l2 * sz;

					const Color   texel = (tex != nullptr) ? tex->GetColor(UPoint{ UInt(a.u * L0 + b.u * L1 + c.u * L2), UInt(a.v * L0 + b.v * L1 + c.v * L2) }) : Color{ 255, 255, 255, Color::Solid };
					const Vector2 luv   = Vector2{ a.lu * L0 + b.lu * L1 + c.lu * L2, a.lv * L0 + b.lv * L1 + c.lv * L2 };
					const UPoint  l00   = UPoint{ UInt(luv.x),   UInt(luv.y) };
					const Color   lumel = Bilerp(
						lightmap.GetColor(l00),                      lightmap.GetColor(UPoint{ l00.x+1, l00.y }),
						lightmap.GetColor(UPoint{ l00.x, l00.y+1 }), lightmap.GetColor(UPoint{ l00.x+1, l00.y+1 }),
						luv.x - l00.x,
						luv.y - l00.y
					);
//					const Color   lumel = lightmap.GetColor(Dither2x2(Vector2{ a.lu * L0 + b.lu * L1 + c.lu * L2, a.lv * L0 + b.lv * L1 + c.lv * L2 }, q)); // Dithered lightmap (looks terrible)

					switch (texel.blend)
					{
					case Color::Solid:
						dst.SetColor(q, Dither2x2(texel * lumel, q));
						if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
						break;
					case Color::AddAlpha:
						dst.SetColor(q, Dither2x2(pixel + texel * lumel, q));
						break;
					case Color::Emissive:
						dst.SetColor(q, texel);
						if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
						break;
					case Color::EmissiveAddAlpha:
						dst.SetColor(q, Dither2x2(pixel + texel, q));
						break;
					default: break;
					}
				}
			}

			w0 += w0_x_inc;
			w1 += w1_x_inc;
			w2 += w2_x_inc;

			l0 += l0_x_inc;
			l1 += l1_x_inc;
			l2 += l2_x_inc;
		}

		w0_y += w0_y_inc;
		w1_y += w1_y_inc;
		w2_y += w2_y_inc;

		l0_y += l0_y_inc;
		l1_y += l1_y_inc;
		l2_y += l2_y_inc;
	}
}

void tiny3d::DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::LVertex &a, const tiny3d::LVertex &b, const tiny3d::LVertex &c, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap, const tiny3d::URect *dst_rect)
{
	internal_impl::DrawTriangle(dst, zread, zwrite, ToI(a, tex, lightmap), ToI(b, tex, lightmap), ToI(c, tex, lightmap), tex, lightmap, dst_rect);
}

void internal_impl::DrawTriangle_Fast(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::ILVertex &a, const internal_impl::ILVertex &b, const internal_impl::ILVertex &c, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap, const tiny3d::URect *dst_rect)
{
	static constexpr SInt SIMD_X_TILE      = TINY_WIDTH;
	static constexpr SInt SIMD_Y_TILE      = 1;
	static constexpr SInt X_COORD_OFFSET[] = TINY_OFFSETS;
	static constexpr SInt Y_COORD_OFFSET[] = TINY_NO_OFFSETS;

	// AABB Clipping
	SInt min_y = tiny3d::Max(tiny3d::Min(a.p.y, b.p.y, c.p.y), SInt(0));
	SInt max_y = tiny3d::Min(tiny3d::Max(a.p.y, b.p.y, c.p.y), SInt(dst.GetHeight() - 1));
	if (max_y - min_y <= 0) { return; }
	SInt min_x = TINY_FLOOR(tiny3d::Max(tiny3d::Min(a.p.x, b.p.x, c.p.x), SInt(0)));
	SInt max_x = tiny3d::Min(tiny3d::Max(a.p.x, b.p.x, c.p.x), SInt(dst.GetWidth() - 1));
	if (max_x - min_x <= 0) { return; }

	if (dst_rect != nullptr) {
		min_y = SInt(tiny3d::Max(UInt(min_y), dst_rect->a.y));
		max_y = SInt(tiny3d::Min(UInt(max_y), dst_rect->b.y - 1));
		min_x = SInt(tiny3d::Max(UInt(min_x), dst_rect->a.x));
		max_x = SInt(tiny3d::Min(UInt(max_x), dst_rect->b.x - 1));
	}

	// Interpolation/triangle setup
	const WidePoint p        = { WideSInt(min_x) + WideSInt(X_COORD_OFFSET), WideSInt(min_y) + WideSInt(Y_COORD_OFFSET) };
	WidePoint       q        = p;
	WideSInt        w0_y     = DetermineHalfspace_Fast(b.p, c.p, p);
	WideSInt        w1_y     = DetermineHalfspace_Fast(c.p, a.p, p);
	WideSInt        w2_y     = DetermineHalfspace_Fast(a.p, b.p, p);
	const WideSInt  w2_x_inc = (a.p.y - b.p.y) * SIMD_X_TILE;
	const WideSInt  w2_y_inc = (b.p.x - a.p.x) * SIMD_Y_TILE;
	const WideSInt  w0_x_inc = (b.p.y - c.p.y) * SIMD_X_TILE;
	const WideSInt  w0_y_inc = (c.p.x - b.p.x) * SIMD_Y_TILE;
	const WideSInt  w1_x_inc = (c.p.y - a.p.y) * SIMD_X_TILE;
	const WideSInt  w1_y_inc = (a.p.x - c.p.x) * SIMD_Y_TILE;
	WideReal        l0_y     = WideReal(w0_y);
	WideReal        l1_y     = WideReal(w1_y);
	WideReal        l2_y     = WideReal(w2_y);
	const WideReal  l0_x_inc = WideReal(w0_x_inc);
	const WideReal  l1_x_inc = WideReal(w1_x_inc);
	const WideReal  l2_x_inc = WideReal(w2_x_inc);
	const WideReal  l0_y_inc = WideReal(w0_y_inc);
	const WideReal  l1_y_inc = WideReal(w1_y_inc);
	const WideReal  l2_y_inc = WideReal(w2_y_inc);
	const WideReal  waw      = a.w;
	const WideReal  wbw      = b.w;
	const WideReal  wcw      = c.w;

	w0_y += IsTopLeft(b.p, c.p) ? WideSInt(0) : WideSInt(-1); // add offsets to coordinates to enforce fill convention
	w1_y += IsTopLeft(c.p, a.p) ? WideSInt(0) : WideSInt(-1);
	w2_y += IsTopLeft(a.p, b.p) ? WideSInt(0) : WideSInt(-1);

	const float *zread_offset  = zread  != nullptr ? &((*zread)[UInt(min_x) + dst.GetWidth() * UInt(min_y)])  : nullptr;
	float       *zwrite_offset = zwrite != nullptr ? &((*zwrite)[UInt(min_x) + dst.GetWidth() * UInt(min_y)]) : nullptr;

	for (int y = min_y; y <= max_y; y += SIMD_Y_TILE) {

		WideSInt w0 = w0_y;
		WideSInt w1 = w1_y;
		WideSInt w2 = w2_y;

		WideReal l0 = l0_y;
		WideReal l1 = l1_y;
		WideReal l2 = l2_y;

		const float *zr = zread_offset;
		float       *zw = zwrite_offset;

		for (int x = min_x; x <= max_x; x += SIMD_X_TILE) {

			WideBool fragment_mask = (w0 | w1 | w2) >= 0;

			if (fragment_mask.all_fail() == false) {

				const WideReal sz = WideReal(1.0f) / (waw * l0 + wbw * l1 + wcw * l2);
				const WideReal dz = (zr) ? WideReal(zr) : std::numeric_limits<float>::infinity();

				fragment_mask = fragment_mask & (sz <= dz);

				if (fragment_mask.all_fail() == false) {

					WideColor pixel;
					for (int i = 0; i < TINY_WIDTH; ++i) {
						Color o = dst.GetColor(UPoint{ UInt(reinterpret_cast<SInt*>(&q.x)[i]), UInt(reinterpret_cast<SInt*>(&q.y)[i]) });
						reinterpret_cast<SInt*>(&pixel.r)[i]     = o.r;
						reinterpret_cast<SInt*>(&pixel.g)[i]     = o.g;
						reinterpret_cast<SInt*>(&pixel.b)[i]     = o.b;
						reinterpret_cast<SInt*>(&pixel.blend)[i] = o.blend;
					}

					fragment_mask = fragment_mask & (pixel.blend != WideSInt(Color::Transparent));

					if (fragment_mask.all_fail() == false) { // use transparency bit as a 1-bit stencil

						const WideReal L0 = l0 * sz;
						const WideReal L1 = l1 * sz;
						const WideReal L2 = l2 * sz;

						const WideSInt u = WideSInt(WideReal(a.u) * L0 + WideReal(b.u) * L1 + WideReal(c.u) * L2);
						const WideSInt v = WideSInt(WideReal(a.v) * L0 + WideReal(b.v) * L1 + WideReal(c.v) * L2);

						const WideReal lu = WideReal(a.lu) * L0 + WideReal(b.lu) * L1 + WideReal(c.lu) * L2;
						const WideReal lv = WideReal(a.lv) * L0 + WideReal(b.lv) * L1 + WideReal(c.lv) * L2;

						const WidePoint l00 = { WideSInt(lu),     WideSInt(lv)     };
						const WidePoint l10 = { WideSInt(lu) + 1, WideSInt(lv)     };
						const WidePoint l01 = { WideSInt(lu),     WideSInt(lv) + 1 };
						const WidePoint l11 = { WideSInt(lu) + 1, WideSInt(lv) + 1 };
						WideColor c00, c10, c01, c11;
						for (SInt i = 0; i < TINY_WIDTH; ++i) {
							const Color lc00 = lightmap.GetColor(UPoint{ UInt(reinterpret_cast<const SInt*>(&l00.x)[i]), UInt(reinterpret_cast<const SInt*>(&l00.y)[i]) });
							const Color lc10 = lightmap.GetColor(UPoint{ UInt(reinterpret_cast<const SInt*>(&l10.x)[i]), UInt(reinterpret_cast<const SInt*>(&l10.y)[i]) });
							const Color lc01 = lightmap.GetColor(UPoint{ UInt(reinterpret_cast<const SInt*>(&l01.x)[i]), UInt(reinterpret_cast<const SInt*>(&l01.y)[i]) });
							const Color lc11 = lightmap.GetColor(UPoint{ UInt(reinterpret_cast<const SInt*>(&l11.x)[i]), UInt(reinterpret_cast<const SInt*>(&l11.y)[i]) });

							reinterpret_cast<SInt*>(&c00.r)[i]     = lc00.r;
							reinterpret_cast<SInt*>(&c00.g)[i]     = lc00.g;
							reinterpret_cast<SInt*>(&c00.b)[i]     = lc00.b;
							reinterpret_cast<SInt*>(&c00.blend)[i] = lc00.blend;

							reinterpret_cast<SInt*>(&c10.r)[i]     = lc10.r;
							reinterpret_cast<SInt*>(&c10.g)[i]     = lc10.g;
							reinterpret_cast<SInt*>(&c10.b)[i]     = lc10.b;
							reinterpret_cast<SInt*>(&c10.blend)[i] = lc10.blend;

							reinterpret_cast<SInt*>(&c01.r)[i]     = lc01.r;
							reinterpret_cast<SInt*>(&c01.g)[i]     = lc01.g;
							reinterpret_cast<SInt*>(&c01.b)[i]     = lc01.b;
							reinterpret_cast<SInt*>(&c01.blend)[i] = lc01.blend;

							reinterpret_cast<SInt*>(&c11.r)[i]     = lc11.r;
							reinterpret_cast<SInt*>(&c11.g)[i]     = lc11.g;
							reinterpret_cast<SInt*>(&c11.b)[i]     = lc11.b;
							reinterpret_cast<SInt*>(&c11.blend)[i] = lc11.blend;
						}

						const WideColor wlumel = Bilerp(
							c00, c10,
							c01, c11,
							lu - WideReal(l00.x),
							lv - WideReal(l00.y)
						);

						for (SInt i = 0; i < TINY_WIDTH; ++i) {

							if (reinterpret_cast<const UInt*>(&fragment_mask)[i] == 0) { continue; }

							const Color texel = (tex) ? tex->GetColor(UPoint{ UInt(reinterpret_cast<const SInt*>(&u)[i]), UInt(reinterpret_cast<const SInt*>(&v)[i]) }) : Color{ 255, 255, 255, Color::Solid };
							const Color lumel = Color{
								Byte(reinterpret_cast<const SInt*>(&wlumel.r)[i]),
								Byte(reinterpret_cast<const SInt*>(&wlumel.g)[i]),
								Byte(reinterpret_cast<const SInt*>(&wlumel.b)[i]),
								Byte(reinterpret_cast<const SInt*>(&wlumel.blend)[i])
							};

							const UPoint pt = UPoint{ UInt(reinterpret_cast<SInt*>(&q.x)[i]), UInt(reinterpret_cast<SInt*>(&q.y)[i]) };
							switch (texel.blend)
							{
							case Color::Solid:
								dst.SetColor(pt, Dither2x2(texel * lumel, pt));
								if (zw) { zw[i] = reinterpret_cast<const float*>(&sz)[i]; }
								break;
							case Color::AddAlpha:
								dst.SetColor(pt, Dither2x2(dst.GetColor(pt) + texel * lumel, pt));
								break;
							case Color::Emissive:
								dst.SetColor(pt, texel);
								if (zw) { zw[i] = reinterpret_cast<const float*>(&sz)[i]; }
								break;
							case Color::EmissiveAddAlpha:
								dst.SetColor(pt, Dither2x2(dst.GetColor(pt) + texel, pt));
								break;
							default: break;
							}
						}
					}
				}
			}

			w0 += w0_x_inc;
			w1 += w1_x_inc;
			w2 += w2_x_inc;

			l0 += l0_x_inc;
			l1 += l1_x_inc;
			l2 += l2_x_inc;

			q.x += SIMD_X_TILE;

			if (zr) { zr += SIMD_X_TILE; }
			if (zw) { zw += SIMD_X_TILE; }
		}

		w0_y += w0_y_inc;
		w1_y += w1_y_inc;
		w2_y += w2_y_inc;

		l0_y += l0_y_inc;
		l1_y += l1_y_inc;
		l2_y += l2_y_inc;

		q.x = p.x;
		q.y += SIMD_Y_TILE;

		if (zread_offset)  { zread_offset  += dst.GetWidth() * SIMD_Y_TILE; }
		if (zwrite_offset) { zwrite_offset += dst.GetWidth() * SIMD_Y_TILE; }
	}
}

void tiny3d::DrawTriangle_Fast(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::LVertex &a, const tiny3d::LVertex &b, const tiny3d::LVertex &c, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap, const tiny3d::URect *dst_rect)
{
	internal_impl::DrawTriangle_Fast(dst, zread, zwrite, ToI(a, tex, lightmap), ToI(b, tex, lightmap), ToI(c, tex, lightmap), tex, lightmap, dst_rect);
}

template < typename src_t >
void internal_impl::DrawRegion(tiny3d::Image &dst, tiny3d::Rect dst_region, const src_t &src, tiny3d::Rect src_region, const tiny3d::URect *dst_rect)
{
	// TODO: u and v do not have to be unit scaled, which saves two multiplications and two int->float conversions per pixel.
	// TODO: Change to fixed point rendering.
	tiny3d::URect DstRect = (dst_rect != nullptr) ? *dst_rect : tiny3d::URect{ tiny3d::UPoint{ 0,0 }, tiny3d::UPoint{ dst.GetWidth(), dst.GetHeight() } };

	// NOTE: Clip src_region against max borders
	src_region.a.x = 0 > src_region.a.x ? 0 : src_region.a.x;
	src_region.a.y = 0 > src_region.a.y ? 0 : src_region.a.y;
	src_region.b.x = SInt(src.GetWidth()) < src_region.b.x ? src.GetWidth() : src_region.b.x;
	src_region.b.y = SInt(src.GetHeight()) < src_region.b.y ? src.GetHeight() : src_region.b.y;

	Real u1 = Real(src_region.a.x) / Real(src.GetWidth());
	Real v1 = Real(src_region.a.y) / Real(src.GetHeight());
	Real u2 = Real(src_region.b.x) / Real(src.GetWidth());
	Real v2 = Real(src_region.b.y) / Real(src.GetHeight());
	Real du = (u2 - u1) / (Real)(dst_region.b.x - dst_region.a.x + 1); // NOTE: Iterate over one more than the actual destination region to render the last pixel in the source region without accessing source out of bounds.
	Real dv = (v2 - v1) / (Real)(dst_region.b.y - dst_region.a.y + 1);

	// NOTE: Enable a negative writable area on dst (flips blit direction).
	if (dst_region.b.x < dst_region.a.x) {
		tiny3d::Swap(dst_region.a.x, dst_region.b.x);
		tiny3d::Swap(u1, v1);
		if (dst_region.a.x < 0) { // NOTE: Make read offset for src + clip against min borders.
			u1 = u1 - du * dst_region.a.x;
			dst_region.a.x = 0;
		}
	} else if (dst_region.a.x < 0) { // NOTE: Make read offset for src + clip against min borders.
		u1 = u1 + du * -dst_region.a.x;
		dst_region.a.x = 0;
	}
	if (dst_region.b.y < dst_region.a.y) {
		tiny3d::Swap(dst_region.a.y, dst_region.b.y);
		tiny3d::Swap(v1, v2);
		if (dst_region.a.y < 0) { // NOTE: Make read offset for src + clip against min borders.
			v1 = v1 - dv * dst_region.a.y;
			dst_region.a.y = 0;
		}
	} else if (dst_region.a.y < 0) { // NOTE: Make read offset for src + clip against min borders.
		v1 = v1 + dv * -dst_region.a.y;
		dst_region.a.y = 0;
	}

	// NOTE: Clip dst_region agains max borders.
	dst_region.b.x = SInt(dst.GetWidth()) < dst_region.b.x ? dst.GetWidth() : dst_region.b.x;
	dst_region.b.y = SInt(dst.GetHeight()) < dst_region.b.y ? dst.GetHeight() : dst_region.b.y;

	DstRect.a = tiny3d::UPoint{
		tiny3d::Max(DstRect.a.x, UInt(dst_region.a.x)),
		tiny3d::Max(DstRect.a.y, UInt(dst_region.a.y))
	};
	DstRect.b = tiny3d::UPoint{
		tiny3d::Min(DstRect.b.x, UInt(dst_region.b.x)),
		tiny3d::Min(DstRect.b.y, UInt(dst_region.b.y))
	};

	// NOTE: Determine writable area.
	const SInt MAXY = DstRect.b.y - DstRect.a.y;
	const SInt MAXX = DstRect.b.x - DstRect.a.x;
	if (MAXX < 0 || MAXY < 0) { return; } // readable area is negative (probably because src is offscreen)

	u1 += du * (DstRect.a.x - dst_region.a.x);
	v1 += dv * (DstRect.a.y - dst_region.a.y);

	// NOTE: Draw scaled scanlines.
	Real v = v1;
	for (UInt y = 0; y < UInt(MAXY); ++y){
		Real u = u1;
		for (UInt x = 0; x < UInt(MAXX); ++x){
			const Color c = src.GetColor(UPoint{UInt(u * src.GetWidth()), UInt(v * src.GetHeight())});
			const UPoint p = { x, y };
			if ((c.blend & dst.GetStencil(p)) > 0) {
				dst.SetColor(UPoint{x,y}, c);
			}
			u += du;
		}
		v += dv;
	}
}

void tiny3d::DrawRegion(Image &dst, Rect dst_region, const Overlay &src, Rect src_region, URect *dst_rect)
{
	internal_impl::DrawRegion(dst, dst_region, src, src_region, dst_rect);
}

#define font_char_px_width  TINY3D_CHAR_WIDTH
#define font_char_px_height TINY3D_CHAR_HEIGHT
#define font_char_first     TINY3D_CHAR_FIRST
#define font_char_last      TINY3D_CHAR_LAST

#define font_char_count_width  4
#define font_char_count_height 24

// XBM data format
#define font_width  24
#define font_height 144
static constexpr unsigned char font_bits[] = {
	0xff, 0xff, 0xff, 0x7d, 0xbd, 0x06, 0x7d, 0x1d, 0xa4, 0xfd, 0xbf, 0x06,
	0xff, 0x1f, 0x2c, 0xfd, 0xbf, 0x06, 0xff, 0xff, 0xff, 0x77, 0xdc, 0xef,
	0xff, 0xde, 0xf7, 0x41, 0xf5, 0xf7, 0x7f, 0xfb, 0xf7, 0x77, 0xf4, 0xef,
	0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xfb, 0xbe, 0xff, 0x7b, 0x1d, 0xff,
	0xfb, 0xbe, 0xef, 0xfd, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x05,
	0xff, 0xff, 0x76, 0xf1, 0x7f, 0x57, 0xff, 0xbf, 0x77, 0x7f, 0xdf, 0x07,
	0xff, 0xff, 0xff, 0x77, 0x10, 0x74, 0xf3, 0xf7, 0x75, 0x77, 0x30, 0x04,
	0x77, 0xff, 0x7d, 0x41, 0x10, 0x7c, 0xff, 0xff, 0xff, 0x41, 0x10, 0x04,
	0x7d, 0xff, 0x75, 0x41, 0xf0, 0x04, 0x5f, 0xf7, 0x75, 0x41, 0xf0, 0x05,
	0xff, 0xff, 0xff, 0xc1, 0xff, 0xdf, 0x5d, 0xbf, 0xef, 0xc1, 0xff, 0xf7,
	0x5f, 0xbf, 0xef, 0xdf, 0xdf, 0xdf, 0xff, 0xff, 0xff, 0x7f, 0x1f, 0x04,
	0xf1, 0xfe, 0x75, 0xff, 0x1d, 0x34, 0xf1, 0xfe, 0x57, 0x7f, 0xdf, 0x17,
	0xff, 0xff, 0xff, 0x41, 0x38, 0x84, 0x5d, 0xdb, 0x77, 0x41, 0xd0, 0x77,
	0x5d, 0xd7, 0x77, 0x5d, 0x10, 0x04, 0xff, 0xff, 0xff, 0x41, 0x10, 0x74,
	0x7d, 0xdf, 0x77, 0x61, 0x58, 0x04, 0x7d, 0xdf, 0x75, 0x41, 0x1f, 0x74,
	0xff, 0xff, 0xff, 0x41, 0xd0, 0xf5, 0xf7, 0xd7, 0xf5, 0xf7, 0x17, 0xf6,
	0x77, 0xd7, 0xf5, 0x41, 0xd0, 0x05, 0xff, 0xff, 0xff, 0x49, 0x17, 0x04,
	0x55, 0xd6, 0x75, 0x5d, 0xd5, 0x05, 0x5d, 0xd3, 0xf5, 0x5d, 0x17, 0xf4,
	0xff, 0xff, 0xff, 0x41, 0x30, 0x04, 0x5d, 0xd7, 0xdf, 0x5d, 0x10, 0xdc,
	0x4d, 0xfb, 0xdd, 0x41, 0x17, 0xde, 0xff, 0xff, 0xff, 0x5d, 0xd7, 0x75,
	0x5d, 0xd7, 0x75, 0x5d, 0xd7, 0x8d, 0xdd, 0x5a, 0x75, 0xc1, 0x9d, 0x74,
	0xff, 0xff, 0xff, 0x5d, 0x90, 0xf7, 0xdd, 0xd7, 0xef, 0xeb, 0xd8, 0xdf,
	0x77, 0xdf, 0xbf, 0x77, 0x90, 0x7f, 0xff, 0xff, 0xff, 0xf9, 0xfe, 0xf7,
	0x7b, 0xfd, 0xef, 0xfb, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xf9, 0x1f, 0xfc,
	0xff, 0xff, 0xff, 0x41, 0x38, 0x84, 0x5d, 0xdb, 0x77, 0x41, 0xd0, 0x77,
	0x5d, 0xd7, 0x77, 0x5d, 0x10, 0x04, 0xff, 0xff, 0xff, 0x41, 0x10, 0x74,
	0x7d, 0xdf, 0x77, 0x61, 0x58, 0x04, 0x7d, 0xdf, 0x75, 0x41, 0x1f, 0x74,
	0xff, 0xff, 0xff, 0x41, 0xd0, 0xf5, 0xf7, 0xd7, 0xf5, 0xf7, 0x17, 0xf6,
	0x77, 0xd7, 0xf5, 0x41, 0xd0, 0x05, 0xff, 0xff, 0xff, 0x49, 0x17, 0x04,
	0x55, 0xd6, 0x75, 0x5d, 0xd5, 0x05, 0x5d, 0xd3, 0xf5, 0x5d, 0x17, 0xf4,
	0xff, 0xff, 0xff, 0x41, 0x30, 0x04, 0x5d, 0xd7, 0xdf, 0x5d, 0x10, 0xdc,
	0x4d, 0xfb, 0xdd, 0x41, 0x17, 0xde, 0xff, 0xff, 0xff, 0x5d, 0xd7, 0x75,
	0x5d, 0xd7, 0x75, 0x5d, 0xd7, 0x8d, 0xdd, 0x5a, 0x75, 0xc1, 0x9d, 0x74,
	0xff, 0xff, 0xff, 0x5d, 0x30, 0xf7, 0xdd, 0xb7, 0xf7, 0xeb, 0xd8, 0xf7,
	0x77, 0xbf, 0xf7, 0x77, 0x30, 0xf7, 0xff, 0xff, 0xff, 0xf9, 0x1a, 0x04,
	0x7b, 0x1d, 0x04, 0xf7, 0x1f, 0x04, 0xfb, 0x1f, 0x04, 0xf9, 0x1f, 0x04
};

tiny3d::Byte ExtractStencilBit(const tiny3d::Byte *stencil_bits, tiny3d::UInt num_bits_width, tiny3d::UInt x, tiny3d::UInt y)
{
	tiny3d::UInt i = x + y * num_bits_width;
	tiny3d::Byte bit = stencil_bits[i >> 3] & (1 << (i & 7));
	return bit != 0 ? 0x0 : 0xff;
}

tiny3d::Point internal_impl::DrawChars(tiny3d::Image &dst, tiny3d::Point p, const char *ch, tiny3d::UInt ch_num, tiny3d::Color color, tiny3d::UInt scale, const tiny3d::URect *dst_rect)
{
	const SInt scaled_font_width = font_char_px_width * SInt(scale);
	const Point out_p = { p.x + scaled_font_width * SInt(ch_num), p.y  };
	if (scale == 0 || ch_num == 0) { return out_p; }
	URect rect = { { 0, 0 }, { dst.GetWidth(), dst.GetHeight() } };
	if (dst_rect != nullptr) {
		rect.a.x = Max(rect.a.x, dst_rect->a.x);
		rect.a.y = Max(rect.a.y, dst_rect->a.y);
		rect.b.x = Min(rect.b.x, dst_rect->b.x);
		rect.b.y = Min(rect.b.y, dst_rect->b.y);
	}
	if (p.x >= SInt(rect.b.x) || p.y >= SInt(rect.b.y)) { return out_p; }
	const SInt scaled_font_height = font_char_px_height * SInt(scale);
	Point a = p;
	Point b = Point{ a.x + scaled_font_width * SInt(ch_num), a.y + scaled_font_height };
	if (b.x < SInt(rect.a.x) || b.y < SInt(rect.a.y)) { return out_p; }

	UPoint A = { Max(UInt(a.x), rect.a.x), Max(UInt(a.y), rect.a.y) };
	UPoint B = { Min(UInt(b.x), rect.b.x), Min(UInt(b.y), rect.b.y) };

	for (UInt y = A.y; y < B.y; ++y) {
		for (UInt x = A.x; x < B.x; ++x) {
			UInt ci = UInt((SInt(x) - a.x) / scaled_font_width);
			TINY3D_ASSERT(ci < ch_num);
			char c = ch[ci];
			if (c == ' ' || c == '\t') {
				x += UInt(scaled_font_width - 1);
				continue;
			}
			if (c < font_char_first || c > font_char_last) { c = font_char_last + 1; }
			UInt fi = UInt(c) - font_char_first;
			UInt fx = (fi % font_char_count_width) * font_char_px_width + (UInt((SInt(x) - a.x) % scaled_font_width) / scale);
			UInt fy = (fi / font_char_count_width) * font_char_px_height + (UInt(SInt(y) - a.y) / scale);

			if (ExtractStencilBit(font_bits, font_width, fx, fy) != 0) {
				UPoint q = { x, y };
				Color pixel = dst.GetColor(q);
				color.blend = pixel.blend;
				dst.SetColor(q, Dither2x2(color, q));
			}
		}
	}

	return out_p;
}

tiny3d::Point tiny3d::DrawChars(tiny3d::Image &dst, tiny3d::Point p, tiny3d::SInt x_margin, const char *ch, tiny3d::UInt ch_num, tiny3d::Color color, tiny3d::UInt scale, const tiny3d::URect *dst_rect)
{
	if (scale <= 0) { return p; }
	if (ch_num > 0) {
		UInt start = 0;
		UInt end = 0;
		const SInt scaled_font_height = SInt(scale) * font_char_px_height;
		while (end < ch_num) {
			if (ch[end] == '\n' || ch[end] == '\r') {
				internal_impl::DrawChars(dst, p, ch + start, (end - start), color, scale, dst_rect);
				p.x = x_margin;
				p.y += scaled_font_height;
				start = end + 1;
			}
			++end;
		}
		p = internal_impl::DrawChars(dst, p, ch + start, (end - start), color, scale, dst_rect);
	}
	return p;
}

void tiny3d::DrawRegion(tiny3d::Image &dst, tiny3d::Rect dst_region, const tiny3d::Image &src, tiny3d::Rect src_region, tiny3d::URect *dst_rect)
{
	internal_impl::DrawRegion(dst, dst_region, src, src_region, dst_rect);
}
