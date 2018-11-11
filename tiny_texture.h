#ifndef TINY_TEXTURE_H
#define TINY_TEXTURE_H

#include "tiny_system.h"
#include "tiny_structs.h"

// TODO
// Mip mapping: https://paroj.github.io/gltut/Texturing/Tut15%20How%20Mipmapping%20Works.html
// Compression

namespace tiny3d
{

class Texture
{
private:
	tiny3d::UHInt            *m_texels; // uncompressed texels
	tiny3d::UHInt             m_dimension;
	tiny3d::UHInt             m_dim_mask;
	tiny3d::UHInt             m_dim_shift;
	tiny3d::Real              m_fix_dim;
	tiny3d::Color::BlendMode  m_blend_modes[2];

private:
	bool           SetDimension(tiny3d::UHInt dimension);
	tiny3d::UInt   GetMortonIndex(tiny3d::UPoint p) const;
	tiny3d::UPoint GetXY(tiny3d::Vector2 uv) const;
	tiny3d::UPoint GetXY(tiny3d::UPoint p) const;
	tiny3d::UInt   GetIndex(tiny3d::Vector2 uv) const;
	tiny3d::UInt   GetIndex(tiny3d::UPoint p) const;
	tiny3d::UHInt  EncodeTexel(tiny3d::Color color) const;
	tiny3d::Color  DecodeTexel(tiny3d::UHInt texel) const;

public:
	 Texture( void );
	 explicit Texture(tiny3d::UHInt dimension);
	 Texture(const tiny3d::Texture &t);
	~Texture( void );

	bool Create(tiny3d::UHInt dimension);
	void Destroy( void );
	void Copy(const tiny3d::Texture &t);
	bool Copy(const tiny3d::Image &i);

	tiny3d::UHInt            GetWidth( void ) const;
	tiny3d::UHInt            GetHeight( void ) const;
	tiny3d::Color            GetColor(tiny3d::Vector2 uv) const;
	tiny3d::Color            GetColor(tiny3d::UPoint p) const;
	void                     SetColor(tiny3d::UPoint p, tiny3d::Color color);
	tiny3d::Color::BlendMode GetBlendMode1( void ) const;
	tiny3d::Color::BlendMode GetBlendMode2( void ) const;
	void                     SetBlendMode1(tiny3d::Color::BlendMode blend_mode);
	void                     SetBlendMode2(tiny3d::Color::BlendMode blend_mode);
	void                     ApplyChanges( void );

	tiny3d::Texture &operator=(const tiny3d::Texture &r);

	static tiny3d::UInt MaxDimension( void );
};

}

#endif // TINY_TEXTURE_H
