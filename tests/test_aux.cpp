#include <sstream>

#include <SDL/SDL.h>

#include "test_aux.h"
#include "../tiny_draw.h"

using namespace tiny3d;

tiny3d::Vector3 Test_ProjectVector(tiny3d::Vector3 v, const tiny3d::Image &screen)
{
	Real screen_offset_x = Real(screen.GetWidth()) / 2;
	Real screen_offset_y = Real(screen.GetHeight()) / 2;
	Real screen_projection = Min(Real(screen.GetWidth()), -Real(screen.GetHeight()));
	Vector3 p = { (v.x / v.z) * screen_projection + screen_offset_x, (v.y / v.z) * screen_projection + screen_offset_y, v.z };
	return p;
}

bool Test_IsFrontfacing(tiny3d::Vector3 a, tiny3d::Vector3 b, tiny3d::Vector3 c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) > Real(0);
}

bool Test_LoadOBJ(const std::string &file, tiny3d::Array<tiny3d::Vector3> *v, tiny3d::Array<tiny3d::Vector2> *t, tiny3d::Array<tiny3d::Vector3> *n, tiny3d::Array<Material> *m)
{
	return false;
}

void Test_ScreenRefresh(tiny3d::Image &screen)
{
	System::Video::Blit(screen);
	System::Video::Update();
	screen.Fill({{0, 0}, {screen.GetWidth(), screen.GetHeight()}}, {0,255,255,Color::BlendMode_Solid});
}

bool Test_ShouldQuit( void )
{
	bool quit = false;
	SDL_Event event;
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
	return quit;
}

void Test_Render(const Model &m)
{
}

TextWriter::TextWriter(tiny3d::Image *dst) : m_dst(dst), m_caret{0,0}, m_caret_origin{0, 0}, m_color{0,0,0,0}, m_inv_color{0,0,0,0}, m_has_shadow(true)
{
	SetColor({255,255,255,1});
}

void TextWriter::SetImage(tiny3d::Image *dst)
{
	m_dst = dst;
}

void TextWriter::SetCaret(tiny3d::UPoint caret)
{
	m_caret = caret;
	m_caret_origin = caret;
}

void TextWriter::ResetCaret( void )
{
	m_caret = m_caret_origin;
}

void TextWriter::SetColor(tiny3d::Color color)
{
	m_color     = color;
	m_inv_color = { Byte(~color.r), Byte(~color.g), Byte(~color.b), color.blend };
}

void TextWriter::EnableShadow( void )
{
	m_has_shadow = true;
}

void TextWriter::DisableShadow( void )
{
	m_has_shadow = false;
}

TextWriter &TextWriter::Write(const std::string &text)
{
	if (m_dst == nullptr) { return *this; }

	for (size_t i = 0; i < text.size(); ++i) {
		if (text[i] == '\n' || text[i] == '\r') {
			m_caret.x = m_caret_origin.x;
			m_caret.y += TINY3D_CHAR_HEIGHT;
			continue;
		}

		if (text[i] != ' ' && text[i] != '\t') {
			if (m_has_shadow) {
				tiny3d::DrawChar(*m_dst, { SInt(m_caret.x + 1), SInt(m_caret.y + 1) }, text[i], m_inv_color, 1);
			}
			tiny3d::DrawChar(*m_dst, { SInt(m_caret.x), SInt(m_caret.y) }, text[i], m_color, 1);
		}

		m_caret.x += TINY3D_CHAR_WIDTH;
	}
	return *this;
}

TextWriter &TextWriter::Write(tiny3d::SInt num)
{
	std::ostringstream sout;
	sout << num;
	return Write(sout.str());
}

TextWriter &TextWriter::Write(tiny3d::Real num)
{
	float f = float(*reinterpret_cast<SInt*>(&num)) / (1 << TINY3D_REAL_PRECISION);
	std::ostringstream sout;
	sout << f;
	return Write(sout.str());
}

TextWriter &TextWriter::Debug_Write(float num)
{
	std::ostringstream sout;
	sout << num;
	return Write(sout.str());
}
