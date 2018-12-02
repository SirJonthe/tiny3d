#include "tiny_texture.h"
#include "tiny_image.h"

using namespace tiny3d;

bool tiny3d::Texture::SetDimension(tiny3d::UInt dimension)
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

tiny3d::Texture::Texture(tiny3d::UInt dimension) : Texture()
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

bool tiny3d::Texture::Create(tiny3d::UInt dimension)
{
	if (dimension != m_dimension) {
		delete m_texels;
		if (SetDimension(dimension)) {
			m_texels = new UHInt[dimension * dimension];
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
}

bool tiny3d::Texture::ToImage(tiny3d::Image &image) const
{
	UInt dim = GetWidth();
	if (!image.Create(dim, dim)) { return false; }
	for (UInt y = 0; y < dim; ++y) {
		for (UInt x = 0; x < dim; ++x) {
			image.SetColor({ x, y }, GetColor({ x, y }));
		}
	}
	return true;
}

bool tiny3d::Texture::FromImage(const tiny3d::Image &image)
{
	if (image.GetWidth() != image.GetHeight() || !Create(image.GetWidth())) { return false; }

	UInt dim = GetWidth();
	for (UInt y = 0; y < dim; ++y) {
		for (UInt x = 0; x < dim; ++x) {
			SetColor({ x, y }, image.GetColor({ x, y }));
		}
	}
	ApplyChanges();

	return true;
}

tiny3d::UInt tiny3d::Texture::GetWidth( void ) const
{
	return m_dimension;
}

tiny3d::UInt tiny3d::Texture::GetHeight( void ) const
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

tiny3d::Vector2 tiny3d::Texture::ProjectUV(tiny3d::Vector2 uv) const
{
	return Vector2{ uv.x * m_fix_dim, uv.y * m_fix_dim };
}

void tiny3d::Texture::ApplyChanges( void )
{
	// Compress
}

tiny3d::Texture &tiny3d::Texture::operator=(const tiny3d::Texture &t)
{
	if (this != &t) {
		Copy(t);
	}
	return *this;
}






/*tiny3d::UHInt tiny3d::Texture::EncodeTexel(tiny3d::Color color) const
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

tiny3d::UPoint tiny3d::Texture::GetXY(const tiny3d::Vector2 &uv, tiny3d::UInt l) const
{
	UInt x = UInt(SInt(uv.x * m_maps[l].rdimensions)) & m_maps[l].dim_mask;
	UInt y = UInt(SInt(uv.y * m_maps[l].rdimensions)) & m_maps[l].dim_mask;
	return { x, y };
}

tiny3d::UInt tiny3d::Texture::GetIndex(tiny3d::UPoint p) const
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

tiny3d::Texture::Texture( void ) : m_maps(), m_blend_modes{ Color::BlendMode_Transparent, Color::BlendMode_Solid }, m_level(0)
{}

tiny3d::Texture::Texture(tiny3d::UInt dimensions) : Texture()
{
	Create(dimensions);
}

tiny3d::Texture::Texture(const tiny3d::Texture &t) : Texture()
{
	Copy(t);
}

tiny3d::Texture::~Texture( void )
{
	Destroy();
}

bool tiny3d::Texture::Create(tiny3d::UInt dimensions)
{
	Destroy();
	if (dimensions == 0 || dimensions > MaxDimension() || IsPow2(dimensions) == false) { return false; }
	UInt levels = 1;
	for (UInt d = dimensions; d >= 8; d >>= 1) {
		++levels;
	}
	m_maps.Create(levels);
	UInt dim_mask  = dimensions - 1;
	for (UInt i = 0; i < m_maps.GetSize(); ++i) {
		m_maps[i] = { new UHInt[dimensions*dimensions], dimensions, dim_mask, Real(SInt(dimensions)) };
		dimensions >>= 1;
		dim_mask >>= 1;
	}
	return true;
}

void tiny3d::Texture::Copy(const tiny3d::Texture &t)
{
	Destroy();
	m_maps.Create(t.m_maps.GetSize());
	for (UInt l = 0; l < m_maps.GetSize(); ++l) {
		m_maps[l].dim_mask = t.m_maps[l].dim_mask;
		m_maps[l].dimensions = t.m_maps[l].dimensions;
		m_maps[l].rdimensions = t.m_maps[l].rdimensions;
		const UInt AREA = m_maps[l].dimensions * m_maps[l].dimensions;
		m_maps[l].texels = new UHInt[AREA];
		for (UInt i = 0; i < AREA; ++i) {
			m_maps[l].texels[i] = t.m_maps[l].texels[i];
		}
	}
	SetMipLevel(0);
}

void tiny3d::Texture::Destroy( void )
{
	for (UInt i = 0; i < m_maps.GetSize(); ++i) {
		delete [] m_maps[i].texels;
	}
	m_maps.Destroy();
	SetMipLevel(0);
}

bool tiny3d::Texture::ToImage(tiny3d::Image &image) const
{
	UInt dim = GetWidth();
	if (!image.Create(dim, dim)) { return false; }
	for (UInt y = 0; y < dim; ++y) {
		for (UInt x = 0; x < dim; ++x) {
			image.SetColor({ x, y }, GetColor({ x, y }));
		}
	}
	return true;
}

bool tiny3d::Texture::FromImage(const tiny3d::Image &image)
{
	if (image.GetWidth() != image.GetHeight() || !Create(image.GetWidth())) { return false; }

	UInt dim = GetWidth();
	for (UInt y = 0; y < dim; ++y) {
		for (UInt x = 0; x < dim; ++x) {
			SetColor({ x, y }, image.GetColor({ x, y }));
		}
	}
	ApplyChanges();
	SetMipLevel(0);

	return true;
}

void tiny3d::Texture::ApplyChanges( void )
{
	for (UInt i = 1; i < m_maps.GetSize(); ++i) {
		UInt j = i - 1;
		for (UInt y = 0; y < m_maps[i].dimensions; ++y) {
			for (UInt x = 0; x < m_maps[i].dimensions; ++x) {
				SetMipLevel(j);
				Color c00 = GetColor({ x*2, y*2 });
				Color c10 = GetColor({ x*2 + 1, y*2 });
				Color c01 = GetColor({ x*2, y*2 + 1 });
				Color c11 = GetColor({ x*2 + 1, y*2 + 1 });
				Vector3 v00 = { Real(c00.r), Real(c00.g), Real(c00.b) };
				Vector3 v10 = { Real(c10.r), Real(c10.g), Real(c10.b) };
				Vector3 v01 = { Real(c01.r), Real(c01.g), Real(c01.b) };
				Vector3 v11 = { Real(c11.r), Real(c11.g), Real(c11.b) };
				Vector3 vx = (v00 + v10 + v01 + v11) / 4;
				SetMipLevel(i);
				SetColor({ x, y }, Color{ Byte(SInt(vx.x)), Byte(SInt(vx.y)), Byte(SInt(vx.z)), 1 });
			}
		}
	}
	SetMipLevel(0);
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

tiny3d::UInt tiny3d::Texture::GetMipLevel( void ) const
{
	return m_level;
}

void tiny3d::Texture::SetMipLevel(tiny3d::UInt level)
{
	m_level = Min(level, m_maps.GetSize() > 0 ? m_maps.GetSize() - 1 : 0);
}

tiny3d::UInt tiny3d::Texture::GetMipCount( void ) const
{
	return m_maps.GetSize();
}

tiny3d::UInt tiny3d::Texture::GetWidth( void ) const
{
	return m_maps.GetSize() > 0 ? m_maps[0].dimensions : 0;
}

tiny3d::UInt tiny3d::Texture::GetHeight( void ) const
{
	return GetWidth();
}

tiny3d::Color tiny3d::Texture::GetColor(tiny3d::Vector2 uv) const
{
	const UInt i = GetIndex(GetXY(uv, m_level));
	TINY3D_ASSERT(i < m_maps[m_level].dimensions * m_maps[m_level].dimensions);
	return DecodeTexel(m_maps[m_level].texels[i]);
}

tiny3d::Color tiny3d::Texture::GetColor(tiny3d::UPoint p) const
{
	const UInt i = GetIndex(p);
	TINY3D_ASSERT(i < m_maps[m_level].dimensions * m_maps[m_level].dimensions);
	return DecodeTexel(m_maps[m_level].texels[i]);
}

void tiny3d::Texture::SetColor(tiny3d::UPoint p, tiny3d::Color c)
{
	const UInt i = GetIndex(p);
	TINY3D_ASSERT(i < m_maps[m_level].dimensions * m_maps[m_level].dimensions);
	m_maps[m_level].texels[i] = EncodeTexel(c);
}*/
