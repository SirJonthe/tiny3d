#ifndef TINY_STRUCTS_H
#define TINY_STRUCTS_H

#include "tiny_system.h"
#include "tiny_math.h"

namespace tiny3d
{

/// @brief Contains coordinates for a point in signed space.
struct Point
{
	SInt x, y;
};

/// @brief Contains coordinates for a point in unsigned space.
struct UPoint
{
	UInt x, y;
};

/// @brief Contains minimum (a) and maximum (b) bounds for a rectangle in signed space.
struct Rect
{
	Point a, b;
};

/// @brief Gets the overlapping rectangle between two rectangles (if any).
/// @param a Input rectangle
/// @param b Input rectangle.
/// @return Overlapping rectangle (zero area if none).
Rect Clip(Rect a, Rect b);

/// @brief Contains minimum (a) and maximum (b) bounds for a rectangle in unsigned space.
struct URect
{
	UPoint a, b;
};

/// @brief Gets the overlapping rectangle between two rectangles (if any).
/// @param a Input rectangle.
/// @param b Input rectangle.
/// @return Overlapping rectangle (zero area if none).
URect Clip(URect a, URect b);


/// @brief Contains 32-bit color. RGB and blend mode.
struct Color
{
	/// @brief Possible values for blend mode.
	enum BlendMode
	{
		Transparent,     // don't render the pixel
		Emissive,        // render the pixel solid with lighting
		AddAlpha,        // additive alpha
		Solid,           // don't add lighting to the pixel
		EmissiveAddAlpha // emissive additive alpha
	};

	Byte r, g, b, blend;
};

Color operator+(Color l, Color r);
Color operator-(Color l, Color r);
Color operator*(Color l, Color r);
Color operator*(Color l, Real r);
Color operator/(Color l, Real r);

/// @brief Linearly interpolates two colors.
/// @param a The starting color at x=0.
/// @param b The end color at x=1.
/// @param x The fractional value [0-1] to blend the input colors.
/// @return The resulting color blend.
Color Lerp(Color a, Color b, Real x);

/// @brief Downsamples a 32-bit color to a 16-bit boundary based on where on the destination buffer the color is to be rendered. Used to improve quantization artifacts. 2x2 dither kernel.
/// @param c The color to be downsampled.
/// @param p The point on the destination buffer to be rendered. Used to sample the dithering kernel.
/// @return The final color.
Color Dither2x2(Color c, UPoint p);

/// @brief Downsamples a 32-bit color to a 16-bit boundary based on where on the destination buffer the color is to be rendered. Used to improve quantization artifacts. 3x3 dither kernel.
/// @param c The color to be downsampled.
/// @param p The point on the destination buffer to be rendered. Used to sample the dithering kernel.
/// @return The final color.
Color Dither3x3(Color c, UPoint p);

/// @brief Downsamples a 32-bit color to a 16-bit boundary based on where on the destination buffer the color is to be rendered. Used to improve quantization artifacts. 4x4 dither kernel.
/// @param c The color to be downsampled.
/// @param p The point on the destination buffer to be rendered. Used to sample the dithering kernel.
/// @return The final color.
Color Dither4x4(Color c, UPoint p);

/// @brief Downsamples a 32-bit color to a 16-bit boundary based on where on the destination buffer the color is to be rendered. Used to improve quantization artifacts. 8x8 dither kernel.
/// @param c The color to be downsampled.
/// @param p The point on the destination buffer to be rendered. Used to sample the dithering kernel.
/// @return The final color.
Color Dither8x8(Color c, UPoint p);

/// @brief Adds an offset to a texture UV (in texture space) to make for a bilinear approximation. 2x2 dither kernel used.
/// @param texture_space_uv UV coorinates in non-normalized texture space.
/// @param p The point on the destination buffer to be rendered. Used to sample the dithering kernel.
/// @return The final texture coordinate to sample.
UPoint Dither2x2(Vector2 texture_space_uv, UPoint p);

/// @brief Calculates the grayscale (or illuminance) of the input color.
/// @param c A color.
/// @return The grayscale (or illuminance) of the color.
Byte Illum(Color c);

/// @brief Decodes a 16-bit color into a 32-bit color.
/// @param c The 16-bit color.
/// @return the decoded 32-bit color.
Color Decode(UHInt c);

/// @brief Encodes a 32-bit color into a 16-bit color.
/// @param c The 32-bit color.
/// @return The 16-bit color.
UHInt Encode(Color c);

/// @brief Decodes a 16-bit color into a 32-bit color RGB565 format.
/// @param c The 16-bit color in 565 format.
/// @return the decoded 32-bit color.
Color Decode565(UHInt c);

/// @brief Encodes a 32-bit color into a 16-bit color RGB565 format.
/// @param c The 32-bit color.
/// @return The 16-bit color in 565 format.
UHInt Encode565(Color c);

/// @brief Contains information the rendering API interpolates and uses to determine final color of a pixel.
struct Vertex
{
	Vector3 v; // The position.
	Vector2 t; // The texture coordinate.
	Color   c; // The color.
};

/// @brief Contains information the rendering API interpolates and uses to determine final color of a pixel. Special use for light mapped triangles.
struct LVertex
{
	Vector3 v; // The position.
	Vector2 t; // The texture coordinate.
	Vector2 l; // The lightmap texture coordinate.
};

/// @brief Contains all possible values for texture sampling.
enum SampleMode
{
	SampleMode_Nearest, // Samples texels exactly where the specified UV lands. Blocky look.
	SampleMode_Dither, // Slightly offsets the UV based on the screen resolution in order to achieve a cheap bilinear effect. Blurry look.
	SampleMode_Bilinear, // Linearly interpolates texels using the input UV:s to achieve an antialiased result. Blurry look.
};

/// @brief A frequently used data structure used to contain an array of data stored linearly in memory.
/// @tparam type_t The underlying type of the array.
template < typename type_t >
class Array
{
private:
	type_t *m_arr; // The internal array.
	UInt    m_size; // The number of elements in the array.

public:
	/// @brief Creates an empty array.
	Array( void ) : m_arr(nullptr), m_size(0) {}

	/// @brief Copies an array.
	/// @param a The array to copy.
	Array(const Array<type_t> &a) : Array() { Copy(a); }

	/// @brief Allocates an array of a predetermined size.
	/// @param num The number of elements in the array to allocate.
	explicit Array(UInt num) : Array() { Create(num); }

	/// @brief Frees the memory of the array.
	~Array( void ) { delete [] m_arr; }

	/// @brief Creates an array with the given number of elements.
	/// @param num The number of elements to create.
	void Create(UInt num)
	{
		if (num == m_size) { return; }
		delete [] m_arr;
		m_arr = (num > 0) ? new type_t[num] : nullptr;
		m_size = num;
	}

	/// @brief Frees resources used by array.
	void Destroy( void )
	{
		delete [] m_arr;
		m_arr = nullptr;
		m_size = 0;
	}

	/// @brief Copies an array.
	/// @param a The array to copy.
	void Copy(const Array<type_t> &a)
	{
		if (this == &a) { return; }
		Create(a.m_size);
		for (UInt i = 0; i < m_size; ++i) {
			m_arr[i] = a.m_arr[i];
		}
	}

	/// @brief Copies an array.
	/// @param a The array to copy.
	/// @return The array (self).
	Array<type_t> &operator=(const Array<type_t> &a)
	{
		Copy(a);
		return *this;
	}

	/// @brief Used to fetch an element in the array.
	/// @param i The index of the element to fetch.
	/// @return The element.
	type_t &operator[](UInt i)
	{
		TINY3D_ASSERT(i < m_size);
		return m_arr[i];
	}
	const type_t &operator[](UInt i) const
	{
		TINY3D_ASSERT(i < m_size);
		return m_arr[i];
	}

	/// @return The raw array of elements.
	operator type_t*( void )             { return m_arr; }
	operator const type_t*( void ) const { return m_arr; }

	/// @return The number of elements in the array.
	UInt GetSize( void ) const { return m_size; }
};

/// @brief Contains 256 16-bit colors.
struct Palette
{
	Color colors[256]; // The colors in the palette.
};

}

#endif // TINY_STRUCTS_H
