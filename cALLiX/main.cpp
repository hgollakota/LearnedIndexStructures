#include <iostream>
#include <vector>
#include "LinearModel.h"
#include "global.h"
#include "GappedArray.h"
#include <random>
#include <algorithm>

using namespace std;


int main() {

	//double lb = 0;
	//double ub = ARRSIZE-1;
	//std::uniform_real_distribution<double> unif(lb, ub);
	ofstream outfile;
	outfile.open("normaldist_m0_s100_n131072INPUT2.txt", ios::out | ofstream::app);

	std::default_random_engine re;
	vector<double> vect;
	normal_distribution<double> distribution(0.0, 100);
	int i = 0;
	vector<double> initVectx;
	vector<int> initVecty;
	while (i < 50) {
		double d = distribution(re);
		initVectx.push_back(d);
		initVecty.push_back(i);
		outfile << fixed << d << "\n";
		i++;
	}
	sort(initVectx.begin(), initVectx.begin() + initVectx.size());

	MinMaxTransformer mmt = MinMaxTransformer(0, i-1, 0, ARRSIZE - 1);
	vector<int> initVecty_t;

	for (int j = 0; j < initVecty.size(); j++) {
		initVecty_t.push_back((int)mmt.y(initVecty[j]));
	}

	LinearModel lm = LinearModel(initVectx,initVecty_t);
	GappedArray ga = GappedArray(lm, initVectx[0]);

	while (i < 131072) {
		double d = distribution(re);
		ga.insert(d);
		outfile << fixed << d << "\n";
		i++;
	}
	outfile.close();

	ga.printhead("normaldist_m0_s100_n131072HEADER2.txt");
	ga.print("normaldist_m0_s100_n131072OUTPUT2.txt");

	return(0);
}

