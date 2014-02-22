#ifndef FRAME_H
#define FRAME_H

#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

struct point 
{
	float x;
	float y;
};

class frame
{
	
	private:
		void matrix_mult(float angle);
	public: 
		int r_eyebrow_landmks [11];
		int l_eyebrow_landmks [11]; 
		point l_centroid;
		point r_centroid;
		std::vector<point> landmarks;
		std::vector<point> left_feats;
		std::vector<point> right_feats;
		
		frame();
		frame(std::vector<int> input_file, int start_row=0);
		void rotate(char side);
		void extract_eyebrow_feat(char side);
		void calc_centroids();


};

#endif