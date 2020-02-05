#ifndef TINY_OVERLAY_H
#define TINY_OVERLAY_H

#include "tiny_system.h"
#include "tiny_structs.h"

namespace tiny3d
{

class Overlay
{
private:
	tiny3d::Byte *m_bits;
	tiny3d::UInt  m_width;
	tiny3d::UInt  m_height;
	tiny3d::UInt  m_byte_width;
	tiny3d::Color m_colors[2];

public:
	Overlay( void );
	explicit Overlay(tiny3d::UInt dimension);
	Overlay(tiny3d::UInt width, tiny3d::UInt height);
	Overlay(const tiny3d::Byte *bit_field, tiny3d::UInt width, tiny3d::UInt height);
	Overlay(const tiny3d::Byte *bit_field, tiny3d::UInt dimension);
	Overlay(const tiny3d::Overlay &o);
	~Overlay( void );

	bool FromBits(const tiny3d::Byte *bit_field, tiny3d::UInt width, tiny3d::UInt height);
	bool FromBits(const tiny3d::Byte *bit_field, tiny3d::UInt dimension);

	bool Create(tiny3d::UInt width, tiny3d::UInt height);
	bool Create(tiny3d::UInt dimension);

	void Destroy( void );

	void Copy(const tiny3d::Overlay &o);

	void Fill(tiny3d::URect rect, bool bit);
	void Fill(bool bit);

	tiny3d::UInt GetWidth( void ) const;
	tiny3d::UInt GetHeight( void ) const;

	bool          GetBit(tiny3d::UPoint p) const;
	tiny3d::Color GetColor(tiny3d::UPoint p) const;

	void SetBit(tiny3d::UPoint p, bool bit);

	void SetColor0(tiny3d::Color c);
	void SetColor1(tiny3d::Color c);
	void SetColors(tiny3d::Color c0, tiny3d::Color c1);

	tiny3d::Color GetColor0( void ) const;
	tiny3d::Color GetColor1( void ) const;

	void FlipX( void );
	void FlipY( void );

	tiny3d::Overlay &operator=(const tiny3d::Overlay &o);

	static constexpr tiny3d::UInt MaxDimension( void ) { return 0x400; }
};

}

#endif // TINY_OVERLAY_H
