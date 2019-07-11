#include <array>
#include <bitset>
#include "LinearModel.h"


using namespace std;

#pragma once
class GappedArray
{
	bool isLeaf;
	GappedArray *left;
	GappedArray *right;
	int medianIndex;
	LinearModel lm;
	array<double, 4096> keys;
	bitset<4096> keysb;
	double density;
	int numKeys;
	int adjPos(double key, int initPos);
	void makeGap(int pos);
	int binSearch(int lb, int ub, double key);
public:
	GappedArray(LinearModel model, double minKey);
	void insert(double key);
	void expand();
};

class Root
{


};