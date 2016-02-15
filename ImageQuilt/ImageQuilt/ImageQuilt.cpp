#include "ImageQuilt.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#define STBI_ONLY_BMP
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <Windows.h>

//#define USE_RGB
//#define USE_HSV
//#define USE_XYZ
#define USE_CIELAB

using std::cout;
using std::endl;
using std::vector;
using std::pair;

ImageQuilt::~ImageQuilt()
{
}

inline void ImageQuilt::fillPatch(Patch * patch, unsigned int inner_w, unsigned int inner_h, unsigned int out_w, unsigned int out_h)
{
#ifdef USE_RGB
	patch->image->rc(inner_w, inner_h, output_image->rc(out_w, out_h));
	patch->image->gc(inner_w, inner_h, output_image->gc(out_w, out_h));
	patch->image->bc(inner_w, inner_h, output_image->bc(out_w, out_h));
#elif defined USE_HSV
	patch->image->h(inner_w, inner_h, output_image->h(out_w, out_h));
	patch->image->s(inner_w, inner_h, output_image->s(out_w, out_h));
	patch->image->v(inner_w, inner_h, output_image->v(out_w, out_h));
#elif defined USE_XYZ
	patch->image->x(inner_w, inner_h, output_image->x(out_w, out_h));
	patch->image->y(inner_w, inner_h, output_image->y(out_w, out_h));
	patch->image->z(inner_w, inner_h, output_image->z(out_w, out_h));
#elif defined USE_CIELAB
	patch->image->l(inner_w, inner_h, output_image->l(out_w, out_h));
	patch->image->a(inner_w, inner_h, output_image->a(out_w, out_h));
	patch->image->b(inner_w, inner_h, output_image->b(out_w, out_h));
#endif
}

inline void ImageQuilt::applyPatch(Patch * patch, unsigned int out_w, unsigned int out_h, unsigned int inner_w, unsigned int inner_h)
{
#ifdef USE_RGB
	output_image->rc(out_w, out_h, patch->image->rc(inner_w, inner_h));
	output_image->gc(out_w, out_h, patch->image->gc(inner_w, inner_h));
	output_image->bc(out_w, out_h, patch->image->bc(inner_w, inner_h));
#elif defined USE_HSV
	output_image->h(out_w, out_h, patch->image->h(inner_w, inner_h));
	output_image->s(out_w, out_h, patch->image->s(inner_w, inner_h));
	output_image->v(out_w, out_h, patch->image->v(inner_w, inner_h));
#elif defined USE_XYZ
	output_image->x(out_w, out_h, patch->image->x(inner_w, inner_h));
	output_image->y(out_w, out_h, patch->image->y(inner_w, inner_h));
	output_image->z(out_w, out_h, patch->image->z(inner_w, inner_h));
#elif defined USE_CIELAB
	output_image->l(out_w, out_h, patch->image->l(inner_w, inner_h));
	output_image->a(out_w, out_h, patch->image->a(inner_w, inner_h));
	output_image->b(out_w, out_h, patch->image->b(inner_w, inner_h));
#endif
}

void ImageQuilt::synthesize()
{
	std::ofstream outFile("output.txt");
	// pick a random patch
	Patch* initial = randomPatch();
	// apply selected patch to output starting at [0,0]
#ifdef USE_HSV
	input_image->rgb2hsv();
	output_image->rgb2hsv();
#elif defined USE_XYZ
	input_image->rgb2xyz();
	output_image->rgb2xyz();
#elif defined USE_CIELAB
	input_image->rgb2xyz();
	input_image->xyz2cielab();
	output_image->rgb2xyz();
	output_image->xyz2cielab();
#endif
	unsigned int line = 0;
	unsigned int col = 0;
	for (unsigned int i = initial->hLocation; i < initial->hLocation + tilesize; i++) {
		for (unsigned int j = initial->wLocation; j < initial->wLocation + tilesize; j++) {
#ifdef USE_RGB
			output_image->rc(col, line, input_image->rc(j, i));
			output_image->gc(col, line, input_image->gc(j, i));
			output_image->bc(col, line, input_image->bc(j, i));
#elif defined USE_HSV
			output_image->h(col, line, input_image->h(j, i));
			output_image->s(col, line, input_image->s(j, i));
			output_image->v(col, line, input_image->v(j, i));
#elif defined USE_XYZ
			output_image->x(col, line, input_image->x(j, i));
			output_image->y(col, line, input_image->y(j, i));
			output_image->z(col, line, input_image->z(j, i));
#elif defined USE_CIELAB
			output_image->l(col, line, input_image->l(j, i));
			output_image->a(col, line, input_image->a(j, i));
			output_image->b(col, line, input_image->b(j, i));
#endif
			col++;
		}
		line++;
		col = 0;
	}
	/*for (auto h = 0; h < tilesize; h++)
	{
		for (auto w = 0; w < tilesize; w++)
		{
			outFile << std::to_string(output_image->rc(w, h)) << "," << std::to_string(output_image->gc(w, h)) << "," << std::to_string(output_image->bc(w, h)) << " ";
		}
		outFile << endl;
	}
	outFile << "HSV" << endl;
	output_image->rgb2hsv();
	for (auto h = 0; h < tilesize; h++)
	{
		for (auto w = 0; w < tilesize; w++)
		{
			outFile << std::to_string(output_image->h(w, h)) << "," << std::to_string(output_image->s(w, h)) << "," << std::to_string(output_image->v(w, h)) << " ";
		}
		outFile << endl;
	}
	outFile << "AFTER" << endl;*/
#ifdef USE_HSV
	output_image->hsv2rgb();
#elif defined USE_XYZ
	output_image->xyz2rgb();
#elif defined USE_CIELAB
	output_image->cielab2xyz();
	output_image->xyz2rgb();
#endif
	/*for (auto h = 0; h < tilesize; h++)
	{
		for (auto w = 0; w < tilesize; w++)
		{
			outFile << std::to_string(output_image->rc(w, h)) << "," << std::to_string(output_image->gc(w, h)) << "," << std::to_string(output_image->bc(w, h)) << " ";
		}
		outFile << endl;
	}*/
	stbi_write_bmp(output_filename.c_str(), output_width, output_height, 3, output_image->data);
	/*Sleep(1000);
	for (auto h = 0; h < output_height; h++)
	{
		for (auto w = 0; w < output_width; w++)
		{
			std::cout << std::to_string(output_image->rc(w, h)) << "\t";
		}
		std::cout << endl;
	}*/
	// total number of tiles to place
	unsigned int total_tiles = num_tiles * num_tiles;
	// start placing them from left to right, top to bottom
	for (unsigned int tile_h = 0; tile_h < num_tiles; tile_h++) {
		for (unsigned int tile_w = 0; tile_w < num_tiles; tile_w++) {
			cout << tile_h*num_tiles + tile_w + 1 << "/" << total_tiles << endl;
			outFile << tile_h*num_tiles + tile_w + 1 << "/" << total_tiles << endl;
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
			bool found_zero = false;
			for (unsigned int img_h = 0; img_h < input_image->height - tilesize; img_h++) {
				for (unsigned int img_w = 0; img_w < input_image->width - tilesize; img_w++) {
					double sum = 0.0;
					if(tile_h == 0 && tile_w > 0)
					{
						for (unsigned int inner_h = 0; inner_h < tilesize; inner_h++) {
							for (unsigned int inner_w = 0; inner_w < overlap; inner_w++) {
								sum += errorCalc(img_w + inner_w, img_h + inner_h, output_w + inner_w, output_h + inner_h);
							}
						}
					}
					else if(tile_h > 0 && tile_w == 0)
					{
						for (unsigned int inner_h = 0; inner_h < overlap; inner_h++) {
							for (unsigned int inner_w = 0; inner_w < tilesize; inner_w++) {
								sum += errorCalc(img_w + inner_w, img_h + inner_h, output_w + inner_w, output_h + inner_h);
							}
						}
					}
					else
					{
						for (unsigned int inner_h = 0; inner_h < overlap; inner_h++) {
							for (unsigned int inner_w = 0; inner_w < tilesize; inner_w++) {
								sum += errorCalc(img_w + inner_w, img_h + inner_h, output_w + inner_w, output_h + inner_h);
							}
						}
						for (unsigned int inner_h = overlap; inner_h < tilesize; inner_h++) {
							for (unsigned int inner_w = 0; inner_w < overlap; inner_w++) {
								sum += errorCalc(img_w + inner_w, img_h + inner_h, output_w + inner_w, output_h + inner_h);
							}
						}
					}
					sum = sqrt(sum);
					if(sum == 0.0)
					{
						distances.clear();
						pair<int, int> location(img_w, img_h);
						pair<pair<int, int>, double> element(location, sum);
						distances.push_back(element);
						found_zero = true;
						cout << "zero" << endl;
						outFile << "zero" << endl;
						break;
					}
					else if (sum <= (1+error)*lowest) {
						if (sum < lowest) {
							lowest = sum;
							cout << "Lowest: " << lowest << endl;
							outFile << "Lowest: " << lowest << endl;
						}
						pair<int, int> location(img_w, img_h);
						pair<pair<int, int>, double> element(location, sum);
						distances.push_back(element);
					}
				}
				if(found_zero)
				{
					break;
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
			double err = distances[distances.size() - randNum - 1].second;
			cout << "Picked: " << randNum << " out of: " << matches << " with error: " << err << " lowest: " << lowest << endl;
			outFile << "Picked: " << randNum << " out of: " << matches << " with error: " << err << " lowest: " << lowest << endl;
			Patch* patch = new Patch(patch_w, patch_h, tilesize, input_image);
#ifdef USE_HSV
			patch->image->rgb2hsv();
#elif defined USE_XYZ
			patch->image->rgb2xyz();
#elif defined USE_CIELAB
			patch->image->rgb2xyz();
			patch->image->xyz2cielab();
#endif
			// time to make the fine cut in the overlap region
			// left overlap
			if (err > 0) {
				if (tile_w > 0)
				{
					// calculate cut path
					auto final_cut = minCut(output_w, output_h, patch_w, patch_h, true);
					// apply it
					for (auto inner_h = 0; inner_h < tilesize; inner_h++)
					{
						for (unsigned int inner_w = 0; inner_w < final_cut->at(inner_h); inner_w++)
						{
							fillPatch(patch, inner_w, inner_h, output_w + inner_w, output_h + inner_h);
						}
					}
					delete final_cut;
				}
				// top overlap
				if (tile_h > 0)
				{
					auto final_cut = minCut(output_w, output_h, patch_w, patch_h, false);
					for (auto inner_w = 0; inner_w < tilesize; inner_w++)
					{
						for (unsigned int inner_h = 0; inner_h < final_cut->at(inner_w); inner_h++)
						{
							fillPatch(patch, inner_w, inner_h, output_w + inner_w, output_h + inner_h);
						}
					}
					delete final_cut;
				}
			}
			// apply patch to output
			for (auto inner_h = 0; inner_h < tilesize; inner_h++)
			{
				for (auto inner_w = 0; inner_w < tilesize; inner_w++)
				{
					applyPatch(patch, output_w + inner_w, output_h + inner_h, inner_w, inner_h);
				}
			}
			delete patch;
			// write to file
#ifdef USE_HSV
			output_image->hsv2rgb();
#elif defined USE_XYZ
			output_image->xyz2rgb();
#elif defined USE_CIELAB
			output_image->cielab2xyz();
			output_image->xyz2rgb();
#endif
			stbi_write_bmp(output_filename.c_str(), output_width, output_height, 3, output_image->data);
		}
	}
	// store the result to .bmp
#ifdef USE_HSV
	output_image->hsv2rgb();
#elif defined USE_XYZ
	output_image->xyz2rgb();
#elif defined USE_CIELAB
	output_image->cielab2xyz();
	output_image->xyz2rgb();
#endif
	stbi_write_bmp(output_filename.c_str(), output_width, output_height, 3, output_image->data);
	// cleanup
	delete input_image;
	delete output_image;
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
	unsigned int w = 0;//rand() % (input_image->width - tilesize);
	unsigned int h = 0;//rand() % (input_image->height - tilesize);
	Patch* p = new Patch(w, h, tilesize, input_image);
	return p;
}

// using L2 Norm
inline double ImageQuilt::errorCalc(unsigned int in_w, unsigned int in_h, unsigned out_w, unsigned int out_h) const
{
#ifdef USE_RGB
	double val = 0.0;
	val += pow(input_image->rc(in_w, in_h) - output_image->rc(out_w, out_h), 2);
	val += pow(input_image->gc(in_w, in_h) - output_image->gc(out_w, out_h), 2);
	val += pow(input_image->bc(in_w, in_h) - output_image->bc(out_w, out_h), 2);
	return val;
#elif defined USE_HSV
	double val = 0.0;
	val += pow(input_image->s(in_w, in_h) - output_image->s(out_w, out_h), 2);
	val += pow(input_image->v(in_w, in_h) - output_image->v(out_w, out_h), 2);
	return val;
#elif defined USE_XYZ
	double val = 0.0;
	val += pow(input_image->x(in_w, in_h) - output_image->x(out_w, out_h), 2);
	val += pow(input_image->y(in_w, in_h) - output_image->y(out_w, out_h), 2);
	val += pow(input_image->z(in_w, in_h) - output_image->z(out_w, out_h), 2);
	return val;
#elif defined USE_CIELAB
	double val = 0.0;
	val += pow(input_image->l(in_w, in_h) - output_image->l(out_w, out_h), 2);
	val += pow(input_image->a(in_w, in_h) - output_image->a(out_w, out_h), 2);
	val += pow(input_image->b(in_w, in_h) - output_image->b(out_w, out_h), 2);
	return val;
#endif
	/*double val = 0.0;
	val += 0.3*output_image->rc(out_w, out_h) * abs(output_image->rc(out_w, out_h) - input_image->rc(in_w, in_h));
	val += 0.59*output_image->gc(out_w, out_h) * abs(output_image->gc(out_w, out_h) - input_image->gc(in_w, in_h));
	val += 0.11*output_image->bc(out_w, out_h) * abs(output_image->bc(out_w, out_h) - input_image->bc(in_w, in_h));
	return val / 3;*/
	//return pow(input_image->v(in_w, in_h) - output_image->v(out_w, out_h),2);
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
			/*double err = 0.3*pow(input_image->rc(patch_w + w, patch_h + h) - output_image->rc(pos_w + w, pos_h + h), 2);
			err += 0.59*pow(input_image->gc(patch_w + w, patch_h + h) - output_image->gc(pos_w + w, pos_h + h), 2);
			err += 0.11*pow(input_image->bc(patch_w + w, patch_h + h) - output_image->bc(pos_w + w, pos_h + h), 2);*/
			overlap_diff[h][w] = sqrt(errorCalc(patch_w + w, patch_h + h, pos_w + w, pos_h + h));
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