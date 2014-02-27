#ifndef FRAME_H
#define FRAME_H

#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath> 

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
		void matrix_mult(float angle, std::vector<point> inp_mat);
		point *calc_centroid(std::vector<point> neutral_marks);
		void create_brow_landmarks();
		void add_translation(point trans, std::vector<point> matrix);
		std::vector<float> get_dist(std::vector<point> lndmarks);

	public: 
		std::vector<point> r_eyebrow_landmks;
		std::vector<point> l_eyebrow_landmks; 
		point l_centroid;
		point r_centroid;
		std::vector<point> landmarks;
		std::vector<point> left_feats;
		std::vector<point> right_feats;
		
		frame();
		frame(std::vector<int> input_file, int start_row=0);
		void rotate();
		std::vector<float> extract_eyebrow_feat(point neutral_ctrd, std::vector<point> lndmarks);
		void calc_centroids();
		//std::vector<point> get_l_eye_feat(std::vecotr<point> peek_left_feat);
		//std::vector<point> get_r_eye_feat(std::vecotr<point> peek_right_feat);




};

#endif