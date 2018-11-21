#include "tiny_texture.h"
#include "tiny_image.h"

using namespace tiny3d;

bool tiny3d::Texture::SetDimension(tiny3d::UHInt dimension)
{
	const bool is_valid = dimension <= MaxDimension() && tiny3d::IsPow2(dimension);
	if (is_valid) {
		m_dimension = dimension;
		m_dim_mask  = dimension - 1;
		m_dim_shift = UHInt(tiny3d::Log2(UInt(dimension)));
		m_fix_dim   = Real(SInt(dimension));
	} else {
		m_dimension = 0;
		m_dim_mask  = 0;
		m_dim_shift = 0;
		m_fix_dim   = Real(0);
	}
	return is_valid;
}

tiny3d::UInt tiny3d::Texture::GetMortonIndex(tiny3d::UPoint p) const
{
	p.x = (p.x | (p.x << 8)) & 0x00FF00FF;
	p.x = (p.x | (p.x << 4)) & 0x0F0F0F0F;
	p.x = (p.x | (p.x << 2)) & 0x33333333;
	p.x = (p.x | (p.x << 1)) & 0x55555555;

	p.y = (p.y | (p.y << 8)) & 0x00FF00FF;
	p.y = (p.y | (p.y << 4)) & 0x0F0F0F0F;
	p.y = (p.y | (p.y << 2)) & 0x33333333;
	p.y = (p.y | (p.y << 1)) & 0x55555555;

	return p.x | (p.y << 1);
}

tiny3d::UPoint tiny3d::Texture::GetXY(tiny3d::Vector2 uv) const
{
	UPoint p;
	p.x = UInt(SInt(uv.x * m_fix_dim)) & m_dim_mask;
	p.y = UInt(SInt(uv.y * m_fix_dim)) & m_dim_mask;
	return p;
}

tiny3d::UPoint tiny3d::Texture::GetXY(tiny3d::UPoint p) const
{
	p.x &= m_dim_mask;
	p.y &= m_dim_mask;
	return p;
}

tiny3d::UInt tiny3d::Texture::GetIndex(tiny3d::Vector2 uv) const
{
	return GetMortonIndex(GetXY(uv));
}

tiny3d::UInt tiny3d::Texture::GetIndex(tiny3d::UPoint p) const
{
	return GetMortonIndex(GetXY(p));
}

tiny3d::UHInt tiny3d::Texture::EncodeTexel(tiny3d::Color color) const
{
	// bits = M BBBB GGGG RRRR
	UInt r = (((UInt(color.r) + 1) * 0x100) - 1) >> 11;
	UInt g = (((UInt(color.g) + 1) * 0x100) - 1) >> 11;
	UInt b = (((UInt(color.b) + 1) * 0x100) - 1) >> 11;
	UInt stencil = UInt(color.blend & 1) << 15;
	UHInt texel = UHInt(stencil) | UHInt(r) | UHInt(g << 5) | UHInt(b << 10);
	return texel;
}

tiny3d::Color tiny3d::Texture::DecodeTexel(tiny3d::UHInt texel) const
{
	// bits = M BBBB GGGG RRRR
	UInt b =
		(((texel & 0x001F) + 1)        // convert channel to number with 1 bit of range and 5 bits of precision
		* 0x100                        // multiply by 1 with 1 bit of range and 8 bits of precision, resulting in a number with 13 bits of precision
		- 1)                           // subtract 1 to compress number back to 0 bits of range and 13 bits of precision
		>> 5;                          // shift away 5 bits of precision to keep a number with 0 bits of range and 8 bits of precision
	UInt g =
		((((texel & 0x03E0) >> 5) + 1)
		* 0x100
		- 1)
		>> 5;
	UInt r =
		((((texel & 0x7C00) >> 10) + 1)
		* 0x100
		- 1)
		>> 5;
	Color color;
	color.r = Byte(r);
	color.g = Byte(g);
	color.b = Byte(b);
	color.blend = (texel & 0x8000) ? m_blend_modes[1] : m_blend_modes[0];
	return color;
}

void tiny3d::Texture::UpdateMip(tiny3d::UInt level, tiny3d::UInt cur_off, tiny3d::UInt cur_dim, tiny3d::UInt prev_off, tiny3d::UInt prev_dim)
{
	for (UInt y = 0; y < cur_dim; ++y) {
		for (UInt x = 0; x < cur_dim; ++x) {
			Color c00 = DecodeTexel(m_texels[prev_off + y*2*prev_dim + x*2]);
			Color c10 = DecodeTexel(m_texels[prev_off + y*2*prev_dim + (x+1)*2]);
			Color c01 = DecodeTexel(m_texels[prev_off + (y+1)*2*prev_dim + x*2]);
			Color c11 = DecodeTexel(m_texels[prev_off + (y+1)*2*prev_dim + (x+1)*2]);
			UInt r = (UInt(c00.r) + UInt(c10.r) + UInt(c01.r) + UInt(c11.r)) / 4;
			UInt g = (UInt(c00.g) + UInt(c10.g) + UInt(c01.g) + UInt(c11.g)) / 4;
			UInt b = (UInt(c00.b) + UInt(c10.b) + UInt(c01.b) + UInt(c11.b)) / 4;
			UInt blend = Max(c00.blend, c10.blend, Max(c01.blend, c11.blend));
			m_texels[cur_off + y*cur_dim + x] = EncodeTexel(Color{ Byte(r), Byte(g), Byte(b), Byte(blend) });
		}
	}
	if (cur_dim > 8) {
		UpdateMip(level + 1, cur_off + cur_dim * cur_dim, cur_dim >> 1, cur_off, cur_dim);
	}
}

tiny3d::Texture::Texture( void ) : m_texels(nullptr), m_dimension(0), m_dim_mask(0), m_dim_shift(0), m_fix_dim(0), m_blend_modes{ Color::BlendMode_Transparent, Color::BlendMode_Solid }
{}

tiny3d::Texture::Texture(tiny3d::UHInt dimension) : Texture()
{
	Create(dimension);
}

tiny3d::Texture::Texture(const tiny3d::Texture &t) : Texture()
{
	Copy(t);
}

tiny3d::Texture::~Texture( void )
{
	delete [] m_texels;
	// TODO: Destroy mipmaps here
}

bool tiny3d::Texture::Create(tiny3d::UHInt dimension)
{
	if (dimension != m_dimension) {
		delete m_texels;
		if (SetDimension(dimension)) {
			m_texels = new UHInt[dimension * dimension];
			// TODO: Create mipmaps here
		} else {
			m_texels = nullptr;
			return false;
		}
	}
	return true;
}

void tiny3d::Texture::Destroy( void )
{
	delete [] m_texels;
	m_texels = nullptr;
	// TODO: Destroy mipmaps here
	SetDimension(0);
}

void tiny3d::Texture::Copy(const tiny3d::Texture &t)
{
	Create(t.m_dimension);
	m_blend_modes[0] = t.m_blend_modes[0];
	m_blend_modes[1] = t.m_blend_modes[1];
	for (UInt i = 0; i < m_dimension*m_dimension; ++i) {
		m_texels[i] = t.m_texels[i];
	}
	// TODO: Mipmap copying here
}

bool tiny3d::Texture::Copy(const tiny3d::Image &i)
{
	if (!Create(UHInt(Min(UInt(Max(i.GetWidth(), i.GetHeight())), Texture::MaxDimension())))) { return false; }
	for (UInt y = 0; y < GetHeight(); ++y) {
		for (UInt x = 0; x < GetWidth(); ++x) {
			UPoint p = { x, y };
			SetColor(p, i.GetColor(p));
		}
	}
	ApplyChanges();
	return true;
}

tiny3d::UHInt tiny3d::Texture::GetWidth( void ) const
{
	return m_dimension;
}

tiny3d::UHInt tiny3d::Texture::GetHeight( void ) const
{
	return m_dimension;
}

tiny3d::Color tiny3d::Texture::GetColor(tiny3d::Vector2 uv) const
{
	UInt i = GetIndex(uv);
	TINY3D_ASSERT(i < m_dimension * m_dimension);
	return DecodeTexel(m_texels[i]);
}

tiny3d::Color tiny3d::Texture::GetColor(tiny3d::UPoint p) const
{
	UInt i = GetIndex(p);
	TINY3D_ASSERT(i < m_dimension * m_dimension);
	return DecodeTexel(m_texels[i]);
}

void tiny3d::Texture::SetColor(tiny3d::UPoint p, tiny3d::Color color)
{
	UInt i = GetIndex(p);
	TINY3D_ASSERT(i < m_dimension * m_dimension);
	m_texels[i] = EncodeTexel(color);
}

tiny3d::Color::BlendMode tiny3d::Texture::GetBlendMode1( void ) const
{
	return m_blend_modes[0];
}

tiny3d::Color::BlendMode tiny3d::Texture::GetBlendMode2( void ) const
{
	return m_blend_modes[1];
}

void tiny3d::Texture::SetBlendMode1(tiny3d::Color::BlendMode blend_mode)
{
	m_blend_modes[0] = blend_mode;
}

void tiny3d::Texture::SetBlendMode2(tiny3d::Color::BlendMode blend_mode)
{
	m_blend_modes[1] = blend_mode;
}

void tiny3d::Texture::ApplyChanges( void )
{
//	UpdateMip(1, m_dimension * m_dimension, m_dimension >> 1, 0, m_dimension);
}

tiny3d::Texture &tiny3d::Texture::operator=(const tiny3d::Texture &t)
{
	if (this != &t) {
		Copy(t);
	}
	return *this;
}

tiny3d::UInt tiny3d::Texture::MaxDimension( void )
{
	return 0x80;
}
