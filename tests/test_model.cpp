#include <fstream>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <list>

#include <SDL/SDL.h>

#include "test_model.h"
#include "test_triangle.h"
#include "test_aux.h"

#include "tiny3d.h"

using namespace tiny3d;

struct AABB
{
	Vector3 min;
	Vector3 max;
};

Real ReadF(std::ifstream &fin)
{
	std::string in;
	fin >> in;
	return Real(std::stof(in));
}

Face ReadI(std::ifstream &fin)
{
	std::string line, arg;
	std::getline(fin, line);
	std::istringstream sin(line);

	UInt i_count = 0;
	while (sin >> arg) {
		++i_count;
	}


	Face f;
	f.i.Create(i_count);
	i_count = 0;
	sin = std::istringstream(line);
	while (sin >> arg) {
		std::string v, t, n;
		size_t a = arg.find_first_of('/');
		size_t b = arg.find_last_of('/');
		v = arg.substr(0, a);
		t = (a < std::string::npos && b - a - 1 > 0) ? arg.substr(a + 1, b - a - 1) : "0";
		n = (b < std::string::npos) ? arg.substr(b + 1) : "0";

		f.i[i_count++] = {
			SInt(std::stol(v)) - 1,
			SInt(std::stol(t)) - 1,
			SInt(std::stol(n)) - 1
		};
	}

	return f;
}

struct MDef
{
	std::string     name;
	std::list<Face> f;
	Color           cd;
	Texture         td;
};

bool LoadMTL(const std::string &file, std::list<MDef> &m)
{
	std::ifstream fin;
	fin.open(file.c_str());
	if (!fin.is_open()) { return false; }
	std::string in;
	while (fin >> in) {
		if (in == "newmtl") {
			m.push_back(MDef());
			fin >> m.back().name;
			m.back().cd = { 255, 255, 255, Color::BlendMode_Solid };
			m.back().td.Create(1);
			m.back().td.SetColor({0,0}, m.back().cd);
		} else if (in == "map_Kd") {
			std::getline(fin, in);
			in = in.substr(in.find_first_not_of(' '), in.find_last_not_of(' '));
			// TODO: Load texture
		} else if (in == "Kd") {
			Real r = ReadF(fin) * 255;
			Real g = ReadF(fin) * 255;
			Real b = ReadF(fin) * 255;
			m.back().cd = { Byte(SInt(r)), Byte(SInt(g)), Byte(SInt(b)), Color::BlendMode_Solid };
		}
	}
	return true;
}

std::list<Face> *FindMaterial(const std::string &name, std::list<MDef> &m)
{
	for (auto i = m.begin(); i != m.end(); ++i) {
		if (i->name == name) { return &(i->f); }
	}
	return nullptr;
}

bool LoadOBJ(const std::string &file, tiny3d::Array<tiny3d::Vector3> *v, tiny3d::Array<tiny3d::Vector2> *t, tiny3d::Array<tiny3d::Vector3> *n, tiny3d::Array<Material> *m)
{
	size_t dir_sep1 = file.find_last_of('\\');
	size_t dir_sep2 = file.find_last_of('/');
	size_t dir_sep = Max(dir_sep1, dir_sep2);
	if (dir_sep == std::string::npos) {
		dir_sep = Min(dir_sep1, dir_sep2);
	}
	const std::string work_dir = (dir_sep != std::string::npos) ? file.substr(0, dir_sep + 1) : "";

	std::list<Vector3>  vl;
	std::list<Vector2>  tl;
	std::list<Vector3>  nl;
	std::list<MDef>     ml;
	std::list<Face>    *fl = nullptr;

	if (m) {
		ml.push_back(MDef());
		fl = &ml.back().f;
	}

	std::ifstream fin;
	fin.open(file.c_str());
	if (!fin.is_open()) { return false; }
	std::string in;
	while (fin >> in) {
		if (in == "v") {
			Vector3 a = Vector3(ReadF(fin), ReadF(fin), ReadF(fin));
			if (v) vl.push_back(a);
		} else if (in == "n") {
			Vector3 a = Vector3(ReadF(fin), ReadF(fin), ReadF(fin));
			if (n) nl.push_back(a);
		} else if (in == "t") {
			Vector2 a = { ReadF(fin), ReadF(fin) };
			if (t) tl.push_back(a);
		} else if (in == "f") {
			Face a = ReadI(fin);
			if (m && fl) fl->push_back(a);
		} else if (in == "usemtl") {
			fin >> in;
			if (m) {
				fl = FindMaterial(in, ml);
				if (fl == nullptr) { return false; }
			}
		} else if (in == "mtllib") {
			std::getline(fin, in);
			in = in.substr(in.find_first_not_of(' '), in.find_last_not_of(' '));
			if (m) {
				if (!LoadMTL(work_dir + in, ml)) {
					return false;
				}
			}
		}
	}

	if (v) {
		v->Create(UInt(vl.size()));
		auto it = vl.begin();
		for (UInt i = 0; i < v->GetSize(); ++i) {
			(*v)[i] = *it;
			++it;
		}
	}
	if (t) {
		t->Create(UInt(tl.size()));
		auto it = tl.begin();
		for (UInt i = 0; i < t->GetSize(); ++i) {
			(*t)[i] = *it;
			++it;
		}
	}
	if (n) {
		n->Create(UInt(nl.size()));
		auto it = nl.begin();
		for (UInt i = 0; i < n->GetSize(); ++i) {
			(*n)[i] = *it;
			++it;
		}
	}
	if (m) {
		m->Create(UInt(ml.size()));
		auto mit = ml.begin();
		for (UInt i = 0; i < m->GetSize(); ++i) {
			(*m)[i].name = mit->name;
			(*m)[i].cd = mit->cd;
			(*m)[i].td = mit->td;
			(*m)[i].f.Create(UInt(mit->f.size()));
			auto fit = mit->f.begin();
			for (UInt j = 0; j < (*m)[i].f.GetSize(); ++j) {
				(*m)[i].f[j] = *fit;
				++fit;
			}
			++mit;
		}
	}

	return true;
}

void CenterModel(tiny3d::Array<tiny3d::Vector3> &v)
{
	if (v.GetSize() == 0) { return; }

	AABB aabb = { v[0], v[0] };
	for (UInt i = 1; i < v.GetSize(); ++i) {
		aabb.min = Min(aabb.min, v[i]);
		aabb.max = Max(aabb.max, v[i]);
	}

	Vector3 center = aabb.min + ( (aabb.max - aabb.min) / 2 );
	Real    scale = 1 / ( Max(aabb.max.x - aabb.min.x, aabb.max.y - aabb.min.y, aabb.max.z - aabb.min.z) / 4);
	for (UInt i = 0; i < v.GetSize(); ++i) {
		v[i] = (v[i] - center) * scale;
	}
}

tiny3d::Real GetRadius(const tiny3d::Array<tiny3d::Vector3> &v)
{
	Real radius = Real(0);
	for (UInt i = 0; i < v.GetSize(); ++i) {
		radius = Max(radius, Len(v[i]));
	}
	return radius;
}

tiny3d::UInt DrawModel(tiny3d::Image &screen, tiny3d::Array<tiny3d::Real> &zbuf, const tiny3d::Array<tiny3d::Vector3> &v, const tiny3d::Array<tiny3d::Vector3> *n, const tiny3d::Array<Material> &m, Real head_angle, Real z_offset)
{
	Matrix3x3 rot = AxisAngle(Vector3(Real(0), Real(1), Real(0)), head_angle);
	UInt tri = 0;
	Vector3 offset = Vector3(Real(0), Real(0), Real(z_offset) + 1);
	for (UInt i = 0; i < m.GetSize(); ++i) {
		for (UInt j = 0; j < m[i].f.GetSize(); ++j) {
			Face face = m[i].f[j];
			Index i1 = face.i[0];
			for (UInt t = 1; t < face.i.GetSize() - 1; ++t) {
				Index i2 = face.i[t], i3 = face.i[t + 1];

				Vector3 av = v[UInt(i1.v)] * rot + offset;
				Vector3 bv = v[UInt(i2.v)] * rot + offset;
				Vector3 cv = v[UInt(i3.v)] * rot + offset;

				if (av.z < Real(0) || bv.z < Real(0) || cv.z < Real(0)) { continue; }

				Color ac = m[i].cd;
				Color bc = m[i].cd;
				Color cc = m[i].cd;

				if (n && n->GetSize() > 0) {
					Vector3 an = (*n)[UInt(i1.n)] * rot;
					Vector3 bn = (*n)[UInt(i1.n)] * rot;
					Vector3 cn = (*n)[UInt(i1.n)] * rot;
					Real da = Clamp(Real(0.5f), Dot(Vector3(Real(0), Real(0), Real(-1)), an), Real(1));
					Real db = Clamp(Real(0.5f), Dot(Vector3(Real(0), Real(0), Real(-1)), bn), Real(1));
					Real dc = Clamp(Real(0.5f), Dot(Vector3(Real(0), Real(0), Real(-1)), cn), Real(1));
					ac.r = Byte(SInt(SInt(ac.r) * da));
					ac.g = Byte(SInt(SInt(ac.g) * da));
					ac.b = Byte(SInt(SInt(ac.b) * da));
					bc.r = Byte(SInt(SInt(bc.r) * db));
					bc.g = Byte(SInt(SInt(bc.g) * db));
					bc.b = Byte(SInt(SInt(bc.b) * db));
					cc.r = Byte(SInt(SInt(cc.r) * dc));
					cc.g = Byte(SInt(SInt(cc.g) * dc));
					cc.b = Byte(SInt(SInt(cc.b) * dc));
				} else {
					Vector3 sn = Normalize(Cross(cv - av, bv - av));
					Real d = Clamp(Real(0.5f), Dot(Vector3(Real(0), Real(0), Real(-1)), sn), Real(1));
					ac.r = Byte(SInt(SInt(ac.r) * d));
					ac.g = Byte(SInt(SInt(ac.g) * d));
					ac.b = Byte(SInt(SInt(ac.b) * d));
					bc.r = Byte(SInt(SInt(bc.r) * d));
					bc.g = Byte(SInt(SInt(bc.g) * d));
					bc.b = Byte(SInt(SInt(bc.b) * d));
					cc.r = Byte(SInt(SInt(cc.r) * d));
					cc.g = Byte(SInt(SInt(cc.g) * d));
					cc.b = Byte(SInt(SInt(cc.b) * d));
				}

				Vertex a = { Test_ProjectVector(av, screen), Vector2{ Real(0), Real(0) }, ac / av.z };
				Vertex b = { Test_ProjectVector(bv, screen), Vector2{ Real(0), Real(0) }, bc / bv.z };
				Vertex c = { Test_ProjectVector(cv, screen), Vector2{ Real(0), Real(0) }, cc / cv.z };

				if (Test_IsFrontfacing(a.v, b.v, c.v)) {
					DrawTriangle(screen, &zbuf, a, b, c, &m[i].td, nullptr);
					++tri;
				}
			}
		}
	}
	return tri;
}

void Test_Model( void )
{
	struct AnimationFrame
	{
		Array<Vector3>  v;
		Array<Vector2>  t;
		Array<Vector3>  n;
		Array<Material> m; // materials should be identical between frames, but just in case they are not...
	};

	std::cout << "Testing model load and render...";

	const UInt NUM_FRAMES = 20;
	AnimationFrame frames[NUM_FRAMES];
	for (UInt i = 0; i < NUM_FRAMES; ++i) {
		std::ostringstream sout;
		sout << "SPIDBOT/SPIDBOT" << (i + 1) << ".obj";
		TINY3D_ASSERT(LoadOBJ(sout.str(), &frames[i].v, &frames[i].t, &frames[i].n, &frames[i].m));

		// Make cute little LED light blink
		if ((i / 3) % 2 == 0) {
			for (UInt j = 0; j < frames[i].m.GetSize(); ++j) {
				if (frames[i].m[j].name == "LED") {
					frames[i].m[j].cd = { 0, 255, 0, Color::BlendMode_Solid };
					break;
				}
			}
		}
	}

	Image screen;
	screen.Create(tiny3d::UHInt(tiny3d::System::Video::Width() / 4), tiny3d::UHInt(tiny3d::System::Video::Height() / 4));

	TextWriter writer(&screen);
	writer.SetColor({255, 255, 255, Color::BlendMode_Solid});
	writer.EnableShadow();

	for (UInt i = 0; i < NUM_FRAMES; ++i) {
		CenterModel(frames[i].v);
	}

	Real z_offset[NUM_FRAMES];
	for (UInt i = 0; i < NUM_FRAMES; ++i) {
		GetRadius(frames[i].v);
	}

	Array<Real> zbuf(screen.GetWidth() * screen.GetHeight());

	bool      quit  = false;
	bool      pause = false;
	UInt      frame = 0;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				} else if (event.key.keysym.sym == SDLK_SPACE) {
					pause = !pause;
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			}
		}

		Real time = System::Time();
		if (!pause) { frame = UInt(SInt(time * SInt(NUM_FRAMES))) % NUM_FRAMES; }
		UInt tri = DrawModel(screen, zbuf, frames[frame].v, &frames[frame].n, frames[frame].m, time / 4 * Pi(), z_offset[frame]);

		writer.Write("time =").Write(time).Write("\n");
		writer.Write("frame=").Write(SInt(frame)).Write("\n");
		writer.Write("tris =").Write(SInt(tri));
		writer.ResetCaret();

		for (UInt i = 0; i < screen.GetWidth() * screen.GetHeight(); ++i) {
			zbuf[i] = Real::Inf();
		}
		Test_ScreenRefresh(screen);
	}

	std::cout << "completed" << std::endl;
}
