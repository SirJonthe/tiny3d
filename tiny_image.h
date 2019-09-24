#ifndef TINY_IMAGE_H
#define TINY_IMAGE_H

#include "tiny_system.h"
#include "tiny_math.h"
#include "tiny_structs.h"

namespace tiny3d
{

class Image
{
private:
	tiny3d::UHInt *m_pixels;
	tiny3d::UInt   m_width;
	tiny3d::UInt   m_height;

private:
	tiny3d::UHInt EncodePixel(Color color) const;
	tiny3d::Color DecodePixel(UHInt pixel) const;

public:
	 Image( void );
	 explicit Image(tiny3d::UInt dimension);
	 Image(tiny3d::UInt width, tiny3d::UInt height);
	 Image(const tiny3d::Image &i);
	~Image( void );

	bool Create(tiny3d::UInt width, tiny3d::UInt height);
	bool Create(tiny3d::UInt dimensions);
	void Destroy( void );
	void Copy(const tiny3d::Image &i);
	void Fill(tiny3d::URect rect, tiny3d::Color color);
	void Fill(tiny3d::Color color);

	void                     ClearStencil(tiny3d::URect rect, tiny3d::Color::BlendMode stencil = tiny3d::Color::Solid);
	tiny3d::Color::BlendMode GetStencil(tiny3d::UPoint p) const;
	void                     SetStencil(tiny3d::UPoint p, tiny3d::Color::BlendMode stencil);

	tiny3d::UInt  GetWidth( void )  const;
	tiny3d::UInt  GetHeight( void ) const;
	tiny3d::Color GetColor(tiny3d::UPoint p) const;
	void          SetColor(tiny3d::UPoint p, tiny3d::Color color);

	void FlipX( void );
	void FlipY( void );

	tiny3d::Image &operator=(const tiny3d::Image &i);

	static constexpr tiny3d::UInt MaxDimension( void ) { return 0x400; }
};

}

#endif // TINY_IMAGE_H
