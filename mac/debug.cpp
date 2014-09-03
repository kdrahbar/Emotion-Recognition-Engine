#include "debug.h"

void debug::print_vec(std::vector<float> inp)
{
	int itr = inp.size();
	cout << "\n";
	for(int i= 0; i < itr; i++)
		cout << inp[i] << "\n";
}

void debug::print_vec(std::vector<point> inp)
{
	int itr = inp.size();
	cout << "\n";
	for(int i= 0; i < itr; i++)
		cout << inp[i].x << " " << inp[i].y << "\n";
}
