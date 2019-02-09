#ifndef TINY_TEXTURE_H
#define TINY_TEXTURE_H

#include "tiny_system.h"
#include "tiny_structs.h"

namespace tiny3d
{

class Texture
{
private:
	struct CCCBlock
	{
		tiny3d::UHInt color_idx; // bit field
		tiny3d::UHInt colors[2];
	};

	struct Index
	{
		tiny3d::UInt block;
		tiny3d::UInt bit;
	};

private:
	CCCBlock                 *m_texels; // compressed
	tiny3d::UInt              m_dimension;
	tiny3d::UInt              m_dim_mask;
	tiny3d::UInt              m_dim_shift;
	tiny3d::Real              m_fix_dim;
	tiny3d::UInt              m_blocks;
	tiny3d::UInt              m_block_mask;
	tiny3d::UInt              m_block_shift;
	tiny3d::Real              m_fix_blocks;
	tiny3d::Color::BlendMode  m_blend_modes[2];

private:
	bool            SetDimension(tiny3d::UInt dimension);
	tiny3d::UInt    GetMortonIndex(tiny3d::UPoint p) const;
	tiny3d::UPoint  GetXY(tiny3d::Vector2 uv) const;
	tiny3d::UPoint  GetXY(tiny3d::UPoint p) const;
	Index           GetIndex(tiny3d::Vector2 uv) const;
	Index           GetIndex(tiny3d::UPoint p) const;
	tiny3d::UHInt   EncodeTexel(tiny3d::Color color) const;
	tiny3d::Color   DecodeTexel(tiny3d::UHInt texel) const;
	tiny3d::Color   GetColor(tiny3d::Texture::Index i) const;

public:
	 Texture( void );
	 explicit Texture(tiny3d::UInt dimension);
	 explicit Texture(const tiny3d::Image &img);
	 Texture(const tiny3d::Texture &t);
	~Texture( void );

	bool Create(tiny3d::UInt dimension);
	void Destroy( void );
	void Copy(const tiny3d::Texture &t);
	bool ToImage(tiny3d::Image &image) const;
	bool FromImage(const tiny3d::Image &image);

	tiny3d::UInt             GetWidth( void ) const;
	tiny3d::UInt             GetHeight( void ) const;
	tiny3d::Color            GetColor(tiny3d::Vector2 uv) const;
	tiny3d::Color            GetColor(tiny3d::UPoint p) const;
	tiny3d::Color::BlendMode GetBlendMode1( void ) const;
	tiny3d::Color::BlendMode GetBlendMode2( void ) const;
	void                     SetBlendMode1(tiny3d::Color::BlendMode blend_mode);
	void                     SetBlendMode2(tiny3d::Color::BlendMode blend_mode);
	Vector2                  ProjectUV(tiny3d::Vector2 uv) const;

	tiny3d::Texture &operator=(const tiny3d::Texture &r);

	static constexpr tiny3d::UInt MinDimension( void ) { return 0x4; }
	static constexpr tiny3d::UInt MaxDimension( void ) { return 0x100; }
};

}

#endif // TINY_TEXTURE_H
