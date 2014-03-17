#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <vector>
#include "frame.h"
using namespace std;

class debug
{
	public:
		debug();
		static void print_vec(std::vector<float> inp);
		static void print_vec(std::vector<point> inp);
};

#endif