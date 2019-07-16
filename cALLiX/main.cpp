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


	LinearModel lm = LinearModel(1.0, 0.0);
	GappedArray ga = GappedArray(lm, 0.0);
	double lb = 0;
	double ub = 4095;
	std::uniform_real_distribution<double> unif(lb, ub);
	std::default_random_engine re;
	vector<double> vect;
	while (vect.size() < 8192) {
		double d = unif(re);
		vect.push_back(d);
		ga.insert(d);
	}

	ga.print("output5.txt");
	ofstream outfile;
	outfile.open("input5.txt", ios::out | ofstream::app);
	for (int i = 0; i < vect.size(); i++) {
		outfile << vect[i] << "\n";
	}
	outfile.close();// closes file; always do this when you are done using the file

	//ga.insert(668.74);
	//ga.insert(4173.39);
	//ga.insert(4844.03);
	//ga.insert(1097.38);
	//ga.insert(1533.92);
	//ga.insert(2731.58);
	//ga.insert(933.794);
	//ga.insert(4964.34);
	//ga.insert(4982.27);
	//ga.insert(4838.15);
	//ga.insert(3626.45);
	//ga.insert(4905.36);
	//ga.insert(540.407);
	//ga.insert(3988.51);
	//ga.insert(1478.12);
	//ga.insert(13.9653);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(4982.27);
	//ga.insert(540.407);
	//ga.insert(540.407);
	//ga.insert(540.407);
	//ga.insert(540.407);
	//ga.insert(540.407);
	//ga.insert(540.407);

	//ga.insert(540.407);
	//ga.insert(540.407);
	//ga.insert(540.407);
	//ga.insert(4982.27);
	//ga.print("outputYES.txt");




	return(0);
}

