#include <iostream>
#include <math.h>

#include "tiny3d.h"

#include "tests/test_math.h"

using namespace tiny3d;

void Test_Sqr( void )
{
	std::cout << "Testing fixed point square root...";

	Real a = Real(10);
	Real c = a * a;
	Real s = tiny3d::Sqrt(c);
	TINY3D_ASSERT(SInt(s) == SInt(a));

	std::cout << "completed" << std::endl;
}

template < typename type_t >
type_t BruteWrap(type_t min, type_t i, type_t max)
{
	type_t span = max - min;
	if (i < min) { do { i += span; } while (i < min); }
	if (i > max) { do { i -= span; } while (i > max); }
	return i;
}

template < typename type_t = SInt >
SInt AlgoWrap(SInt min, SInt x, SInt max)
{
	SXInt span = max - min;
	SXInt abs = tiny3d::Abs(x);
	SXInt abs_span = abs * span;
	SXInt wrap_val = x + abs_span - min;
	SXInt wrapped = wrap_val % span;
	SXInt final = wrapped + min;
	return SInt(final);
}

void Test_Wrap( void )
{
	std::cout << "Testing fixed point value range wrapping...";
	Real end = Real(6.28f);
	Real start = -end;
	Real inc = Real(1);
	Real max = Real(3.14f);
	Real min = -max;
	for (Real i = start; i < end; i += inc) {
		Real awrap = tiny3d::Wrap(min, i, max);
		Real bwrap = BruteWrap(min, i, max);
		TINY3D_ASSERT(awrap == bwrap);
	}
	std::cout << "completed" << std::endl;
}

void Test_Math( void )
{
	Test_Sqr();
	Test_Wrap();
}
