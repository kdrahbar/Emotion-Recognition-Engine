#include "frame.h"
#define dist_between(p1, p2) sqrt( pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2) )

//Helper function to multiply rotation matrix by landmark matrix
void frame::matrix_mult(float angle)
{
	for (std::vector<point>::iterator it = landmarks.begin() ; it != landmarks.end(); ++it)
	{
		it->x = (it->x * cos(-angle)) + (it->y * sin(-angle));
		it->y = (it->x * sin(-angle)) + (it->y * cos(-angle));
	}
}

void frame::matrix_mult(float angle, std::vector<point> inp_mat)
{
	for (std::vector<point>::iterator it = inp_mat.begin() ; it != inp_mat.end(); ++it)
	{
		it->x = (it->x * cos(-angle)) + (it->y * sin(-angle));
		it->y = (it->x * sin(-angle)) + (it->y * cos(-angle));
	}	
}

// This is used in extract_eye_feat. In matlab code it has landmarks of 6:
// So is that 6 through 11 or 5 throught eleven?
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

void frame::create_brow_landmarks()
{
	int r_tmp [11] ={6,7,8,9,10,26,27,28,29,30,31};
	int l_tmp [11]= {1,2,3,4,5,20,21,22,23,24,25};
	for (int i = 0; i < 11; i++)
	{
		l_eyebrow_landmks.push_back(landmarks[l_tmp[i]]);
		r_eyebrow_landmks.push_back(landmarks[r_tmp[i]]);
	}
}

void frame::add_translation(point trans, std::vector<point> matrix)
{
	for (std::vector<point>::iterator it = matrix.begin() ; it != matrix.end(); ++it)
	{
		it->x += trans.x;
		it->y += trans.y;
	}
}

// Need a better value for pi
// Used by Extract eyebrow feat to calculate L1, L2, L3, L4, A1, A2
std::vector<float> frame::get_dist(std::vector<point> lndmarks)
{
	std::vector<float> feat_point;
	feat_point.push_back( dist_between(lndmarks[5], lndmarks[0]) );
	feat_point.push_back( dist_between(lndmarks[8], lndmarks[4]) );
	float a1 = atan( (lndmarks[0].y - lndmarks[5].y)/ (lndmarks[0].x - lndmarks[5].x)*180/3.14 );
	if (a1 < 0)
		a1 += 180;
	feat_point.push_back(a1);
	float a2 = atan( (lndmarks[4].y - lndmarks[8].y)/ (lndmarks[4].x - lndmarks[8].x)*180/3.14 );
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
	int start_index = start_row*49;
	point add_point;
	for( int i = start_index; i < (start_index + 98); i+=2 )
	{
		add_point.x = input_file[i];
		add_point.y = input_file[i+1];
		landmarks.push_back(add_point);
	}
	create_brow_landmarks();
}

void frame::rotate()
{
	float angle;
	angle = atan( (landmarks[5].x - landmarks[6].x)/(landmarks[5].y - landmarks[6].y) );
	cout << "Angle to be rotated: " << angle << '\n';
	matrix_mult(angle);
}

std::vector<float> frame::extract_eyebrow_feat(point neutral_ctrd, std::vector<point> lndmarks)
{
	float angle = atan( (lndmarks[6].y - lndmarks[9].y)/(lndmarks[6].x - lndmarks[9].y) );
	matrix_mult(angle, lndmarks);
	point *centroid = calc_centroid(lndmarks);
	point translate;
	translate.x = neutral_ctrd.x - centroid->x;
	translate.y = neutral_ctrd.y - centroid->y;
	add_translation(translate, lndmarks);
	return get_dist(lndmarks);

}
