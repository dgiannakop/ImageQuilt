#pragma once
#include <vector>
#include <string>
#include <stdint.h>


class Image {
public:
	typedef double precision;

	unsigned int width;
	unsigned int height;
	uint8_t* data;
	precision* hsv_data;
	precision* xyz_data;
	precision* cielab_data;

	Image(unsigned int _w, unsigned int _h);
	Image(unsigned int _w, unsigned int _h, uint8_t* _data) : width(_w), height(_h), data(_data) {}
	~Image();
	void rgb2hsv();
	static void rgb2hsv(precision r, precision g, precision b, precision* h, precision* s, precision* v);
	void hsv2rgb();
	static void hsv2rgb(precision *r, precision *g, precision *b, precision h, precision s, precision v);
	void rgb2xyz();
	static void rgb2xyz(precision r, precision g, precision b, precision* x, precision* y, precision* z);
	void xyz2rgb();
	static void xyz2rgb(precision* r, precision* g, precision* b, precision x, precision y, precision z);
	void xyz2cielab();
	static void xyz2cielab(precision x, precision y, precision z, precision* l, precision* a, precision* b);
	void cielab2xyz();
	static void cielab2xyz(precision* x, precision* y, precision* z, precision l, precision a, precision b);

	uint8_t rc(unsigned int w, unsigned int h) const;
	uint8_t gc(unsigned int w, unsigned int h) const;
	uint8_t bc(unsigned int w, unsigned int h) const;

	precision h(unsigned int w, unsigned int h) const;
	precision s(unsigned int w, unsigned int h) const;
	precision v(unsigned int w, unsigned int h) const;

	precision x(unsigned int w, unsigned int h) const;
	precision y(unsigned int w, unsigned int h) const;
	precision z(unsigned int w, unsigned int h) const;

	precision l(unsigned int w, unsigned int h) const;
	precision a(unsigned int w, unsigned int h) const;
	precision b(unsigned int w, unsigned int h) const;

	void rc(unsigned int w, unsigned int h, uint8_t value);
	void gc(unsigned int w, unsigned int h, uint8_t value);
	void bc(unsigned int w, unsigned int h, uint8_t value);

	void h(unsigned int w, unsigned int h, precision value);
	void s(unsigned int w, unsigned int h, precision value);
	void v(unsigned int w, unsigned int h, precision value);

	void x(unsigned int w, unsigned int h, precision value);
	void y(unsigned int w, unsigned int h, precision value);
	void z(unsigned int w, unsigned int h, precision value);

	void l(unsigned int w, unsigned int h, precision value);
	void a(unsigned int w, unsigned int h, precision value);
	void b(unsigned int w, unsigned int h, precision value);
};

class Texture {
public:
	uint8_t* rc;
	uint8_t* gc;
	uint8_t* bc;
	unsigned int width;
	unsigned int height;

	Texture(unsigned int w, unsigned int h) {
		rc = new uint8_t[w*h];
		gc = new uint8_t[w*h];
		bc = new uint8_t[w*h];
		width = w;
		height = h;
	}
	~Texture() {
		delete rc;
		delete gc;
		delete bc;
	}
};

class Patch {
public:
	unsigned int wLocation;
	unsigned int hLocation;
	unsigned int tilesize;
	double error;
	Image* image;
	Patch(unsigned int _w, unsigned int _h, unsigned int _tilesize, Image* source) : wLocation(_w), hLocation(_h), tilesize(_tilesize), error(DBL_MAX)
	{
		image = new Image(tilesize, tilesize);
		for (auto h = 0; h < tilesize; h++)
		{
			for (auto w = 0; w < tilesize; w++)
			{
				image->rc(w, h, source->rc(wLocation + w, hLocation + h));
				image->gc(w, h, source->gc(wLocation + w, hLocation + h));
				image->bc(w, h, source->bc(wLocation + w, hLocation + h));
			}
		}
	}
	~Patch()
	{
		delete image;
	}
};

template<class T>
class Prioritize
{
public:
	bool operator ()(std::pair<T,double>&p1, std::pair<T, double>&p2)
	{
		return p1.second > p2.second;
	}
};