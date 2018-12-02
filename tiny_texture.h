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
	tiny3d::UInt              m_dimension;
	tiny3d::UInt              m_dim_mask;
	tiny3d::UInt              m_dim_shift;
	tiny3d::Real              m_fix_dim;
	tiny3d::Color::BlendMode  m_blend_modes[2];

private:
	bool           SetDimension(tiny3d::UInt dimension);
	tiny3d::UInt   GetMortonIndex(tiny3d::UPoint p) const;
	tiny3d::UPoint GetXY(tiny3d::Vector2 uv) const;
	tiny3d::UPoint GetXY(tiny3d::UPoint p) const;
	tiny3d::UInt   GetIndex(tiny3d::Vector2 uv) const;
	tiny3d::UInt   GetIndex(tiny3d::UPoint p) const;
	tiny3d::UHInt  EncodeTexel(tiny3d::Color color) const;
	tiny3d::Color  DecodeTexel(tiny3d::UHInt texel) const;
	void           UpdateMip(UInt level, UInt cur_off, UInt cur_dim, UInt prev_off, UInt prev_dim);

public:
	 Texture( void );
	 explicit Texture(tiny3d::UInt dimension);
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
	void                     SetColor(tiny3d::UPoint p, tiny3d::Color color);
	tiny3d::Color::BlendMode GetBlendMode1( void ) const;
	tiny3d::Color::BlendMode GetBlendMode2( void ) const;
	void                     SetBlendMode1(tiny3d::Color::BlendMode blend_mode);
	void                     SetBlendMode2(tiny3d::Color::BlendMode blend_mode);
	Vector2                  ProjectUV(tiny3d::Vector2 uv) const;
	void                     ApplyChanges( void );

	tiny3d::Texture &operator=(const tiny3d::Texture &r);

	static constexpr tiny3d::UInt MaxDimension( void ) { return 0x80; }
};

/*class Texture
{
private:
	struct Map
	{
		tiny3d::UHInt *texels;
		tiny3d::UInt   dimensions;
		tiny3d::UInt   dim_mask;
		tiny3d::Real   rdimensions;
	};

private:
	tiny3d::Array<Map>       m_maps;
	tiny3d::Color::BlendMode m_blend_modes[2];
	tiny3d::UInt             m_level;

private:
	tiny3d::UHInt  EncodeTexel(tiny3d::Color color) const;
	tiny3d::Color  DecodeTexel(tiny3d::UHInt texel) const;
	tiny3d::UPoint GetXY(const tiny3d::Vector2 &uv, tiny3d::UInt l) const;
	tiny3d::UInt   GetIndex(tiny3d::UPoint p) const;

public:
	Texture( void );
	explicit Texture(tiny3d::UInt dimensions);
	Texture(const tiny3d::Texture &t);
	~Texture( void );

	bool Create(tiny3d::UInt dimensions);
	void Copy(const tiny3d::Texture &t);
	void Destroy( void );

	bool ToImage(tiny3d::Image &image) const;
	bool FromImage(const tiny3d::Image &image);
	void ApplyChanges( void );

	tiny3d::Color::BlendMode GetBlendMode1( void ) const;
	tiny3d::Color::BlendMode GetBlendMode2( void ) const;
	void                     SetBlendMode1(tiny3d::Color::BlendMode blend_mode);
	void                     SetBlendMode2(tiny3d::Color::BlendMode blend_mode);

	tiny3d::Color GetColor(tiny3d::Vector2 uv) const;
	tiny3d::Color GetColor(tiny3d::UPoint p) const;
	void          SetColor(tiny3d::UPoint p, tiny3d::Color c);

	tiny3d::UInt GetMipLevel( void ) const;
	void         SetMipLevel(tiny3d::UInt level);
	tiny3d::UInt GetMipCount( void ) const;
	tiny3d::UInt GetWidth( void ) const;
	tiny3d::UInt GetHeight( void ) const;

	static constexpr tiny3d::UInt MaxDimension( void ) { return 0x80; }
};*/

}

#endif // TINY_TEXTURE_H
