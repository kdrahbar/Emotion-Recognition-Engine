#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <opencv2/core/core.hpp>

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
		std::vector<point> matrix_mult(float angle, std::vector<point> inp_mat);
		std::vector<point> add_translation(point trans, std::vector<point> matrix);
		point *calc_centroid(std::vector<point> neutral_marks);
		std::vector<float> get_dist(std::vector<point> lndmarks);
		static float get_probability(float feat, float mean, float std);

	public:
		std::vector<point> r_eye_region_landmks; 
		std::vector<point> l_eye_region_landmks;
		std::vector<point> landmarks;
		std::vector<float> l_eyebrow_feats;
		std::vector<float> r_eyebrow_feats;
		std::vector<float> l_eye_feats;
		std::vector<float> r_eye_feats;
		point l_centroid;
		point r_centroid;

		frame();
		frame(std::vector<int> input_file, int start_row=0);
		frame(cv::Mat cvLandmarks);
		void rotate();
		void calc_centroids();
		std::vector<float> extract_eyebrow_feat(point neutral_ctrd, std::vector<point> lndmarks);
		std::vector<float> extract_eye_feat(point neutral_ctrd, std::vector<point> lndmarks);
		void extract_eye_region_landmarks();
		static std::vector<float> calc_probs(std::vector<float> model_vals, std::vector<float> feat_vals);
		static std::vector<float> eyebrow_feat_ratio(std::vector<float> peek_feat, std::vector<float> neutral_feat);
		static std::vector<float> eye_feat_ratio(std::vector<float> peek_feat, std::vector<float> neutral_feat);
	};