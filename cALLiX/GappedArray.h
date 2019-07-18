#include <array>
#include <vector>
#include <bitset>
#include "LinearModel.h"
#include "global.h"
#include <fstream>
#include <iostream>
#include <set>


using namespace std;

#pragma once
class GappedArray
{
	array<double, ARRSIZE> * keys;
	bitset<ARRSIZE> * keysb;
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
	int DEPTH;
	long aldh();
//	void contract();
public:
	GappedArray(LinearModel model, double minKey, int depth);
	~GappedArray();
	bool insert(double key);
//	void remove(double key);
	bool lookup(double key);
	void print(string outputloc);
	void printhead(string outputloc);
	bool testOrder();
	int numLeaves();
	double avgLeafDepth();
};


#pragma once
class GA
{
	size_t numKeys;
	GappedArray * root;
	MinMaxTransformer * mmt;
	LinearModel * lm;
	bool initializing;
	set<double> * initkeys;
	size_t initSize;
	void initialize();
	double textToDouble(const string& str);
public:
	GA();
	GA(int initialSize);
	~GA();
	bool insert(double key);
	void insertFromFile(string inputloc);
	bool sorted();
	bool contains(double key);
	int count();
	int numLeaves();
	double avgLeafDepth();
	double avgKeysPerLeaf();
	void print(string outputloc);
	void printhead(string outputloc);
	void testLookup(string inputloc);
	void testLookup(string inputloc, string outputloc);
	void info();
};