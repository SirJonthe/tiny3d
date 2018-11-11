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
	tiny3d::UHInt  m_width;
	tiny3d::UHInt  m_height;

private:
	tiny3d::UHInt EncodePixel(Color color) const;
	tiny3d::Color DecodePixel(UHInt pixel) const;

public:
	 Image( void );
	 explicit Image(tiny3d::UHInt dimension);
	 Image(tiny3d::UHInt width, tiny3d::UHInt height);
	 Image(const tiny3d::Image &i);
	~Image( void );

	bool Create(tiny3d::UHInt width, tiny3d::UHInt height);
	bool Create(tiny3d::UHInt dimensions);
	void Destroy( void );
	void Copy(const tiny3d::Image &i);
	void Fill(tiny3d::URect rect, tiny3d::Color color);
	void ClearStencil(tiny3d::URect rect, tiny3d::Color::BlendMode stencil = tiny3d::Color::BlendMode_Solid);

	tiny3d::UHInt GetWidth( void )  const;
	tiny3d::UHInt GetHeight( void ) const;
	tiny3d::Color GetColor(tiny3d::UPoint p) const;
	void          SetColor(tiny3d::UPoint p, tiny3d::Color color);

	tiny3d::Image &operator=(const tiny3d::Image &i);

	static tiny3d::UInt MaxDimension( void );
};

}

#endif // TINY_IMAGE_H
