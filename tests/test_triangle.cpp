// low resolution
// only low-precision fixed point
// nearest neighbor
// per pixel lighting

#include <SDL/SDL.h>
#include <iostream>

#include "../tiny3d.h"

#include "test_triangle.h"
#include "test_aux.h"

using namespace tiny3d;

void CreateTexture(tiny3d::Texture &t)
{
	if (!t.Create(4)) { return; }
	for (UInt y = 0; y < t.GetHeight(); ++y) {
		for (UInt x = 0; x < t.GetWidth(); ++x) {
			Color c;
			if ((x + y) & 1) {
				c.r = 255;
				c.g = 255;
				c.b = 255;
				c.blend = 1;
			} else {
				c.r = 127;
				c.g = 127;
				c.b = 127;
				c.blend = 1;
			}
			t.SetColor({x, y}, c);
		}
	}
	t.ApplyChanges();
}

void Test_Triangle( void )
{
	Texture t;
	CreateTexture(t);

	Image screen;
	screen.Create(tiny3d::UHInt(tiny3d::System::Video::Width() / 8), tiny3d::UHInt(tiny3d::System::Video::Height() / 8));

	TextWriter writer = TextWriter(&screen);
	writer.SetColor({0,0,0,Color::BlendMode_Solid});

	const Vector3 a_start = Vector3(-Real(0.25f), -Real(0.25f), Real(0));
	const Vector3 b_start = Vector3( Real(0.25f), -Real(0.25f), Real(0));
	const Vector3 c_start = Vector3( Real(0.25f),  Real(0.25f), Real(0));
	const Vector3 d_start = Vector3(-Real(0.25f),  Real(0.25f), Real(0));

	bool quit = false;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			}
		}

		Real      time    = tiny3d::System::Time();
		Real      rot_ang = Wrap(-Pi(), Tau() * (time / 4), Pi());
		Matrix3x3 rot     = AxisAngle(Vector3(Real(1), Real(0), Real(0)), rot_ang);

		Vector3 av = a_start * rot + Vector3(Real(0), Real(0), Real(1));
		Vector3 bv = b_start * rot + Vector3(Real(0), Real(0), Real(1));
		Vector3 cv = c_start * rot + Vector3(Real(0), Real(0), Real(1));
		Vector3 dv = d_start * rot + Vector3(Real(0), Real(0), Real(1));

		Vertex a, b, c, d;
		a.v = Test_ProjectVector(av, screen);
		a.t = { Real(0), Real(0) };
		a.c = { 255, 0, 0, Color::BlendMode_Solid };
		b.v = Test_ProjectVector(bv, screen);
		b.t = { Real(1), Real(0) };
		b.c = { 0, 255, 0, Color::BlendMode_Solid };
		c.v = Test_ProjectVector(cv, screen);
		c.t = { Real(1), Real(1) };
		c.c = { 0, 0, 255, Color::BlendMode_Solid };
		d.v = Test_ProjectVector(dv, screen);
		d.t = { Real(0), Real(1) };
		d.c = { 255, 255, 255, Color::BlendMode_Solid };

		screen.Fill({{0, 0}, {screen.GetWidth(), screen.GetHeight()}}, {0,255,255,Color::BlendMode_Solid});
		tiny3d::DrawTriangle(screen, nullptr, a, b, c, &t);
		tiny3d::DrawTriangle(screen, nullptr, c, d, a, &t);
		writer.Write("time:").Write(time);
		writer.ResetCaret();

//		for (UInt y = 0; y < screen.GetHeight(); ++y) {
//			for (UInt x = 0; x < screen.GetWidth(); ++x) {
//				Color c = screen.GetColor({x, y});
//				Byte i = Illum(c);
//				screen.SetColor({x,y}, {i, i, i, Color::BlendMode_Solid});
//			}
//		}

		tiny3d::System::Video::Blit(screen);
		tiny3d::System::Video::Update();
	}
}
