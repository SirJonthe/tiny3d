#ifndef TINY_DRAW_H
#define TINY_DRAW_H

#include "tiny_math.h"
#include "tiny_image.h"
#include "tiny_texture.h"
#include "tiny_structs.h"
#include "tiny_overlay.h"

/// @brief The width in pixels of a character in the built-in system font.
#define TINY3D_CHAR_WIDTH  6

/// @brief The height in pixels of a character in the built-in system font.
#define TINY3D_CHAR_HEIGHT 6

/// @brief The first character in the character table in the built-in system font.
#define TINY3D_CHAR_FIRST  '!'

/// @brief The last character in the character table in the built-in system font.
#define TINY3D_CHAR_LAST   '~'

namespace tiny3d
{
/// @brief Draws a single pixel point on the destination buffer.
/// @param zread The depth buffer used to determine visibility. NULL to disable depth read.
/// @param a The vertex to render.
/// @param tex The texture to use for rendering. NULL for untextured.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
/// @param dst The destination color buffer to draw a point to.
/// @param zwrite The depth buffer to store depth information in. NULL to disable depth write.
void DrawPoint(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::Vertex &a, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);

/// @brief Draws a single pixel width line on the destination buffer.
/// @param zread The depth buffer used to determine visibility. NULL to disable depth read.
/// @param a A vertex defining the line segment to render.
/// @param b A vertex defining the line segment to render.
/// @param tex The texture to use for rendering. NULL for untextured.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
/// @param dst The destination color buffer to draw a point to.
/// @param zwrite The depth buffer to store depth information in. NULL to disable depth write.
void DrawLine(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);

/// @brief Draws a triangle on the destination buffer.
/// @param zread The depth buffer used to determine visibility. NULL to disable depth read.
/// @param a A vertex defining the triangle to render.
/// @param b A vertex defining the triangle to render.
/// @param c A vertex defining the triangle to render.
/// @param tex The texture to use for rendering. NULL for untextured.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
/// @param dst The destination color buffer to draw a point to.
/// @param zwrite The depth buffer to store depth information in. NULL to disable depth write.
void DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Vertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);

/// @brief Draws a triangle on the destination buffer (SIMD version).
/// @param zread The depth buffer used to determine visibility. NULL to disable depth read.
/// @param a A vertex defining the triangle to render.
/// @param b A vertex defining the triangle to render.
/// @param c A vertex defining the triangle to render.
/// @param tex The texture to use for rendering. NULL for untextured.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
/// @param dst The destination color buffer to draw a point to.
/// @param zwrite The depth buffer to store depth information in. NULL to disable depth write.
void DrawTriangle_Fast(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Vertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);

/// @brief Draws a lightmap shaded triangle to the destination buffer.
/// @param zread The depth buffer used to determine visibility. NULL to disable depth read.
/// @param a A vertex defining the triangle to render.
/// @param b A vertex defining the triangle to render.
/// @param c A vertex defining the triangle to render.
/// @param tex The texture to use for rendering. NULL for untextured.
/// @param lightmap The non-optional light map used for shading the triangle.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
/// @param dst The destination color buffer to draw a point to.
/// @param zwrite The depth buffer to store depth information in. NULL to disable depth write.
void DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::LVertex &a, const tiny3d::LVertex &b, const tiny3d::LVertex &c, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap, const tiny3d::URect *dst_rect = nullptr);

/// @brief Draws a lightmap shaded triangle to the destination buffer (SIMD version).
/// @param zread The depth buffer used to determine visibility. NULL to disable depth read.
/// @param a A vertex defining the triangle to render.
/// @param b A vertex defining the triangle to render.
/// @param c A vertex defining the triangle to render.
/// @param tex The texture to use for rendering. NULL for untextured.
/// @param lightmap The non-optional light map used for shading the triangle.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
/// @param dst The destination color buffer to draw a point to.
/// @param zwrite The depth buffer to store depth information in. NULL to disable depth write.
void DrawTriangle_Fast(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const tiny3d::LVertex &a, const tiny3d::LVertex &b, const tiny3d::LVertex &c, const tiny3d::Texture *tex, const tiny3d::Texture &lightmap, const tiny3d::URect *dst_rect = nullptr);

/// @brief Transfers a source region to a destination region. Rescales source region to fit destination region.
/// @param dst The target/destination image buffer.
/// @param dst_region The target/destination region.
/// @param src The source overlay.
/// @param src_region The source region.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
void DrawRegion(tiny3d::Image &dst, tiny3d::Rect dst_region, const tiny3d::Overlay &src, tiny3d::Rect src_region, tiny3d::URect *dst_rect = nullptr);

/// @brief Draws a series of characters to the destination buffer using the built-in system font.
/// @param p The origin of render (top-left corner of text).
/// @param x_margin The left margin used when resetting caret on new line.
/// @param ch The series of characgters to render.
/// @param ch_num The number of characters to render.
/// @param color The color of the text.
/// @param scale The integer scale of the text.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
/// @param dst The destination color buffer to draw a point to.
tiny3d::Point DrawChars(tiny3d::Image &dst, tiny3d::Point p, tiny3d::SInt x_margin, const char *ch, tiny3d::UInt ch_num, tiny3d::Color color, tiny3d::UInt scale, const tiny3d::URect *dst_rect = nullptr);

/// @brief Transfers a source region to a destination region. Rescales source region to fit destination region.
/// @param dst The target/destination image buffer.
/// @param dst_region The target/destination region.
/// @param src The source image buffer.
/// @param src_region The source region.
/// @param dst_rect The mask rectangle. Discards rendering outside of the given bounds. NULL for full screen.
void DrawRegion(tiny3d::Image &dst, tiny3d::Rect dst_region, const tiny3d::Image &src, tiny3d::Rect src_region, tiny3d::URect *dst_rect = nullptr);

}

#endif // TINY_DRAW_H
