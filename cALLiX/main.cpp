#include <iostream>
#include <vector>
#include "LinearModel.h"
#include "GappedArray.h"

using namespace std;

#pragma once
int main() {
	vector<double> samplex;
	vector<int> sampley;
	double toinsert = 4.0;
	for (int i = 0; i < 10; i++) {
		samplex.push_back(toinsert);
		sampley.push_back(2*i);
		toinsert += 1.0;
	}

	LinearModel lm = LinearModel(samplex, sampley);

    cout << "slope of lm is\t\t" << lm.slope() << "\n";
	cout << "intercept of lm is\t" << lm.intercept() << "\n";
	cout << "the y of 1.25 is \t" << lm.y(1.25) << "\n";
	return(0);
}

