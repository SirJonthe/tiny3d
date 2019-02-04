#ifndef TINY_DRAW_H
#define TINY_DRAW_H

#include "tiny_math.h"
#include "tiny_image.h"
#include "tiny_texture.h"
#include "tiny_structs.h"

#define TINY3D_CHAR_WIDTH  6
#define TINY3D_CHAR_HEIGHT 6

namespace tiny3d
{

void DrawPoint(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::WVertex &a, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);
void DrawPoint(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::Vertex &a, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);
void DrawLine(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, tiny3d::WVertex a, tiny3d::WVertex b, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);
void DrawLine(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);
void DrawTriangle(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::WVertex &a, const tiny3d::WVertex &b, const tiny3d::WVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);
void DrawTriangle(tiny3d::Image &dst, tiny3d::Array<tiny3d::Real> *zbuf, const tiny3d::Vertex &a, const tiny3d::Vertex &b, const tiny3d::Vertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect = nullptr);
void DrawChar(tiny3d::Image &dst, tiny3d::Point p, char ch, tiny3d::Color color, tiny3d::UInt scale, const tiny3d::URect *dst_rect = nullptr);
//void Blit(tiny3d::Image &dst, tiny3d::Rect dst_rect, const tiny3d::Image &src, tiny3d::Rect src_rect);
//void Blit(tiny3d::Image &dst, tiny3d::Rect dst_rect, const tiny3d::Texture &src, tiny3d::Rect src_rect);

}

#endif // TINY_DRAW_H
