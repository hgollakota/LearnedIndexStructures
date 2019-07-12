#include <array>
#include <vector>
#include <deque>
#include <bitset>
#include "LinearModel.h"


using namespace std;

#pragma once
class GappedArray
{
	array<double, 4096> * keys;
	bitset<4096> * keysb;
	bool isLeaf;
	GappedArray *left;
	GappedArray *right;
	int medianIndex;
	LinearModel lm;
	//array<double, 4096> keys;
	//bitset<4096> keysb;
	double density;
	int numKeys;
	int adjPos(double key, int initPos);
	void makeGap(int pos);
	int binSearch(int lb, int ub, double key);
	void expand();
	void contract();
public:
	GappedArray(LinearModel model, double minKey);
	~GappedArray();
	void insert(double key);
	void remove(double key);
	bool lookup(double key);
};

class Root
{


};