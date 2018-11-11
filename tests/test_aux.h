#ifndef TEST_AUX_H
#define TEST_AUX_H

#include "../tiny3d.h"

struct Index
{
	tiny3d::SInt v, t, n;
};

struct Face
{
	tiny3d::Array<Index> i;
};

struct Material
{
	std::string         name;
	tiny3d::Array<Face> f;
	tiny3d::Color       cd;
	tiny3d::Texture     td;
};

struct Model
{
	tiny3d::Array<tiny3d::Vector3> v;
	tiny3d::Array<tiny3d::Vector2> t;
	tiny3d::Array<tiny3d::Vector3> n;
	tiny3d::Array<Material>        m;
};

tiny3d::Vector3 Test_ProjectVector(tiny3d::Vector3 v, const tiny3d::Image &screen);
bool            Test_IsFrontfacing(tiny3d::Vector3 a, tiny3d::Vector3 b, tiny3d::Vector3 c);
bool            Test_LoadOBJ(const std::string &file, tiny3d::Array<tiny3d::Vector3> *v, tiny3d::Array<tiny3d::Vector2> *t, tiny3d::Array<tiny3d::Vector3> *n, tiny3d::Array<Material> *m);
void            Test_ScreenRefresh(tiny3d::Image &screen);
bool            Test_ShouldQuit( void );
void            Test_Render(const Model &m);

class TextWriter
{
private:
	tiny3d::Image *m_dst;
	tiny3d::UPoint m_caret;
	tiny3d::UPoint m_caret_origin;
	tiny3d::Color  m_color;
	tiny3d::Color  m_inv_color;
	bool           m_has_shadow;

public:
	explicit TextWriter(tiny3d::Image *dst);
	void SetImage(tiny3d::Image *dst);
	void SetCaret(tiny3d::UPoint caret);
	void ResetCaret( void );
	void SetColor(tiny3d::Color color);
	void EnableShadow( void );
	void DisableShadow( void );
	TextWriter &Write(const std::string &text);
	TextWriter &Write(tiny3d::SInt num);
	TextWriter &Write(tiny3d::Real num);
	TextWriter &Debug_Write(float num);
};

#endif // TEST_AUX_H
