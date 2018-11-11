#include <iostream>
#include <math.h>

#include <SDL/SDL.h>

#include "tiny3d.h"

#include "tests/test_triangle.h"
#include "tests/test_math.h"
#include "tests/test_model.h"

using namespace tiny3d;

int main(int, char**)
{
	if (!tiny3d::System::Init(1024, 576, "Tiny3d Demo")) {
		std::cout << "multimedia system failed to init" << std::endl;
		return 1;
	}

	Test_Math();
//	Test_Triangle();
	Test_Model();

	tiny3d::System::Close();
	return 0;
}
