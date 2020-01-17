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
//class Real;
typedef float Real;
struct URect;

typedef uint64_t UXInt;
typedef int64_t  SXInt;
typedef uint32_t UInt;
typedef int32_t  SInt;
typedef UInt     Word;
typedef uint16_t UHInt;
typedef int16_t  SHInt;
typedef uint8_t  Byte;

}

// @data TINY3D_BITS_PER_BYTE
// @info The bits per Tiny3d byte.
#define TINY3D_BITS_PER_BYTE            (CHAR_BIT * sizeof(Byte))

// @data TINY3D_BITS_PER_WORD
// @info The bits per Tiny3d word.
#define TINY3D_BITS_PER_WORD            (CHAR_BIT * sizeof(Word))

// @data TINY3D_BYTE_MAX
// @info The maximum value of a Tiny3d byte.
#define TINY3D_BYTE_MAX                 std::numeric_limits<Byte>::max()

// @data TINY3D_BITS_PER_COLOR_CHANNEL
// @info The number of bits per compressed color channel.
#define TINY3D_BITS_PER_COLOR_CHANNEL   5

// @data TINY3D_SHADES_PER_COLOR_CHANNEL
// @info The number of shades per color channel.
#define TINY3D_SHADES_PER_COLOR_CHANNEL 32

#endif // TINY_SYSTEM_H
