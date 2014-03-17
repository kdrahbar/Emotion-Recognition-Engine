#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "frame.h"
#include <stdlib.h>
#include "debug.h"

using namespace std;

// Take input file and serialize it into a vector
std::vector<int> file_to_vector(char input[])
{
	std::vector<int> file_vector;
	std::string line;
	std::ifstream input_file;
	input_file.open(input);

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

// Used to parse the output file generated by the python parser
std::vector<float> file_to_vector_float(char input[])
{
	std::vector<float> file_vector;
	std::string line;
	std::ifstream input_file;
	input_file.open(input);

	while (std::getline(input_file, line)) 
	{
		std::istringstream iss(line);
		float value;
		while (iss >> value)
	 	{
	 		file_vector.push_back(value);
	  	}
	}
	return file_vector;
}

// Generates a vector with the 12 feature values of A and B
// The first half of the vector corresponds to the values for A
// The second half corresponds to the values for B
std::vector<float> bayes_parser(char input[])
{
	std::vector<float> bayes_file = file_to_vector_float(input);
	std::vector<float> result;
	int file_size = bayes_file.size();
	// Extract bayes class values for A
	for(int i = 0; i < file_size; i+=2)
	{
		result.push_back(bayes_file[i]);
	}
	// Extract bayes class values for B
	for(int i = 1; i < file_size; i+=2)
	{
		result.push_back(bayes_file[i]);
	}
	return result;
}


int main()
{
	char input[] = "landmarks.txt";
	char bayes_file_name[] = "output.txt"; 
	// Serialize the points into a vector
	std::vector<int> lndmarks = file_to_vector(input);
	
	// Creates the neutral landmarks
	frame *neutral = new frame(lndmarks);
	neutral->rotate();
	neutral->calc_centroids();
	
	std::vector<float> lndmk_l = neutral->extract_eyebrow_feat(neutral->l_centroid, neutral->l_eyebrow_landmks);
	std::vector<float> lndmk_r = neutral->extract_eyebrow_feat(neutral->r_centroid, neutral->r_eyebrow_landmks);
	cout << "\n" << "Neutral Left Landmarks: ";
	debug::print_vec(lndmk_l);
	cout << "\n" << "Neutral Right Landmarks: ";
	debug::print_vec(lndmk_r);

	frame *tst_frame = new frame(lndmarks, 1);
	tst_frame->rotate();
	tst_frame->calc_centroids();

	std::vector<float> tst_lndmk_l = tst_frame->extract_eyebrow_feat(neutral->l_centroid, tst_frame->l_eyebrow_landmks);
	std::vector<float> tst_lndmk_r = tst_frame->extract_eyebrow_feat(neutral->r_centroid, tst_frame->r_eyebrow_landmks);
	cout <<"\n" << "Test Landmarks Left:";
	debug::print_vec(tst_lndmk_l);
	cout <<"\n" << "Test Landmarks Right:";
	debug::print_vec(tst_lndmk_r);

	std::vector<float> final_features_l = frame::eye_feat_ratio(tst_lndmk_l, lndmk_l);
	std::vector<float> final_features_r = frame::eye_feat_ratio(tst_lndmk_r, lndmk_r);

	final_features_l.insert(final_features_l.end(), final_features_r.begin(), final_features_r.end());
	std::vector<float> final_features = final_features_l;

	// Python parser that cleans up the original naive bayes file
	system("python naive_bayes_parse.py");
	std::vector<float> naive_bayes_file = bayes_parser(bayes_file_name);

	std::vector<float> prob_vec;
	prob_vec = frame::calc_probs(naive_bayes_file, final_features);

	//Print class B values from bayes parser
	//cout << "Class B begins: " << '\n' << '\n';
	//int file_size = naive_bayes_file.size();
	//for (int i = file_size/2; i < file_size; i++ )
	//	cout << naive_bayes_file[i] << '\n';

}