#include <array>
#include <vector>
#include <deque>
#include <bitset>
#include "LinearModel.h"
#include <fstream>
#include <iostream>


using namespace std;

#pragma once
class GappedArray
{
	array<double, 4096> * keys;
	bitset<4096> * keysb;
	bool isLeaf;
	GappedArray *left;
	GappedArray *right;
	int MEDIND;
	int MAXIND;
	LinearModel lm;
	double density;
	int numKeys;
	int adjPos(double key, int initPos);
	int binSearch(int lb, int ub, double key);
	void expand();
//	void contract();
public:
	GappedArray(LinearModel model, double minKey);
	~GappedArray();
	void insert(double key);
//	void remove(double key);
	bool lookup(double key);
	void print(string outputloc);
	bool testOrder();
};
