#ifndef TINY_TEXTURE_H
#define TINY_TEXTURE_H

#include "tiny_system.h"
#include "tiny_image.h"
#include "tiny_structs.h"

// TODO
// [ ] The user of tiny3d will not have direct access to this data structure.
// [ ] The user of tiny3d will only have access to an integer texture ID.
// [ ] The Texture data structure will not allocate or de-allocate memory per-se, and will use a central memory manager which allocate textures inside a giant atlas.
// [ ] Texels and texture metadata (dimensions, other useful data) are stored separately.
// [ ] Compressed color cell blocks do not store colors, but indices to a color palette.

namespace tiny3d
{

/// @brief Contains compressed color data (average 3 bits per element) in a spatial format that is especially suited for pseudo-linear access in non-axis aligned access patterns.
class Texture
{
private:
	/// @brief A block of compressed color cells (CCC = Color Cell Compression).
	struct CCCBlock
	{
		tiny3d::UHInt color_idx; // 4x4 bit field used as indices into the color lookup table.
		tiny3d::UHInt colors[2]; // The color lookup table.
	};

	/// @brief An index into the compressed block of texels.
	struct Index
	{
		tiny3d::UInt block; // The index of the block.
		tiny3d::UInt bit;   // The index of the bit in the block.
	};

private:
	CCCBlock                 *m_texels;         // Compressed texels.
	tiny3d::UInt              m_dimension;      // The dimensions of the texture (both axis are equal power-of-2).
	tiny3d::UInt              m_dim_mask;       // The dimensions as a bit mask.
	tiny3d::UInt              m_dim_shift;      // The number of bitshifts to scale up or down a value by the dimensions.
	tiny3d::Real              m_fix_dim;        // 
	tiny3d::UInt              m_blocks;         // The number of compressed texel blocks in the texel array (power-of-2).
	tiny3d::UInt              m_block_mask;     // The number of compressed texel blocks as a bit mask.
	tiny3d::UInt              m_block_shift;    // The number of bitshifts to scale up or down a value by the number of compressed texel blocks.
	tiny3d::Real              m_fix_blocks;     // 
	tiny3d::Color::BlendMode  m_blend_modes[2]; // The stored blend modes.

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

	/// @brief Creates a new texture surface with the speficied dimensions.
	/// @note Maximum dimensions are defined by MaxDImension and minimum dimensions are specified in MinDimensions. Only supports width equal to height, and a power of 2 dimension.
	/// @param dimensions The unsigned dimension of the new texture surface.
	/// @return TRUE on success.
	bool Create(tiny3d::UInt dimension);
	
	/// @brief Releases the texture resources.
	void Destroy( void );
	
	/// @brief Copies an texture surface.
	/// @param t The surface to copy.
	void Copy(const tiny3d::Texture &t);
	
	/// @brief Copies contents of texture to a newly created image.
	/// @param image The resulting image.
	/// @return TRUE on success (this should always return TRUE, unless Image specifications change).
	bool ToImage(tiny3d::Image &image) const;
	
	/// @brief Converts an image to a texture. Has same constraints as Create.
	/// @note This is a lossy compression algorithm.
	/// @param image The image to convert.
	/// @return TRUE on success.
	bool FromImage(const tiny3d::Image &image);

	/// @return The width in pixels of the image.
	tiny3d::UInt GetWidth( void ) const;
	
	/// @return The height in pixels of the image.
	tiny3d::UInt GetHeight( void ) const;
	
	/// @brief Gets a color based on normalized texture coordinates (0-1). Coordinates less than 0 or greater than 1 wraps around to 0-1 range.
	/// @param uv The normalized texture coordinates (0-1).
	/// @return The color.
	tiny3d::Color GetColor(tiny3d::Vector2 uv) const;
	
	/// @brief Decodes a color at a given coordinate into a 32-bit value.
	/// @note Needs to decode a 16-bit color to a 32-bit color. May be slow.
	/// @param p The coordinate of the color to get.
	/// @return The color.
	tiny3d::Color GetColor(tiny3d::UPoint p) const;
	
	/// @return The primary blend mode.
	tiny3d::Color::BlendMode GetBlendMode1( void ) const;
	
	/// @return The secondary blend mode.
	tiny3d::Color::BlendMode GetBlendMode2( void ) const;
	
	/// @param blend_mode The new primary blend mode.
	void SetBlendMode1(tiny3d::Color::BlendMode blend_mode);
	
	/// @param blend_mode The new secondary blend mode.
	void SetBlendMode2(tiny3d::Color::BlendMode blend_mode);
	
	/// @brief Projects normalized UV texture coordinates (0-1) to absolute texture coordinates.
	/// @param uv The normalized UV texture coordinates (0-1). Coordinates less than 0 or greater than 1 wraps around to 0-1 range.
	/// @return The projected absolute texture coordinates.
	tiny3d::Vector2 ProjectUV(tiny3d::Vector2 uv) const;

	/// @brief Copies a texture.
	/// @param i The texture to be copied.
	/// @return The texture (self).
	tiny3d::Texture &operator=(const tiny3d::Texture &r);

	/// @return The minimum supported image size in one dimension.
	static constexpr tiny3d::UInt MinDimension( void ) { return 0x4; }
	
	/// @return The maximum supported image size in one dimension.
	static constexpr tiny3d::UInt MaxDimension( void ) { return 0x100; }
};

}

#endif // TINY_TEXTURE_H
