#include "frame.h"
#define dist_between(p1, p2) sqrt( pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2) )
#define ratio(p1, p2) (p1 - p2)/p2

//Helper function to multiply rotation matrix by landmark matrix
void frame::matrix_mult(float angle)
{
	for (std::vector<point>::iterator it = landmarks.begin() ; it != landmarks.end(); ++it)
	{
		it->x = (it->x * cos(-angle)) + (it->y * sin(-angle));
		it->y = (it->x * sin(-angle)) + (it->y * cos(-angle));
	}
}

void frame::matrix_mult(float angle, std::vector<point> &inp_mat)
{
	for (std::vector<point>::iterator it = inp_mat.begin() ; it != inp_mat.end(); ++it)
	{
		it->x = (it->x * cos(-angle)) + (it->y * -sin(-angle));
		it->y = (it->x * sin(-angle)) + (it->y * cos(-angle));
	}
}

point* frame::calc_centroid(std::vector<point> neutral_marks)
{
	int x_sum=0, y_sum=0;
	for (int i = 5; i < 11; i++) 
	{
		x_sum += neutral_marks[i].x;
		y_sum += neutral_marks[i].y;
	}
	point *centroid = new point();
	centroid->x = x_sum/6;
	centroid->y = y_sum/6;
	return centroid;
}

void frame::calc_centroids()
{
	int rx_sum=0, ry_sum=0, lx_sum=0, ly_sum=0;
	for (int i = 5; i < 11; i++) 
	{
		lx_sum += l_eyebrow_landmks[i].x;
		ly_sum += l_eyebrow_landmks[i].y;
		rx_sum += r_eyebrow_landmks[i].x;
		ry_sum += r_eyebrow_landmks[i].y;
	}
	l_centroid.x = lx_sum/6;
	l_centroid.y = ly_sum/6;
	r_centroid.x = rx_sum/6;
	r_centroid.y = ry_sum/6;

}

// Used by constructor to create the indicie vectors corresponding to the l and r landmarks
void frame::create_brow_landmarks()
{
	int r_tmp [11] = {5,6,7,8,9,25,26,27,28,29,30};
	int l_tmp [11] = {0,1,2,3,4,19,20,21,22,23,24};
	for (int i = 0; i < 11; i++)
	{
		l_eyebrow_landmks.push_back(landmarks[l_tmp[i]]);
		r_eyebrow_landmks.push_back(landmarks[r_tmp[i]]);
	}
}

void frame::add_translation(point trans, std::vector<point> &matrix)
{
	for (std::vector<point>::iterator it = matrix.begin() ; it != matrix.end(); ++it)
	{
		it->x += trans.x;
		it->y += trans.y;
	}
}

// Need a better value for pi
// Used by Extract eyebrow feat to calculate L1, L2, L3, L4, A1, A2
std::vector<float> frame::get_feats(std::vector<point> const &lndmarks)
{
	std::vector<float> feat_point;
	feat_point.push_back( dist_between(lndmarks[5], lndmarks[0]) );
	feat_point.push_back( dist_between(lndmarks[8], lndmarks[4]) );
	float a1 = atan( (lndmarks[0].y - lndmarks[5].y)/ (lndmarks[0].x - lndmarks[5].x) ) *180/3.14;
	if (a1 < 0)
		a1 += 180;
	feat_point.push_back(a1);
	float a2 = atan( (lndmarks[4].y - lndmarks[8].y)/ (lndmarks[4].x - lndmarks[8].x) )*180/3.14;
	if (a2 < 0)
		a2 += 180;
	feat_point.push_back(a2);
	feat_point.push_back( dist_between(lndmarks[5], lndmarks[8]) );
	// Calculate L4
	float x_avg, y_avg;
	x_avg = pow( ((lndmarks[9].x + lndmarks[10].x)/2 - (lndmarks[6].x + lndmarks[7].x)/2), 2);
	y_avg = pow( ((lndmarks[9].y + lndmarks[10].y)/2 - (lndmarks[6].y + lndmarks[7].y)/2), 2);
	feat_point.push_back( sqrt(x_avg + y_avg) );
	return feat_point;
}


// Constructor that builds the landmark vector given a starting row (from a file)
frame::frame(std::vector<int> input_file, int start_row)
{
	int start_index = start_row*98;
	point add_point;
	for( int i = start_index; i < (start_index + 98); i+=2 )
	{
		add_point.x = input_file[i];
		add_point.y = -input_file[i+1];
		landmarks.push_back(add_point);
	}
	create_brow_landmarks();
}

// Rotate feature matrix to center it
void frame::rotate()
{
	float angle;
	angle = atan( (landmarks[4].y - landmarks[5].y)/(landmarks[4].x - landmarks[5].x) );
	matrix_mult(angle);
}

// Extract the eyebrow features given a neutral centroid and a vector of landmarks
void frame::extract_left_eyebrow_feat(point neutral_ctrd)
{
	float angle = atan( (l_eyebrow_landmks[5].y - l_eyebrow_landmks[8].y)/(l_eyebrow_landmks[5].x - l_eyebrow_landmks[8].x) );
	matrix_mult(angle, l_eyebrow_landmks);
	
	point translate;
	translate.x = neutral_ctrd.x - l_centroid.x;
	translate.y = neutral_ctrd.y - l_centroid.y;
	add_translation(translate, l_eyebrow_landmks);
	left_feats = get_feats(l_eyebrow_landmks);
}

void frame::extract_right_eyebrow_feat(point neutral_ctrd)
{
	float angle = atan( (r_eyebrow_landmks[5].y - r_eyebrow_landmks[8].y)/(r_eyebrow_landmks[5].x - r_eyebrow_landmks[8].x) );
	matrix_mult(angle, r_eyebrow_landmks);
	
	point translate;
	translate.x = neutral_ctrd.x - r_centroid.x;
	translate.y = neutral_ctrd.y - r_centroid.y;
	add_translation(translate, r_eyebrow_landmks);
	right_feats = get_feats(r_eyebrow_landmks);
}

void frame::extract_eyebrow_feat(frame *neutral_frame)
{
	extract_left_eyebrow_feat(neutral_frame->l_centroid);
	extract_right_eyebrow_feat(neutral_frame->r_centroid);
}

// Helper func used by eye_feat_ratio. Calculate the ratio between the neutral and peek frames. 
std::vector<float> frame::calc_eye_feat_ratio(std::vector<float> peek_feat, std::vector<float> neutral_feat)
{
	std::vector<float> eye_feat;
	float temp;
	for (int i = 0; i < 6; i++)
	{
		if(i == 2 || i == 3)
		{	
			temp = abs (peek_feat[i] - neutral_feat[i])/neutral_feat[i] * 100;
			eye_feat.push_back(temp);
		}
		else
		{
			temp = ratio(peek_feat[i], neutral_feat[i])/neutral_feat[i] * 100;
			eye_feat.push_back(temp);
		}
	}
	return eye_feat;
}

// call calc_eye_feat_ratio to calc eye features for both left and right then return the combined results
std::vector<float> frame::eye_feat_ratio(frame *peak_frame, frame *neutral_frame)
{
	std::vector<float> final_features_l = calc_eye_feat_ratio(peak_frame->left_feats, neutral_frame->left_feats);
	std::vector<float> final_features_r = calc_eye_feat_ratio(peak_frame->right_feats, neutral_frame->right_feats);
	// Combine the two vectors
	final_features_l.insert(final_features_l.end(), final_features_r.begin(), final_features_r.end());
	return final_features_l;

}


// Helper function used by calc_probs that calculates probability mass
float frame::get_probability(float feat, float mean, float std, float precision)
{
	//cout << '\n' << "Mean: " << mean << " Feat: " << feat << " Std: " << std;
	//cout << '\n' << "Value: " << exp( -pow( (feat - mean), 2 )/(2* pow(std,2)) )/( pow( (2*3.14), (1/2) )*std ); 
	//cout << endl;
	return exp( -pow( (feat - mean), 2 )/(2* pow(std,2)) )/( pow( (2*3.14), (1/2) )*std );
}

//Given the calculated feature values (after finding final ratios) calculate the probablility associated with each class
std::vector<float> frame::calc_probs(std::vector<float> model_vals, std::vector<float> feat_vals)
{
	float prob_a = log(model_vals[0]);
	float prob_b = log(model_vals[model_vals.size()/2]); // Get model B's value
	model_vals.erase(model_vals.begin()); // Erase the class probability of A from the vector
	model_vals.erase(model_vals.begin() + model_vals.size()/2); // Erase the class probability of B from the vector
	int itr_size = model_vals.size();
	int lin_count = 0; 
	std:vector<float> class_probs; // Vector of size two holding prob for class A and B

	// Iterate through each feature for class A and B and sum to get the probability
	for(int i = 0; i < itr_size/2 ; i+=3)
	{
			prob_a += log(get_probability(feat_vals[lin_count], model_vals[i], model_vals[i+1], model_vals[i+3]));
			prob_b += log(get_probability(feat_vals[lin_count], model_vals[i+itr_size/2], model_vals[i+itr_size/2 +1], model_vals[i+itr_size/2+2]));
			cout << feat_vals[lin_count] << endl;
			lin_count++;
	}
	//cout << "PROB A: " << prob_a << endl;
	//cout << "PROB B: " << prob_b << endl;
	class_probs.push_back(prob_a);
	class_probs.push_back(prob_b);
	return class_probs;
}