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
		void matrix_mult(float angle, std::vector<point> &inp_mat);
		void create_brow_landmarks();
		void add_translation(point trans, std::vector<point> &matrix);
		void extract_left_eyebrow_feat(point neutral_ctrd);
		void extract_right_eyebrow_feat(point neutral_ctrd);
		point *calc_centroid(std::vector<point> neutral_marks);
		std::vector<float> get_feats(std::vector<point> const &lndmarks);
		static float get_probability(float feat, float mean, float std, float precision);
		static std::vector<float> calc_eye_feat_ratio(std::vector<float> peek_feat, std::vector<float> neutral_feat);

	public: 
		std::vector<point> r_eyebrow_landmks; // Holds the INDICIES of the right eyebrow landmarks
		std::vector<point> l_eyebrow_landmks; // Holds the INDICIES of the left eyebrow landmarks
		std::vector<point> landmarks;
		std::vector<float> left_feats;
		std::vector<float> right_feats;
		point l_centroid;
		point r_centroid;
		
		frame();
		frame(std::vector<int> input_file, int start_row=0);
		void rotate();
		void calc_centroids();
		void extract_eyebrow_feat(frame *neutral_frame);
		static std::vector<float> calc_probs(std::vector<float> model_vals, std::vector<float> feat_vals);
		static std::vector<float> eye_feat_ratio(frame *peak_frame, frame *neutral_frame);
	};

#endif