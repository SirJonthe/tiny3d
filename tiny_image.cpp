#include "tiny_image.h"

using namespace tiny3d;

tiny3d::UHInt tiny3d::Image::EncodePixel(tiny3d::Color color) const
{

	// bits = M BBBBB GGGGG RRRRR
	UInt r = (((UInt(color.r) + 1) * 0x100) - 1) >> 11;
	UInt g = (((UInt(color.g) + 1) * 0x100) - 1) >> 11;
	UInt b = (((UInt(color.b) + 1) * 0x100) - 1) >> 11;
	UInt stencil = UInt(color.blend & 1) << 15;
	UHInt pixel = UHInt(stencil) | UHInt(b << 10) | UHInt(g << 5) | UHInt(r);
	return pixel;
}

tiny3d::Color tiny3d::Image::DecodePixel(tiny3d::UHInt pixel) const
{
	// bits = M BBBBB GGGGG RRRRR
	UInt r =
		(((pixel & 0x001F) + 1)        // convert channel to number with 1 bit of range and 5 bits of precision
		* 0x100                        // multiply by 1 with 1 bit of range and 8 bits of precision, resulting in a number with 13 bits of precision
		- 1)                           // subtract 1 to compress number back to 0 bits of range and 13 bits of precision
		>> 5;                          // shift away 5 bits of precision to keep a number with 0 bits of range and 8 bits of precision
	UInt g =
		((((pixel & 0x03E0) >> 5) + 1)
		* 0x100
		- 1)
		>> 5;
	UInt b =
		((((pixel & 0x7C00) >> 10) + 1)
		* 0x100
		- 1)
		>> 5;
	Color color;
	color.r = Byte(r);
	color.g = Byte(g);
	color.b = Byte(b);
	color.blend = (pixel & 0x8000) ? Color::Solid : Color::Transparent;
	return color;
}

tiny3d::Image::Image( void ) : m_pixels(nullptr), m_width(0), m_height(0)
{}

tiny3d::Image::Image(tiny3d::UInt dimension) : Image()
{
	Create(dimension);
}

tiny3d::Image::Image(tiny3d::UInt width, tiny3d::UInt height) : Image()
{
	Create(width, height);
}

tiny3d::Image::Image(const tiny3d::Image &i) : Image()
{
	Copy(i);
}

tiny3d::Image::~Image( void )
{
	delete [] m_pixels;
}

bool tiny3d::Image::Create(tiny3d::UInt width, tiny3d::UInt height)
{
	if (width > MaxDimension() || height > MaxDimension()) {
		Destroy();
		return false;
	}
	if (width * height != m_width * m_height) {
		delete [] m_pixels;
		m_pixels = new UHInt[width * height];
	}
	m_width = width;
	m_height = height;
	return true;
}

bool tiny3d::Image::Create(tiny3d::UInt dimensions)
{
	return Create(dimensions, dimensions);
}

void tiny3d::Image::Destroy( void )
{
	delete [] m_pixels;
	m_width = 0;
	m_height = 0;
}

void tiny3d::Image::Copy(const tiny3d::Image &img)
{
	if (this == &img) { return; }
	Create(img.m_width, img.m_height);
	for (UInt i = 0; i < m_width*m_height; ++i) {
		m_pixels[i] = img.m_pixels[i];
	}
}

void tiny3d::Image::Fill(tiny3d::URect rect, tiny3d::Color color)
{
	URect dst = {
		{ Max(UInt(0), rect.a.x), Max(UInt(0), rect.a.y) },
		{ Min(UInt(m_width), rect.b.x), Min(UInt(m_height), rect.b.y) }
	};
	UHInt  pixel = EncodePixel(color);
	UHInt *pixel_row = m_pixels + (dst.a.y * m_width);
	for (UInt y = dst.a.y; y < dst.b.y; ++y) {
		for (UInt x = dst.a.x; x < dst.b.x; ++x) {
			pixel_row[x] = pixel;
		}
		pixel_row += m_width;
	}
}

void tiny3d::Image::Fill(tiny3d::Color color)
{
	Fill(URect{ UPoint{ 0, 0 }, UPoint{ m_width, m_height } }, color);
}

void tiny3d::Image::ClearStencil(tiny3d::URect rect, tiny3d::Color::BlendMode stencil)
{
	URect dst = {
		{ Max(UInt(0), rect.a.x), Max(UInt(0), rect.a.y) },
		{ Min(UInt(m_width), rect.b.x), Min(UInt(m_height), rect.b.y) }
	};
	UHInt pixel_mask = 0xFFFF & ((stencil & 1) << 15);
	UHInt *pixel_row = m_pixels + (dst.a.y * m_width);
	for (UInt y = dst.a.y; y < dst.b.y; ++y) {
		for (UInt x = dst.a.x; x < dst.b.x; ++x) {
			pixel_row[x] &= pixel_mask;
		}
		pixel_row += m_width;
	}
}

tiny3d::Color::BlendMode tiny3d::Image::GetStencil(tiny3d::UPoint p) const
{
	UInt i = p.x + m_width * p.y;
	TINY3D_ASSERT(i < m_width * m_height);
	return (m_pixels[i] & 0x8000) ? tiny3d::Color::Solid : tiny3d::Color::Transparent;
}

void tiny3d::Image::SetStencil(tiny3d::UPoint p, tiny3d::Color::BlendMode stencil)
{
	UInt i = p.x + m_width * p.y;
	TINY3D_ASSERT(i < m_width * m_height);
	UHInt pixel_mask = 0xFFFF & ((stencil & 1) << 15);
	m_pixels[i] &= pixel_mask;
}

tiny3d::UInt tiny3d::Image::GetWidth( void ) const
{
	return m_width;
}

tiny3d::UInt tiny3d::Image::GetHeight( void ) const
{
	return m_height;
}

tiny3d::Color tiny3d::Image::GetColor(tiny3d::UPoint p) const
{
	UInt i = p.x + m_width * p.y;
	TINY3D_ASSERT(i < m_width * m_height);
	return DecodePixel(m_pixels[i]);
}

void tiny3d::Image::SetColor(tiny3d::UPoint p, tiny3d::Color color)
{
	UInt i = p.x + m_width * p.y;
	TINY3D_ASSERT(i < m_width * m_height);
	m_pixels[i] = EncodePixel(color);
}

tiny3d::Image &tiny3d::Image::operator=(const tiny3d::Image &i)
{
	Copy(i);
	return *this;
}
