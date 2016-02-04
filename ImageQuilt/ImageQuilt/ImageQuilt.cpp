#include "ImageQuilt.h"
#include <vector>
#include <iostream>
#include <queue>
#define STBI_ONLY_BMP
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using std::cout;
using std::endl;
using std::vector;
using std::pair;

ImageQuilt::~ImageQuilt()
{
}

void ImageQuilt::synthesize()
{
	// pick a random patch
	Patch* initial = randomPatch();
	// apply selected patch to output starting at [0,0]
	unsigned int line = 0;
	unsigned int col = 0;
	for (unsigned int i = initial->hLocation; i < initial->hLocation + tilesize; i++) {
		for (unsigned int j = initial->wLocation; j < initial->wLocation + tilesize; j++) {
			output_image->rc(col, line, input_image->rc(j, i));
			output_image->gc(col, line, input_image->gc(j, i));
			output_image->bc(col, line, input_image->bc(j, i));
			col++;
		}
		line++;
		col = 0;
	}

	unsigned int* last_point = new unsigned int[2]; //width, height
	last_point[0] = tilesize - overlap;
	last_point[1] = 0;
	// total number of tiles to place
	unsigned int total_tiles = num_tiles * num_tiles;
	// start placing them from left to right, top to bottom
	for (unsigned int tile_h = 0; tile_h < num_tiles; tile_h++) {
		for (unsigned int tile_w = 0; tile_w < num_tiles; tile_w++) {
			cout << tile_h*num_tiles + tile_w + 1 << "/" << total_tiles << endl;
			// first patch has already been placed
			if (tile_w == 0 && tile_h == 0) {
				continue;
			}
			// coordinates on the output image
			auto output_w = tile_w*tilesize - tile_w*overlap;
			auto output_h = tile_h*tilesize - tile_h*overlap;
			// scan input image 
			// For every location, search the input texture for a set of blocks that satisfy the overlap constraints(above and left) within some error tolerance.
			// Randomly pick one such block.
			double lowest = DBL_MAX;
			vector<pair<pair<int, int>, double>> distances;
			for (unsigned int img_h = 0; img_h < input_image->height - tilesize; img_h++) {
				for (unsigned int img_w = 0; img_w < input_image->width - tilesize; img_w++) {
					double sum = 0.0;
					for (unsigned int inner_h = 0; inner_h < tilesize; inner_h++) {
						for (unsigned int inner_w = 0; inner_w < tilesize; inner_w++) {
							if (inner_w > overlap && inner_h > overlap) {
								continue;
							}
							// L2 Norm with standard luminance
							double val = 0.0;
							val += 0.3*pow(input_image->rc(img_w + inner_w, img_h + inner_h) - output_image->rc(output_w + inner_w, output_h + inner_h), 2);
							val += 0.59*pow(input_image->gc(img_w + inner_w, img_h + inner_h) - output_image->gc(output_w + inner_w, output_h + inner_h), 2);
							val += 0.11*pow(input_image->bc(img_w + inner_w, img_h + inner_h) - output_image->bc(output_w + inner_w, output_h + inner_h), 2);
							sum += sqrt(val);
							/*double val = output_image->rc(output_w + inner_w, output_h + inner_h) * (output_image->rc(output_w + inner_w, output_h + inner_h) - input_image->rc(img_w + inner_w, img_h + inner_h));
							val += output_image->gc(output_w + inner_w, output_h + inner_h) * (output_image->rc(output_w + inner_w, output_h + inner_h) - input_image->rc(img_w + inner_w, img_h + inner_h));
							val += output_image->bc(output_w + inner_w, output_h + inner_h) * (output_image->rc(output_w + inner_w, output_h + inner_h) - input_image->rc(img_w + inner_w, img_h + inner_h));*/
							//val *= val/3;
							//sum += val;
						}
					}
					if (sum <= lowest) {
						lowest = sum;
						pair<int, int> location(img_w, img_h);
						pair<pair<int, int>, double> element(location, sum);
						distances.push_back(element);
					}
					
				}
			}
			// count how many patches are below the cutoff
			auto cutoff = (1 + error)*lowest;
			auto matches = -1;
			for (vector<pair<pair<int, int>, double>>::reverse_iterator it = distances.rbegin(); it != distances.rend(); ++it) {
				if (it->second > cutoff) {
					break;
				}
				matches++;
			}
			// pick one patch
			unsigned int randNum = rand() % (matches - 0 + 1);
			//cout << "Picked: " << randNum << endl;
			unsigned int patch_w = distances[distances.size() - randNum - 1].first.first;
			unsigned int patch_h = distances[distances.size() - randNum - 1].first.second;
			Patch* patch = new Patch(patch_w, patch_h, tilesize, input_image);

			// time to make the fine cut in the overlap region
			// left overlap
			if(tile_w > 0)
			{
				// calculate cut path
				auto final_cut = minCut(output_w, output_h, patch_w, patch_h, true);
				// apply it
				for (auto inner_h = 0; inner_h < tilesize; inner_h++)
				{
					for (unsigned int inner_w = 0; inner_w < final_cut->at(inner_h); inner_w++)
					{
						patch->image->rc(inner_w, inner_h, output_image->rc(output_w + inner_w, output_h + inner_h));
						patch->image->gc(inner_w, inner_h, output_image->gc(output_w + inner_w, output_h + inner_h));
						patch->image->bc(inner_w, inner_h, output_image->bc(output_w + inner_w, output_h + inner_h));
					}
				}
				delete final_cut;
			}
			// top overlap
			if(tile_h > 0)
			{
				auto final_cut = minCut(output_w, output_h, patch_w, patch_h, false);
				for (auto inner_w = 0; inner_w < tilesize; inner_w++)
				{
					for (unsigned int inner_h = 0; inner_h < final_cut->at(inner_w); inner_h++)
					{
						patch->image->rc(inner_w, inner_h, output_image->rc(output_w + inner_w, output_h + inner_h));
						patch->image->gc(inner_w, inner_h, output_image->gc(output_w + inner_w, output_h + inner_h));
						patch->image->bc(inner_w, inner_h, output_image->bc(output_w + inner_w, output_h + inner_h));
					}
				}
				delete final_cut;
			}
			// apply patch to output
			for (auto inner_h = 0; inner_h < tilesize; inner_h++)
			{
				for (auto inner_w = 0; inner_w < tilesize; inner_w++)
				{
					output_image->rc(output_w + inner_w, output_h + inner_h, patch->image->rc(inner_w, inner_h));
					output_image->gc(output_w + inner_w, output_h + inner_h, patch->image->gc(inner_w, inner_h));
					output_image->bc(output_w + inner_w, output_h + inner_h, patch->image->bc(inner_w, inner_h));
				}
			}
			// write to file
			stbi_write_bmp(output_filename.c_str(), output_width, output_height, 3, output_image->data);
		}
	}
	// store the result to .bmp
	stbi_write_bmp(output_filename.c_str(), output_width, output_height, 3, output_image->data);
	// cleanup
	delete input_image;
	delete output_image;
	delete last_point;
}

Image* ImageQuilt::get_output() const
{
	return output_image;
}

void ImageQuilt::loadImage()
{
	int x, y, n;
	uint8_t* data = stbi_load(input_filename.c_str(), &x, &y, &n, 0);
	assert(data);
	input_image = new Image(x, y, data);
}

Patch * ImageQuilt::randomPatch()
{
	unsigned int w = rand() % (input_image->width - tilesize);
	unsigned int h = rand() % (input_image->height - tilesize);
	Patch* p = new Patch(w, h, tilesize, input_image);
	return p;
}

vector<int>* ImageQuilt::minCut(unsigned int pos_w, unsigned int pos_h, unsigned int patch_w, unsigned int patch_h, const bool left)
{
	unsigned int vertical, horizontal;
	if(left)
	{
		vertical = tilesize;
		horizontal = overlap;
	}
	else
	{
		vertical = overlap;
		horizontal = tilesize;
	}

	int** dis = new int*[vertical];
	bool** visited = new bool*[vertical];
	double** overlap_diff = new double*[vertical];
	for (auto h = 0; h < vertical; h++)
	{
		dis[h] = new int[horizontal];
		visited[h] = new bool[horizontal];
		overlap_diff[h] = new double[horizontal];
		for (auto w = 0; w < horizontal; w++)
		{
			dis[h][w] = INT_MAX;
			visited[h][w] = false;
			double err = 0.3*pow(input_image->rc(patch_w + w, patch_h + h) - output_image->rc(pos_w + w, pos_h + h), 2);
			err += 0.59*pow(input_image->gc(patch_w + w, patch_h + h) - output_image->gc(pos_w + w, pos_h + h), 2);
			err += 0.11*pow(input_image->bc(patch_w + w, patch_h + h) - output_image->bc(pos_w + w, pos_h + h), 2);
			overlap_diff[h][w] = sqrt(err);
		}
	}
	typedef pair<pair<int, int>, vector<int>*> tt;
	std::priority_queue< pair<tt, double>, vector<pair<tt, double>>, Prioritize<tt> > pq;
	vector<int>* final_cut(0);
	if (left)
	{
		for (auto w = 0; w < horizontal; w++)
		{
			pq.push(std::make_pair(std::make_pair(std::make_pair(w, 0), new vector<int>(1, w)), overlap_diff[0][w]));
		}
		while (!pq.empty())
		{
			auto current = pq.top(); //Current vertex. The shortest distance for this has been found
			pq.pop();
			auto current_w = current.first.first.first;
			auto current_h = current.first.first.second;
			vector<int>* current_vector = current.first.second;
			auto current_error = current.second;
			if (current_h == vertical - 1)
			{
				// found the best
				final_cut = current_vector;
				break;
			}
			if (visited[current_h][current_w])
			{
				delete current_vector;
				continue;
			}
			visited[current_h][current_w] = true;
			auto start = (current_w - 1) >= 0 ? (current_w - 1) : 0;
			auto end = (current_w + 1) < horizontal ? (current_w + 1) : current_w;
			for (auto i = start;i <= end;i++)
			{
				if (!visited[current_h + 1][i] && overlap_diff[current_h + 1][i] + current_error < dis[current_h + 1][i])
				{
					auto entry = std::make_pair(std::make_pair(i, current_h + 1), new vector<int>(current_vector->begin(), current_vector->end()));
					entry.second->push_back(i);
					pq.push(std::make_pair(entry, dis[current_h + 1][i] = overlap_diff[current_h + 1][i] + current_error));
				}
			}
			delete current_vector;
		}
	}
	else
	{
		for (auto h = 0; h < vertical; h++)
		{
			pq.push(std::make_pair(std::make_pair(std::make_pair(0, h), new vector<int>(1, h)), overlap_diff[h][0]));
		}
		while (!pq.empty())
		{
			auto current = pq.top(); //Current vertex. The shortest distance for this has been found
			pq.pop();
			auto current_w = current.first.first.first;
			auto current_h = current.first.first.second;
			vector<int>* current_vector = current.first.second;
			auto current_error = current.second;
			if (current_w == horizontal - 1)
			{
				// found the best
				final_cut = current_vector;
				break;
			}
			if (visited[current_h][current_w])
			{
				delete current_vector;
				continue;
			}
			visited[current_h][current_w] = true;
			auto start = (current_h - 1) >= 0 ? (current_h - 1) : 0;
			auto end = (current_h + 1) <= vertical ? (current_h + 1) : current_h;
			for (auto i = start;i < end;i++)
			{
				if (!visited[i][current_w + 1] && overlap_diff[i][current_w + 1] + current_error < dis[i][current_w + 1])
				{
					auto entry = std::make_pair(std::make_pair(current_w + 1, i), new vector<int>(current_vector->begin(), current_vector->end()));
					entry.second->push_back(i);
					pq.push(std::make_pair(entry, dis[i][current_w + 1] = overlap_diff[i][current_w + 1] + current_error));
				}
			}
			delete current_vector;
		}
	}
	// cleanup
	for (auto h = 0; h < vertical; h++)
	{
		delete dis[h];
		delete visited[h];
		delete overlap_diff[h];
	}
	delete dis, visited, overlap_diff;
	return final_cut;
}