#include "custom_types.h"
#include <stdlib.h>
#include <vector>

Image::Image(unsigned int _w, unsigned int _h)
{
	width = _w;
	height = _h;
	data = new uint8_t[3 * width*height];
	for (unsigned int i = 0; i < 3 * width * height; i++) {
		data[i] = 0;
	}
}

Image::~Image()
{
	delete data;
	delete hsv_data;
	delete xyz_data;
	delete cielab_data;
}

void Image::rgb2hsv()
{
	if (hsv_data == nullptr)
		hsv_data = new precision[3 * width * height];
	for (unsigned int h = 0; h < height; h++)
	{
		for (unsigned int w = 0; w < width; w++)
		{
			auto rc = static_cast<precision>(this->rc(w, h)) / 255;
			auto gc = static_cast<precision>(this->gc(w, h)) / 255;
			auto bc = static_cast<precision>(this->bc(w, h)) / 255;
			precision hue, sat, value;
			rgb2hsv(rc, gc, bc, &hue, &sat, &value);
			this->h(w, h, hue);
			this->s(w, h, sat);
			this->v(w, h, value);
		}
	}
}
void Image::rgb2hsv(precision r, precision g, precision b, precision* h, precision* s, precision* v)
{
	precision min, max, delta;
	//min = MIN(r, g, b);
	min = r < g ? r : g;
	min = min < b ? min : b;

	//max = MAX(r, g, b);
	max = r > g ? r : g;
	max = max > b ? max : b;

	*v = max;				// v
	delta = max - min;
	if (max != 0)
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	if (r == max)
		*h = (g - b) / delta;		// between yellow & magenta
	else if (g == max)
		*h = 2 + (b - r) / delta;	// between cyan & yellow
	else
		*h = 4 + (r - g) / delta;	// between magenta & cyan
	*h *= 60;				// degrees
	if (*h < 0)
		*h += 360;
}

void Image::hsv2rgb()
{
	for (unsigned int h = 0; h < height; h++)
	{
		for (unsigned int w = 0; w < width; w++)
		{
			auto hue = this->h(w, h);
			auto sat = this->s(w, h);
			auto val = this->v(w, h);
			precision rc, gc, bc;
			hsv2rgb(&rc, &gc, &bc, hue, sat, val);
			this->rc(w, h, static_cast<uint8_t>(round(rc * 255)));
			this->gc(w, h, static_cast<uint8_t>(round(gc * 255)));
			this->bc(w, h, static_cast<uint8_t>(round(bc * 255)));
		}
	}
}
void Image::hsv2rgb(precision *r, precision *g, precision *b, precision h, precision s, precision v)
{
	int i;
	precision f, p, q, t;
	if (s == 0) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = floor(h);
	f = h - i;			// factorial part of h
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	switch (i) {
	case 0:
		*r = v;
		*g = t;
		*b = p;
		break;
	case 1:
		*r = q;
		*g = v;
		*b = p;
		break;
	case 2:
		*r = p;
		*g = v;
		*b = t;
		break;
	case 3:
		*r = p;
		*g = q;
		*b = v;
		break;
	case 4:
		*r = t;
		*g = p;
		*b = v;
		break;
	default:		// case 5:
		*r = v;
		*g = p;
		*b = q;
		break;
	}
}

void Image::rgb2xyz()
{
	if (xyz_data == nullptr)
		xyz_data = new precision[3 * width * height];
	for (unsigned int h = 0; h < height; h++)
	{
		for (unsigned int w = 0; w < width; w++)
		{
			auto rc = this->rc(w, h);
			auto gc = this->gc(w, h);
			auto bc = this->bc(w, h);
			precision x, y, z;
			rgb2xyz(rc, gc, bc, &x, &y, &z);
			this->x(w, h, x);
			this->y(w, h, y);
			this->z(w, h, z);
		}
	}
}
void Image::rgb2xyz(precision r, precision g, precision b, precision* x, precision* y, precision* z)
{
	r = r / 255.0;
	g = g / 255.0;
	b = b / 255.0;
	if (r > 0.04045f) {
		r = powf(((r + 0.055f) / 1.055f), 2.4f);
	}
	else {
		r = r / 12.92f;
	}

	if (g > 0.04045) {
		g = powf(((g + 0.055f) / 1.055f), 2.4f);
	}
	else {
		g = g / 12.92f;
	}

	if (b > 0.04045f) {
		b = powf(((b + 0.055f) / 1.055f), 2.4f);
	}
	else {
		b = b / 12.92f;
	}

	r = r * 100;
	g = g * 100;
	b = b * 100;

	//Observer. = 2°, Illuminant = D65
	*x = r * 0.4124f + g * 0.3576f + b * 0.1805f;
	*y = r * 0.2126f + g * 0.7152f + b * 0.0722f;
	*z = r * 0.0193f + g * 0.1192f + b * 0.9505f;
}

void Image::xyz2rgb()
{
	for (unsigned int h = 0; h < height; h++)
	{
		for (unsigned int w = 0; w < width; w++)
		{
			auto x = this->x(w, h);
			auto y = this->y(w, h);
			auto z = this->z(w, h);
			precision rc, gc, bc;
			xyz2rgb(&rc, &gc, &bc, x, y, z);
			this->rc(w, h, static_cast<uint8_t>(rc));
			this->gc(w, h, static_cast<uint8_t>(gc));
			this->bc(w, h, static_cast<uint8_t>(bc));
		}
	}
}
void Image::xyz2rgb(precision* r, precision* g, precision* b, precision x, precision y, precision z)
{
	x = x / 100;       //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
	y = y / 100;        //Y from 0 to 100.000
	z = z / 100;        //Z from 0 to 108.883

	*r = x *  3.2406 + y * -1.5372 + z * -0.4986;
	*g = x * -0.9689 + y *  1.8758 + z *  0.0415;
	*b = x *  0.0557 + y * -0.2040 + z *  1.0570;

	if (*r > 0.0031308)
		*r = 1.055 * (powf(*r,(1 / 2.4))) - 0.055;
	else
		*r = 12.92 * (*r);
	if (*g > 0.0031308)
		*g = 1.055 * (powf(*g, (1 / 2.4))) - 0.055;
	else
		*g = 12.92 * (*g);
	if (*b > 0.0031308)
		*b = 1.055 * (powf(*b, (1 / 2.4))) - 0.055;
	else
		*b = 12.92 * (*b);

	*r = round(*r * 255);
	*g = round(*g * 255);
	*b = round(*b * 255);
}

void Image::xyz2cielab()
{
	if (cielab_data == nullptr)
		cielab_data = new precision[3 * width * height];
	for (unsigned int h = 0; h < height; h++)
	{
		for (unsigned int w = 0; w < width; w++)
		{
			auto x = this->x(w, h);
			auto y = this->y(w, h);
			auto z = this->z(w, h);
			precision l, a, b;
			xyz2cielab(x, y, z, &l, &a, &b);
			this->l(w, h, l);
			this->a(w, h, a);
			this->b(w, h, b);
		}
	}
}
void Image::xyz2cielab(precision x, precision y, precision z, precision* l, precision* a, precision* b)
{
	precision ref_X = 95.047;
	precision ref_Y = 100.000;
	precision ref_Z = 108.883;
	x = x / ref_X;
	y = y / ref_Y;
	z = z / ref_Z;

	if (x > 0.008856)
		x = powf(x, (1.0 / 3.0));
	else
		x = (7.787 * x) + (16.0 / 116.0);
	if (y > 0.008856)
		y = powf(y, (1.0 / 3.0));
	else
		y = (7.787 * y) + (16.0 / 116.0);
	if (z > 0.008856)
		z = powf(z, (1.0 / 3.0));
	else
		z = (7.787 * z) + (16.0 / 116.0);

	*l = (116 * y) - 16;
	*a = 500 * (x - y);
	*b = 200 * (y - z);
}

void Image::cielab2xyz()
{
	if (xyz_data == nullptr)
		xyz_data = new precision[3 * width * height];
	for (unsigned int h = 0; h < height; h++)
	{
		for (unsigned int w = 0; w < width; w++)
		{
			auto l = this->l(w, h);
			auto a = this->a(w, h);
			auto b = this->b(w, h);
			precision x, y, z;
			cielab2xyz(&x, &y, &z, l, a, b);
			this->x(w, h, x);
			this->y(w, h, y);
			this->z(w, h, z);
		}
	}
}
void Image::cielab2xyz(precision* x, precision* y, precision* z, precision l, precision a, precision b)
{
	precision ref_X = 95.047;
	precision ref_Y = 100.000;
	precision ref_Z = 108.883;
	double e = 0.008856;
	double k = 903.3;

	*y = (l + 16) / 116.0;
	*x = a / 500 + (*y);
	*z = (*y) - b / 200;

	if (powf(*y, 3) > e)
		*y = powf(*y, 3);
	else
		*y = ((*y) * 116 - 16) / k;
	if (powf(*x, 3) > e)
		*x = powf(*x, 3);
	else
		*x = ((*x)*116 - 16) / k;
	if (powf(*z, 3) > e)
		*z = powf(*z, 3);
	else
		*z = ((*z)*116 - 16) / k;

	*x = ref_X * (*x);     //ref_X =  95.047     Observer= 2°, Illuminant= D65
	*y = ref_Y * (*y);     //ref_Y = 100.000
	*z = ref_Z * (*z);    //ref_Z = 108.883
}

void Image::rgb2cielab()
{
	this->rgb2xyz();
	this->xyz2cielab();
}
void Image::cielab2rgb()
{
	this->cielab2xyz();
	this->xyz2rgb();
}

uint8_t Image::rc(unsigned int w, unsigned int h) const
{
	return data[3 * width * h + 3 * w];
}

uint8_t Image::gc(unsigned int w, unsigned int h) const
{
	return data[3 * width * h + 3 * w + 1];
}

uint8_t Image::bc(unsigned int w, unsigned int h) const
{
	return data[3 * width * h + 3 * w + 2];
}

Image::precision Image::h(unsigned int w, unsigned int h) const
{
	return hsv_data[3 * width * h + 3 * w];
}

Image::precision Image::s(unsigned int w, unsigned int h) const
{
	return hsv_data[3 * width * h + 3 * w + 1];
}

Image::precision Image::v(unsigned int w, unsigned int h) const
{
	return hsv_data[3 * width * h + 3 * w + 2];
}

Image::precision Image::x(unsigned int w, unsigned int h) const
{
	return xyz_data[3 * width * h + 3 * w];
}

Image::precision Image::y(unsigned int w, unsigned int h) const
{
	return xyz_data[3 * width * h + 3 * w + 1];
}

Image::precision Image::z(unsigned int w, unsigned int h) const
{
	return xyz_data[3 * width * h + 3 * w + 2];
}

Image::precision Image::l(unsigned int w, unsigned int h) const
{
	return cielab_data[3 * width * h + 3 * w];
}

Image::precision Image::a(unsigned int w, unsigned int h) const
{
	return cielab_data[3 * width * h + 3 * w + 1];
}

Image::precision Image::b(unsigned int w, unsigned int h) const
{
	return cielab_data[3 * width * h + 3 * w + 2];
}

void Image::rc(unsigned int w, unsigned int h, uint8_t value)
{
	data[3 * width * h + 3 * w] = value;
}

void Image::gc(unsigned int w, unsigned int h, uint8_t value)
{
	data[3 * width * h + 3 * w + 1] = value;
}

void Image::bc(unsigned int w, unsigned int h, uint8_t value)
{
	data[3 * width * h + 3 * w + 2] = value;
}

void Image::h(unsigned int w, unsigned int h, precision value)
{
	hsv_data[3 * width * h + 3 * w + 0] = value;
}

void Image::s(unsigned int w, unsigned int h, precision value)
{
	hsv_data[3 * width * h + 3 * w + 1] = value;
}

void Image::v(unsigned int w, unsigned int h, precision value)
{
	hsv_data[3 * width * h + 3 * w + 2] = value;
}

void Image::x(unsigned int w, unsigned int h, precision value)
{
	xyz_data[3 * width * h + 3 * w + 0] = value;
}

void Image::y(unsigned int w, unsigned int h, precision value)
{
	xyz_data[3 * width * h + 3 * w + 1] = value;
}

void Image::z(unsigned int w, unsigned int h, precision value)
{
	xyz_data[3 * width * h + 3 * w + 2] = value;
}

void Image::l(unsigned int w, unsigned int h, precision value)
{
	cielab_data[3 * width * h + 3 * w + 0] = value;
}

void Image::a(unsigned int w, unsigned int h, precision value)
{
	cielab_data[3 * width * h + 3 * w + 1] = value;
}

void Image::b(unsigned int w, unsigned int h, precision value)
{
	cielab_data[3 * width * h + 3 * w + 2] = value;
}
