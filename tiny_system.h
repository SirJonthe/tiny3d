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

typedef float    Real;
typedef uint64_t UXInt;
typedef int64_t  SXInt;
typedef uint32_t UInt;
typedef int32_t  SInt;
typedef UInt     Word;
typedef uint16_t UHInt;
typedef int16_t  SHInt;
typedef uint8_t  Byte;

}

// @brief The bits per Tiny3d byte.
#define TINY3D_BITS_PER_BYTE (CHAR_BIT * sizeof(Byte))

// @brief The bits per Tiny3d word.
#define TINY3D_BITS_PER_WORD (CHAR_BIT * sizeof(Word))

// @brief The maximum value of a Tiny3d byte.
#define TINY3D_BYTE_MAX std::numeric_limits<Byte>::max()

// @brief The number of bits per compressed color channel.
#define TINY3D_BITS_PER_COLOR_CHANNEL 5

// @brief The number of shades per color channel.
#define TINY3D_SHADES_PER_COLOR_CHANNEL 32

#endif // TINY_SYSTEM_H
