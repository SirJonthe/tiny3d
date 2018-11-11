#include <SDL/SDL.h>

#include "tiny_system.h"
#include "tiny_image.h"

using namespace tiny3d;

bool tiny3d::System::Init(int width, int height, const std::string &caption)
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1) { return false; }
	if (SDL_SetVideoMode(width, height, 24, SDL_SWSURFACE) == nullptr) { return false; }
	SDL_WM_SetCaption(caption.c_str(), nullptr);
	return true;
}

void tiny3d::System::Close( void )
{
	SDL_FreeSurface(SDL_GetVideoSurface());
	SDL_Quit();
}

tiny3d::Real tiny3d::System::Time()
{
	Uint32 ticks = SDL_GetTicks();
	Real time = Real(float(ticks) / 1000.0f);
	return time;
}

void tiny3d::System::Video::Blit(const Image &src)
{
	if (SDL_GetVideoSurface() == nullptr) { return; }

	const UXInt vid_width  = UXInt(SDL_GetVideoSurface()->w);
	const UXInt vid_height = UXInt(SDL_GetVideoSurface()->h);
	const UXInt x_frac     = (UXInt(src.GetWidth()) << 16) / vid_width;
	const UXInt y_frac     = (UXInt(src.GetHeight()) << 16) / vid_height;

	unsigned char *pixel_row = reinterpret_cast<unsigned char*>(SDL_GetVideoSurface()->pixels);
	for (UXInt y = 0; y < vid_height; ++y) {
		unsigned char *pixels = pixel_row;
		for (UXInt x = 0; x < vid_width; ++x) {
			Color c = src.GetColor({UInt((x * x_frac) >> 16), UInt((y * y_frac) >> 16)});
			pixels[0] = c.r;
			pixels[1] = c.g;
			pixels[2] = c.b;
			pixels += SDL_GetVideoSurface()->format->BytesPerPixel;
		}
		pixel_row += SDL_GetVideoSurface()->pitch;
	}
}

void tiny3d::System::Video::Update( void )
{
	SDL_Flip(SDL_GetVideoSurface());
}

int tiny3d::System::Video::Width( void )
{
	return SDL_GetVideoSurface()->w;
}

int tiny3d::System::Video::Height( void )
{
	return SDL_GetVideoSurface()->h;
}
