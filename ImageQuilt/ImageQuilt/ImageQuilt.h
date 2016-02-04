#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "custom_types.h"

using std::string;
class ImageQuilt
{
public:
	ImageQuilt(const string& _input_filename, const string& _output_folder, unsigned _tilesize, unsigned _num_tiles, unsigned _overlap, double _error)
		: input_filename(_input_filename),
		  tilesize(_tilesize),
		  num_tiles(_num_tiles),
		  overlap(_overlap),
		  error(_error)
	{
		if (overlap == 0)
		{
			overlap = static_cast<unsigned int>(round(tilesize / 6));
		}
		if(error == 0)
		{
			error = 0.1;
		}
		output_filename = _output_folder + "output-" + std::to_string(tilesize) + "-" + std::to_string(num_tiles) + "-" + std::to_string(overlap) + ".bmp";
		output_width = num_tiles * tilesize - ((num_tiles - 1) * overlap);
		output_height = num_tiles * tilesize - ((num_tiles - 1) * overlap);
		output_image = new Image(output_width, output_height);
		loadImage();
	}
	~ImageQuilt();
	void synthesize();
	Image* get_output() const;
private:
	void loadImage();
	Patch* randomPatch();
	std::vector<int>* minCut(unsigned int pos_w, unsigned int pos_h, unsigned int patch_w, unsigned int patch_h, const bool left);

	string input_filename;
	string output_filename;
	unsigned int tilesize;
	unsigned int num_tiles;
	unsigned int output_width;
	unsigned int output_height;
	unsigned int overlap;
	double error;
	Image* input_image;
	Image* output_image;
};

