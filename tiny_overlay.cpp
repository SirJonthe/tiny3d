#include "tiny_overlay.h"

constexpr tiny3d::UInt BitCount = sizeof(tiny3d::Byte) * CHAR_BIT;

tiny3d::Overlay::Overlay( void ) :
	m_bits(nullptr), m_width(0), m_height(0), m_byte_width(0),
	m_colors{ tiny3d::Color{ 0, 0, 0, tiny3d::Color::Transparent }, tiny3d::Color{ 255, 255, 255, tiny3d::Color::Solid } }
{}

tiny3d::Overlay::Overlay(tiny3d::UInt dimension) : Overlay()
{
	Create(dimension);
}

tiny3d::Overlay::Overlay(tiny3d::UInt width, tiny3d::UInt height) : Overlay()
{
	Create(width, height);
}

tiny3d::Overlay::Overlay(const tiny3d::Byte *bit_field, tiny3d::UInt width, tiny3d::UInt height) : Overlay()
{
	FromBits(bit_field, width, height);
}

tiny3d::Overlay::Overlay(const tiny3d::Byte *bit_field, tiny3d::UInt dimension) : Overlay()
{
	FromBits(bit_field, dimension);
}

tiny3d::Overlay::Overlay(const tiny3d::Overlay &o) : Overlay()
{
	Copy(o);
}

tiny3d::Overlay::~Overlay( void )
{
	delete [] m_bits;
}

bool tiny3d::Overlay::FromBits(const tiny3d::Byte *bit_field, tiny3d::UInt width, tiny3d::UInt height)
{
	// NOTE: The bit field x-axis must be padded with unused data to fill out each byte boundary.
	if (Create(width, height) == false) { return false; }

	const tiny3d::UInt ByteCount = m_byte_width * m_height;
	for (tiny3d::UInt i = 0; i < ByteCount; ++i) {
		m_bits[i] = bit_field[i];
	}

	return true;
}

bool tiny3d::Overlay::FromBits(const tiny3d::Byte *bit_field, tiny3d::UInt dimension)
{
	return FromBits(bit_field, dimension, dimension);
}

bool tiny3d::Overlay::Create(tiny3d::UInt width, tiny3d::UInt height)
{
	Destroy();
	if (width <= MaxDimension() && height <= MaxDimension()) {
		m_width = width;
		m_height = height;
		m_byte_width = ((width % BitCount) == 0) ? (width / BitCount) : (width / BitCount) + 1;
		m_bits = new tiny3d::Byte[m_byte_width * m_height];
		return true;
	}
	return false;
}

bool tiny3d::Overlay::Create(tiny3d::UInt dimension)
{
	return Create(dimension, dimension);
}

void tiny3d::Overlay::Destroy( void )
{
	delete [] m_bits;
	m_bits = nullptr;
	m_width = 0;
	m_height = 0;
}

void tiny3d::Overlay::Copy(const tiny3d::Overlay &o)
{
	Create(o.GetWidth(), o.GetHeight());
	const tiny3d::UInt Count = m_byte_width * m_height;
	for (tiny3d::UInt i = 0; i < Count; ++i) {
		m_bits[i] = o.m_bits[i];
	}
}

void tiny3d::Overlay::Fill(tiny3d::URect rect, bool bit)
{
	const URect dst = {
		{ Max(UInt(0), rect.a.x), Max(UInt(0), rect.a.y) },
		{ Min(UInt(m_width), rect.b.x), Min(UInt(m_height), rect.b.y) }
	};

	const tiny3d::UInt misaligned_x0 = dst.a.x % BitCount;
	const tiny3d::UInt misaligned_x1 = dst.b.x % BitCount;
	const tiny3d::UInt start_x = (misaligned_x0 == 0) ? dst.a.x / BitCount : dst.a.x / BitCount + 1;
	const tiny3d::UInt end_x = dst.b.x / BitCount;
	const tiny3d::Byte BYTE = bit ? tiny3d::Byte(-1) : 0;
	for (tiny3d::UInt y = 0; y < m_height; ++y) {
		for (tiny3d::UInt x = dst.a.x; x < dst.a.x + misaligned_x0; ++x) {
			SetBit(tiny3d::UPoint{ x, y }, bit);
		}
		const tiny3d::UInt Y = y * m_width;
		for (tiny3d::UInt x = start_x; x < end_x; ++x) {
			m_bits[Y + x] = BYTE;
		}
		for (tiny3d::UInt x = dst.b.x - misaligned_x1; x < dst.b.x; ++x) {
			SetBit(tiny3d::UPoint{ x, y }, bit);
		}
	}
}

void tiny3d::Overlay::Fill(bool bit)
{
	const tiny3d::UInt Count = m_byte_width * m_height;
	const tiny3d::Byte BYTE = bit ? tiny3d::Byte(-1) : 0;
	for (tiny3d::UInt i = 0; i < Count; ++i) {
		m_bits[i] = BYTE;
	}
}

tiny3d::UInt tiny3d::Overlay::GetWidth( void ) const
{
	return m_width;
}

tiny3d::UInt tiny3d::Overlay::GetHeight( void ) const
{
	return m_height;
}

bool tiny3d::Overlay::GetBit(tiny3d::UPoint p) const
{
	TINY3D_ASSERT(p.x < m_width && p.y < m_height);
	const tiny3d::UInt X = p.x / BitCount;
	const tiny3d::Byte MASK = 1 << (p.x % BitCount);
	return (m_bits[p.y * m_byte_width + X] & MASK) > 0;
}

tiny3d::Color tiny3d::Overlay::GetColor(tiny3d::UPoint p) const
{
	return GetBit(p) ? m_colors[1] : m_colors[0];
}

void tiny3d::Overlay::SetBit(tiny3d::UPoint p, bool bit)
{
	TINY3D_ASSERT(p.x < m_width && p.y < m_height);
	const tiny3d::UInt X = p.x / BitCount;
	const tiny3d::Byte MASK = 1 << (p.x % BitCount);

	switch (bit) {
	case true:
		m_bits[p.y * m_byte_width + X] |= MASK;
		break;
	case false:
		m_bits[p.y * m_byte_width + X] &= (~MASK);
		break;
	}
}

void tiny3d::Overlay::SetColor0(tiny3d::Color c)
{
	m_colors[0] = c;
}

void tiny3d::Overlay::SetColor1(tiny3d::Color c)
{
	m_colors[1] = c;
}

void tiny3d::Overlay::SetColors(tiny3d::Color c0, tiny3d::Color c1)
{
	m_colors[0] = c0;
	m_colors[1] = c1;
}

tiny3d::Color tiny3d::Overlay::GetColor0( void ) const
{
	return m_colors[0];
}

tiny3d::Color tiny3d::Overlay::GetColor1( void ) const
{
	return m_colors[1];
}

void tiny3d::Overlay::FlipX( void )
{
	const tiny3d::UInt HalfWidth = m_width / 2;
	for (tiny3d::UInt y = 0; y < m_height; ++y) {
		for (tiny3d::UInt x = 0; x < HalfWidth; ++x) {
			const tiny3d::UPoint p0 = tiny3d::UPoint{ x, y };
			const tiny3d::UPoint p1 = tiny3d::UPoint{ m_width - x - 1, y };
			const bool x0 = GetBit(p0);
			const bool x1 = GetBit(p1);
			SetBit(p0, x1);
			SetBit(p1, x0);
		}
	}
}

void tiny3d::Overlay::FlipY( void )
{
	const tiny3d::UInt HalfHeight = m_height / 2;
	const tiny3d::UInt FullSize   = m_byte_width * (m_height - 1);
	for (tiny3d::UInt y = 0; y < HalfHeight; ++y) {
		const tiny3d::UInt Y = m_byte_width * y;
		const tiny3d::UInt Y0 = FullSize - Y;
		for (tiny3d::UInt x = 0; x < m_byte_width; ++x) {
			tiny3d::Swap(m_bits[Y + x], m_bits[Y0 + x]);
		}
	}
}

tiny3d::Overlay &tiny3d::Overlay::operator=(const tiny3d::Overlay &o)
{
	if (this != &o) {
		Copy(o);
	}
	return *this;
}
