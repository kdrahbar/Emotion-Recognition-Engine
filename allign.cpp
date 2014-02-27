#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "frame.h"

using namespace std;

// Take input file and serialize it into a vector
std::vector<int> file_to_vector()
{
	std::vector<int> file_vector;
	std::string line;
	std::ifstream input_file;
	input_file.open("landmarks.txt");

	while (std::getline(input_file, line)) 
	{
		std::istringstream iss(line);
		int value;
		while (iss >> value)
	 	{
	 		file_vector.push_back(value);
	  	}
	}
	return file_vector;
}

int main()
{
	std::vector<int> lndmarks = file_to_vector();
	// Creates the neutral landmarks
	frame *neutral = new frame(lndmarks);
	//for(int i =0; i < neutral->landmarks.size(); i++)
	//	cout << neutral->landmarks[i].x << " " << neutral->landmarks[i].y << '\n';
	neutral->rotate();
	//for(int i =0; i < neutral->landmarks.size(); i++)
	//	cout << neutral->landmarks[i].x << " " << neutral->landmarks[i].y << '\n';
	
	neutral->calc_centroids();
	//cout << neutral->l_centroid.x << " " << neutral->l_centroid.y << '\n';
	for(int i =0; i < neutral->l_eyebrow_landmks.size(); i++)
		cout << neutral->l_eyebrow_landmks[i].x << " " << neutral->l_eyebrow_landmks[i].y << '\n';
	
	std::vector<float> lndmk = neutral->extract_eyebrow_feat(neutral->l_centroid, neutral->l_eyebrow_landmks);
	for (int i = 0; i < lndmk.size(); i ++)
		cout << lndmk[i] << '\n';

}