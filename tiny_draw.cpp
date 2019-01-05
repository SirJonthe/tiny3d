#include "tiny_draw.h"
#include "tiny_math.h"

using namespace tiny3d;

void tiny3d::DrawPoint(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::WVertex &a, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	const URect srect = URect{ { 0, 0 }, { UInt(dst.GetWidth()), UInt(dst.GetHeight()) } };
	const URect rect = (dst_rect != nullptr) ? tiny3d::Clip(*dst_rect, srect) : srect;
	if (a.p.x >= SInt(rect.a.x) && a.p.y >= SInt(rect.a.y) && a.p.x < SInt(rect.b.x) && a.p.y < SInt(rect.b.y)) {

		const UPoint q     = { UInt(a.p.x), UInt(a.p.y) };
		const Color  pixel = dst.GetColor(q);
		const UInt   zi    = q.x + q.y * dst.GetWidth();
		const Real   sz    = 1 / a.w;
		const Real   dz    = (zbuf != nullptr) ? (*zbuf)[zi] : Real::Inf();

		if (sz <= dz && pixel.blend != Color::BlendMode_Transparent) {
			const Color col = {
				Byte(SInt(a.c.x * sz)),
				Byte(SInt(a.c.y * sz)),
				Byte(SInt(a.c.z * sz)),
				Color::BlendMode_Solid
			};
			const Color texel = (tex != nullptr) ? tex->GetColor(a.t * sz) : Color{ 255, 255, 255, Color::BlendMode_Solid };

			switch (texel.blend)
			{
			case tiny3d::Color::BlendMode_Emissive:
				dst.SetColor(q, Dither2x2(texel, q));
				if (zbuf != nullptr) { (*zbuf)[zi] = sz; }
				break;
			case tiny3d::Color::BlendMode_Solid:
				dst.SetColor(q, Dither2x2(texel * col, q));
				if (zbuf != nullptr) { (*zbuf)[zi] = sz; }
				break;
			default: break;
			}
		}
	}
}

void tiny3d::DrawPoint(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::Vertex &a, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	DrawPoint(dst, zbuf, ToW(a), tex, dst_rect);
}

void tiny3d::DrawLine(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::WVertex &a, const tiny3d::WVertex &b, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
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
	const SInt dx = b.p.x - a.p.x;
	const SInt dy = b.p.y - a.p.y;
	const Vector3 dv = Vector3{ Real(dx), Real(dy), b.w - a.w };
	const Vector2 dt = b.t - a.t;
	const Vector3 dc = b.c - a.c;
	const SInt steps = Abs(dx) > Abs(dy) ? Abs(dx) : Abs(dy);
	const Vector3 v_inc = dv / steps;
	const Vector2 t_inc = dt / steps;
	const Vector3 c_inc = dc / steps;

	// Clip to dst_rect
		// Do this in constant time so we do not just skip over pixels one at a time in the inner-loop
	// DETERMINE IF LINE IS TOO LONG - RECURSIVELY CALL DrawLine with midpoints

	Vector3 v = Vector3{ Real(a.p.x), Real(a.p.y), a.w };
	Vector2 t = a.t;
	Vector3 c = a.c;
	for (SInt i = 0; i <= steps; i++) {
		Point p = { SInt(v.x),SInt(v.y) };
		if (p.x >= min_x && p.x <= max_x && p.y >= min_y && p.y <= max_y) {

			const UPoint q     = { UInt(p.x), UInt(p.y) };
			const Color  pixel = dst.GetColor(q);
			const UInt   zi    = q.x + q.y * dst.GetWidth();
			const Real   sz    = 1 / v.z;
			const Real   dz    = (zbuf != nullptr) ? (*zbuf)[zi] : Real::Inf();

			if (sz <= dz && pixel.blend != tiny3d::Color::BlendMode_Transparent) { // use transparency bit as a 1-bit stencil

				const Vector2 uv = t * sz;
				const Vector3 tcol = c * sz;
				const Color col = {
					Byte(SInt(tcol.x)),
					Byte(SInt(tcol.y)),
					Byte(SInt(tcol.z)),
					Color::BlendMode_Solid
				};

				const Color texel = (tex != nullptr) ? tex->GetColor(uv) : Color{ 255, 255, 255, Color::BlendMode_Solid };

				switch (texel.blend)
				{
				case tiny3d::Color::BlendMode_Emissive:
					dst.SetColor(q, Dither2x2(texel, q));
					if (zbuf != nullptr) { (*zbuf)[zi] = sz; }
					break;
				case tiny3d::Color::BlendMode_Solid:
					dst.SetColor(q, Dither2x2(texel * col, q));
					if (zbuf != nullptr) { (*zbuf)[zi] = sz; }
					break;
				default: break;
				}
			}

		}
		v += v_inc;
		t += t_inc;
		c += c_inc;
	}
}

void tiny3d::DrawLine(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	DrawLine(dst, zbuf, ToW(a), ToW(b), tex, dst_rect);
}

tiny3d::SXInt DetermineHalfspace(tiny3d::Point a, tiny3d::Point b, tiny3d::Point point)
{
	return tiny3d::SXInt(b.x - a.x) * tiny3d::SXInt(point.y - a.y) - tiny3d::SXInt(b.y - a.y) * tiny3d::SXInt(point.x - a.x);
}

bool IsTopLeft(tiny3d::Point a, tiny3d::Point b)
{
	// strictly connected to winding order
	return (a.x < b.x && b.y == a.y) || (a.y > b.y);
}

Real BLerp(Real a, Real b, Real c, Real l0, Real l1, Real l2)
{
	return a * l0 + b * l1 + c * l2;
}

bool ShouldDivide(SXInt req_prec)
{
	return req_prec > (SXInt(1) << Real::Precision());
}

WVertex MidVertex(const WVertex &a, const WVertex &b)
{
	WVertex ab;
	ab.p.x = a.p.x / 2 + b.p.x / 2;
	ab.p.y = a.p.y / 2 + b.p.y / 2;
	ab.t   = a.t   / 2 + b.t   / 2;
	ab.c   = a.c   / 2 + b.c   / 2;
	ab.w   = a.w   / 2 + b.w   / 2;
	return ab;
}

void DrawSubdivTri(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::WVertex &a, const tiny3d::WVertex &b, const tiny3d::WVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{

	WVertex ab = MidVertex(a, b);
	WVertex bc = MidVertex(b, c);
	WVertex ca = MidVertex(c, a);
	DrawTriangle(dst, zbuf, a,  ab, ca, tex, dst_rect);
	DrawTriangle(dst, zbuf, ab, b,  bc, tex, dst_rect);
	DrawTriangle(dst, zbuf, ca, bc, c,  tex, dst_rect);
	DrawTriangle(dst, zbuf, ca, ab, bc, tex, dst_rect);
}

void tiny3d::DrawTriangle(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::WVertex &a, const tiny3d::WVertex &b, const tiny3d::WVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
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

	if (ShouldDivide(w0_y + w1_y + w2_y)) {
		DrawSubdivTri(dst, zbuf, a, b, c, tex, dst_rect);
		return;
	}

	// Interpolation/triangle setup
	const SInt w2_x_inc        = a.p.y - b.p.y;
	const SInt w2_y_inc        = b.p.x - a.p.x;
	const SInt w0_x_inc        = b.p.y - c.p.y;
	const SInt w0_y_inc        = c.p.x - b.p.x;
	const SInt w1_x_inc        = c.p.y - a.p.y;
	const SInt w1_y_inc        = a.p.x - c.p.x;
	const Real sum_inv_area_x2 = Real(1) / SInt(w0_y + w1_y + w2_y);
	w0_y += IsTopLeft(b.p, c.p) ? 0 : -1; // add offsets to coordinates to enforce fill convention
	w1_y += IsTopLeft(c.p, a.p) ? 0 : -1;
	w2_y += IsTopLeft(a.p, b.p) ? 0 : -1;
	Real l0_y           = SInt(w0_y) * sum_inv_area_x2;
	Real l1_y           = SInt(w1_y) * sum_inv_area_x2;
	Real l2_y           = SInt(w2_y) * sum_inv_area_x2;
	const Real l0_x_inc = w0_x_inc * sum_inv_area_x2;
	const Real l1_x_inc = w1_x_inc * sum_inv_area_x2;
	const Real l2_x_inc = w2_x_inc * sum_inv_area_x2;
	const Real l0_y_inc = w0_y_inc * sum_inv_area_x2;
	const Real l1_y_inc = w1_y_inc * sum_inv_area_x2;
	const Real l2_y_inc = w2_y_inc * sum_inv_area_x2;

	for (p.y = min_y; p.y <= max_y; ++p.y) {

		SInt w0 = SInt(w0_y);
		SInt w1 = SInt(w1_y);
		SInt w2 = SInt(w2_y);

		Real l0 = l0_y;
		Real l1 = l1_y;
		Real l2 = l2_y;

		for (p.x = min_x; p.x <= max_x; ++p.x) {

			if ((w0 | w1 | w2) >= 0) {

				const UPoint q     = { UInt(p.x), UInt(p.y) };
				const Color  pixel = dst.GetColor(q);
				const UInt   zi    = q.x + q.y * dst.GetWidth();
				const Real   sz    = 1 / (a.w * l0 + b.w * l1 + c.w * l2);
				const Real   dz    = (zbuf != nullptr) ? (*zbuf)[zi] : Real::Inf();

				if (sz <= dz && pixel.blend != tiny3d::Color::BlendMode_Transparent) { // use transparency bit as a 1-bit stencil

					const Real L0 = l0 * sz;
					const Real L1 = l1 * sz;
					const Real L2 = l2 * sz;

					//const Vector2 uv   = (a.t * l0 + b.t * l1 + c.t * l2) * sz;
					//const Vector3 tcol = (a.c * l0 + b.c * l1 + c.c * l2) * sz;
					const Vector2 uv = (a.t * L0 + b.t * L1 + c.t * L2);
					const Vector3 tcol = (a.c * L0 + b.c * L1 + c.c * L2);
					const Color col = {
						Byte(SInt(tcol.x)),
						Byte(SInt(tcol.y)),
						Byte(SInt(tcol.z)),
						Color::BlendMode_Solid
					};

					const Color texel = (tex != nullptr) ? tex->GetColor(uv) : Color{ 255, 255, 255, Color::BlendMode_Solid };

					switch (texel.blend)
					{
					case tiny3d::Color::BlendMode_Emissive:
						dst.SetColor(q, Dither2x2(texel, q));
						if (zbuf != nullptr) { (*zbuf)[zi] = sz; }
						break;
					case tiny3d::Color::BlendMode_Solid:
						dst.SetColor(q, Dither2x2(texel * col, q));
						if (zbuf != nullptr) { (*zbuf)[zi] = sz; }
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

void tiny3d::DrawTriangle(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Vertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	DrawTriangle(dst, zbuf, ToW(a), ToW(b), ToW(c), tex, dst_rect);
}

#define font_char_px_width  TINY3D_CHAR_WIDTH
#define font_char_px_height TINY3D_CHAR_HEIGHT
#define font_char_first    '!'
#define font_char_last     '~'

#define font_char_count_width  4
#define font_char_count_height 24

// XBM data format
#define font_width  24
#define font_height 144
static const unsigned char font_bits[] = {
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

//void DrawChar(char ch, const mtlByte *stencil_bits, int font_width, int char_count_width, int char_width, int char_height, mtlByte *dst, int dst_bpp, mglByteOrder32 dst_order, int dst_w, int dst_h, int x, int y, mtlByte r, mtlByte g, mtlByte b, int scale)
void tiny3d::DrawChar(tiny3d::Image &dst, tiny3d::Point p, char ch, tiny3d::Color color, UInt scale, const tiny3d::URect *dst_rect)
{
	if (scale <= 0) { return; }
	if (ch < font_char_first || ch > font_char_last) {
		ch = font_char_last + 1;
	}
	const URect full_rect = { UPoint{ 0, 0 }, UPoint{ dst.GetWidth(), dst.GetHeight() } };
	if (dst_rect == nullptr) {
		dst_rect = &full_rect;
	}

	UInt ch_index = UInt(ch) - font_char_first;
	UInt ch_x     = (ch_index % font_char_count_width) * font_char_px_width;
	UInt ch_y     = (ch_index / font_char_count_width) * font_char_px_height;
	SInt start_i  = p.x < SInt(dst_rect->a.x) ? SInt(dst_rect->a.x) - p.x : SInt(dst_rect->a.x);
	UInt clip_x   = p.x < SInt(dst_rect->a.x) ? dst_rect->a.x : UInt(p.x);
	SInt start_j  = p.y < SInt(dst_rect->a.y) ? SInt(dst_rect->a.y) - p.y : SInt(dst_rect->a.y);
	UInt clip_y   = p.y < SInt(dst_rect->a.y) ? dst_rect->a.y : UInt(p.y);
	SInt end_i    = (p.x + font_char_px_width)  >= SInt(dst_rect->b.x) ? SInt(dst_rect->b.x) - p.x : font_char_px_width;
	SInt end_j    = (p.y + font_char_px_height) >= SInt(dst_rect->b.y) ? SInt(dst_rect->b.y) - p.y : font_char_px_height;
	SInt size_i   = end_i - start_i;
	SInt size_j   = end_j - start_j;

	if (size_i <= 0) { return; }

	for (UInt y = 0; y < UInt(size_j); ++y) {
		UInt font_bit_row = ch_y + y;
		for (UInt x = 0; x < UInt(size_i); ++x) {
			UInt font_bit_col = ch_x + x;
			UPoint q = { UInt(clip_x + x), UInt(clip_y + y) };
			if (ExtractStencilBit(font_bits, font_width, font_bit_col, font_bit_row) != 0) {
				Color pixel = dst.GetColor(q);
				color.blend = pixel.blend;
				dst.SetColor(q, Dither2x2(color, q));
			}
		}
	}
}

/*void tiny3d::Blit(tiny3d::Image &dst, tiny3d::Rect dst_rect, const tiny3d::Image &src, tiny3d::Rect src_rect)
{
	Rect srect = tiny3d::Clip(src_rect, { { 0, 0 }, { src.GetWidth(), src.GetHeight() } });
	Rect drect = tiny3d::Clip(dst_rect, { { 0, 0 }, { dst.GetWidth(), dst.GetHeight() } });
	if (((srect.b.x - srect.a.x) * (srect.b.y - srect.a.y)) == 0 || ((drect.b.x - drect.a.x) * (drect.b.y - drect.a.y)) == 0) { return; }
}*/
