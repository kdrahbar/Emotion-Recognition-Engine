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
	std::vector<int> landmarks = file_to_vector();
	frame *neutral = new frame(landmarks);
	neutral->rotate('l');
	neutral->calc_centroids();


}