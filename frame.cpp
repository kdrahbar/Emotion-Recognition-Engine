#include "frame.h"

/*
frame::frame()
{
	right_centroid;
	right_centroid;
	landmarks;
	left_feats;
	right_feats;

	r_eyebrow_landmks = {6,7,8,9,10,26,27,28,29,30,31};
	l_eyebrow_landmks = {1,2,3,4,5,20,21,22,23,24,25};

}*/
//int r_eyebrow_landmks [11] = {6,7,8,9,10,26,27,28,29,30,31};
//int l_eyebrow_landmks [11] = {1,2,3,4,5,20,21,22,23,24,25};

//Helper function to multiply rotation matrix by landmark matrix
void frame::matrix_mult(float angle)
{
	for (std::vector<point>::iterator it = landmarks.begin() ; it != landmarks.end(); ++it)
	{
		it->x = (it->x * cos(-angle)) + (it->y * sin(-angle));
		it->y = (it->x * sin(-angle)) + (it->y * cos(-angle));
	}
}

void frame::calc_centroids()
{
	int rx_sum=0, ry_sum=0, lx_sum=0, ly_sum=0;
	for (int i = 5; i < 11; i++) 
	{
		lx_sum += landmarks[l_eyebrow_landmks[i]].x;
		ly_sum += landmarks[l_eyebrow_landmks[i]].y;
		rx_sum += landmarks[r_eyebrow_landmks[i]].x;
		ry_sum += landmarks[r_eyebrow_landmks[i]].y;
	}
	l_centroid.x = lx_sum/6;
	l_centroid.y = ly_sum/6;
	r_centroid.x = rx_sum/6;
	r_centroid.y = ry_sum/6;

}

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
	int r_tmp [11] ={6,7,8,9,10,26,27,28,29,30,31};
	int l_tmp [11]= {1,2,3,4,5,20,21,22,23,24,25};
	for (int i = 0; i < 11; i++)
	{
		l_eyebrow_landmks[i] = l_tmp[i];
		r_eyebrow_landmks[i] = r_tmp[i];
	}
}

void frame::rotate(char side)
{
	float angle;
	angle = atan( (landmarks[5].x - landmarks[6].x)/(landmarks[5].y - landmarks[6].y) );
	cout << "Angle to be rotated: " << angle << '\n';
	matrix_mult(angle);
}

