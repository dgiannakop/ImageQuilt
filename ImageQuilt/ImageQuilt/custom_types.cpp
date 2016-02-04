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