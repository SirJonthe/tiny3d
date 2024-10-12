#ifndef TINY_IMAGE_H
#define TINY_IMAGE_H

#include "tiny_system.h"
#include "tiny_math.h"
#include "tiny_structs.h"

namespace tiny3d
{

/// @brief Contains pixel information. 16 bits per pixel (5 bits per RGB channel + 1 bit for stencil).
class Image
{
private:
	tiny3d::UHInt *m_pixels; // The picture elements, encoded as 16-bit colors (5,5,5 rgb, 1 stencil).
	tiny3d::UInt   m_width; // The number of pixels in the X axis.
	tiny3d::UInt   m_height; // The number of pixels in the Y axis.

private:
	/// @brief Encodes a color 32-bit color into a 16-bit pixel.
	/// @param color The color to encode into a pixel.
	/// @return The encoded pixel.
	tiny3d::UHInt EncodePixel(Color color) const;

	/// @brief Decodes a 16-bit pixel into a 32-bit color.
	/// @param pixel The pixel to decode into a color.
	/// @return The decoded color.
	tiny3d::Color DecodePixel(UHInt pixel) const;

public:
	 /// @brief Creates an empty image object.
	 Image( void );

	 /// @brief Creates an image of a specified dimension.
	 /// @param dimension The dimensions to allocate.
	 explicit Image(tiny3d::UInt dimension);

	 /// @brief Creates an image of a specified dimension.
	 /// @param width The width to allocate.
	 /// @param height The height to allocate.
	 Image(tiny3d::UInt width, tiny3d::UInt height);

	 /// @brief Copies an image.
	 /// @param i The image to copy.
	 Image(const tiny3d::Image &i);

	/// @brief Free the memory of the image.
	~Image( void );

	/// @brief Creates a new image surface with the speficied dimensions.
	/// @note Maximum dimensions are defined by MaxDImension.
	/// @param width The unsigned width of the new image surface.
	/// @param height The unsigned height of the new image surface.
	/// @return TRUE on success.
	bool Create(tiny3d::UInt width, tiny3d::UInt height);
	
	/// @brief Creates a new image surface with the speficied dimensions.
	/// @note Maximum dimensions are defined by MaxDImension.
	/// @param dimensions The unsigned dimension of the new image surface.
	/// @return TRUE on success.
	bool Create(tiny3d::UInt dimensions);
	
	/// @brief Releases the image resources.
	void Destroy( void );
	
	/// @brief Copies an image surface.
	/// @param i The surface to copy.
	void Copy(const tiny3d::Image &i);
	
	/// @brief Fills a rectangle with a given color.
	/// @param rect The rectangle to fill.
	/// @param color The color to use.
	void Fill(tiny3d::URect rect, tiny3d::Color color);
	
	/// @brief Fills the entire image with a given color.
	/// @param color The color to use.
	void Fill(tiny3d::Color color);

	/// @brief Sets the stencil bit in the specified rectangle.
	/// @param rect The rectangle to clear the stencil in.
	/// @param stencil The state of the stencil to set.
	void ClearStencil(tiny3d::URect rect, tiny3d::Color::BlendMode stencil = tiny3d::Color::Solid);
	
	/// @brief Retrieves a stencil bit.
	/// @param p The coordinate of the stencil bit to fetch.
	/// @return The requested stencil bit.
	tiny3d::Color::BlendMode GetStencil(tiny3d::UPoint p) const;
	
	/// @brief Sets a stencil bit.
	/// @param p The coordinate of the stencil to set.
	/// @param stencil The state to set the stencil.
	void SetStencil(tiny3d::UPoint p, tiny3d::Color::BlendMode stencil);

	/// @return The width in pixels of the image.
	tiny3d::UInt GetWidth( void )  const;
	
	/// @return The height in pixels of the image.
	tiny3d::UInt GetHeight( void ) const;
	
	/// @brief Decodes a color at a given coordinate into a 32-bit value.
	/// @note Needs to decode a 16-bit color to a 32-bit color. May be slow.
	/// @param p The coordinate of the color to get.
	/// @return The color.
	tiny3d::Color GetColor(tiny3d::UPoint p) const;
	
	/// @brief Sets the color of a pixel at a given coordinate.
	/// @note Needs to encode the input 32-bit color to a output 16-bit color. May be slow.
	/// @param p The coordinate of the color to set.
	/// @param color The color to set.
	void SetColor(tiny3d::UPoint p, tiny3d::Color color);

	/// @brief Sets the color matching the key to Transparent.
	/// @param key The color to make transparent.
	void SetColorKey(tiny3d::Color key);
	
	/// @brief Mirror the image in the X axis.
	void FlipX( void );
	
	/// @brief Mirror the image in the Y axis.
	void FlipY( void );

	/// @brief Copies an image.
	/// @param i The image to be copied.
	/// @return The image (self).
	tiny3d::Image &operator=(const tiny3d::Image &i);

	/// @brief Returns the maximum supported image dimension.
	/// @return The maximum supported image size in one dimension.
	static constexpr tiny3d::UInt MaxDimension( void ) { return 0x400; }
};

}

#endif // TINY_IMAGE_H
