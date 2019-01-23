#ifndef TINY_SYSTEM_H
#define TINY_SYSTEM_H

#include <assert.h>
#include <limits>
#include <limits.h>
#include <string>
#include <cstdint>

#ifdef TINY3D_ASSERT
	#undef TINY3D_ASSERT
#endif

#ifdef TINY3D_DEBUG
	#define TINY3D_ASSERT(x) assert(x)
#else
	#define TINY3D_ASSERT(x)
#endif

namespace tiny3d
{

class Image;
class Real;
struct URect;

typedef uint64_t UXInt;
typedef int64_t  SXInt;
typedef uint32_t UInt;
typedef int32_t  SInt;
typedef UInt     Word;
typedef uint16_t UHInt;
typedef int16_t  SHInt;
typedef uint8_t  Byte;

namespace System
{
	bool         Init(tiny3d::UInt width, tiny3d::UInt height, const std::string &caption);
	void         Close( void );
	tiny3d::Real Time( void );

	namespace Video
	{
		void Blit(const Image &src, const URect *dst_rect = nullptr);
		void Update(const URect *dst_rect = nullptr);
		tiny3d::UInt  Width( void );
		tiny3d::UInt  Height( void );
	}

	namespace Audio
	{
	}

	namespace Input
	{
		// Touch
		// Controller
		// Keyboard
	}
};

}

#define TINY3D_BITS_PER_BYTE            (CHAR_BIT * sizeof(Byte))
#define TINY3D_BITS_PER_WORD            (CHAR_BIT * sizeof(Word))
#define TINY3D_BYTE_MAX                 std::numeric_limits<Byte>::max()
#define TINY3D_BITS_PER_COLOR_CHANNEL   5
#define TINY3D_SHADES_PER_COLOR_CHANNEL 32

#endif // TINY_SYSTEM_H
