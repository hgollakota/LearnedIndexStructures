#include <iostream>
#include <vector>
#include "LinearModel.h"
#include <deque>
#include "GappedArray.h"
#include <random>


using namespace std;
const size_t ARR_SIZE = 4096;
const double DENSITY = 0.5;


int main() {


	LinearModel lm = LinearModel(1.0, 0.0);

	GappedArray ga = GappedArray(lm, 0.0);
	//test insert at bottom and betwixt them all
	//ga.insert(0.5);
	//ga.insert(5);
	//ga.insert(5.5);
	//ga.insert(-1.0);
	//ga.insert(0.25);
	//ga.insert(1.0);
	//ga.insert(5.4);
	//ga.insert(2.5);
	//ga.insert(0.75);
	//ga.insert(-0.5);
	//ga.insert(-5.0);
	//ga.insert(5.6);
	//ga.insert(5.55);
	//ga.insert(5.56);
	//ga.insert(5.7);
	//ga.insert(5.677);
	//ga.insert(-6);
	//ga.insert(-5.5);
	//ga.insert(5.2);

	//////test insert at top and betwixt
	//ga.insert(4096);
	//ga.insert(4095);
	//ga.insert(4094);
	//ga.insert(5000);
	//ga.insert(5000.1);
	//ga.insert(5000.002);
	//ga.insert(4099);
	//ga.insert(100000);
	//ga.insert(6000);
	//ga.insert(6000.1);
	//
	////test insert middle
	//for (double d = 3500.01; d < 5090; d += 50) {
	//	ga.insert(d);
	//}

	//ga.insert(25);
	//ga.insert(26);
	//ga.insert(27);
	//ga.insert(26.5);
	//ga.insert(24.5);

	double lb = -10.0;
	double ub = 5000.00;
	std::uniform_real_distribution<double> unif(lb, ub);
	std::default_random_engine re;
	int i = 2000;
	while (0 < i) {
		ga.insert(unif(re));
		i--;
	}




	ga.print("output1.txt");

	return(0);
}

