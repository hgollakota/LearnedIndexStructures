#include <iostream>
#include <fstream>
#include <vector>
#include "LinearModel.h"
#include "global.h"
#include "GappedArray.h"
#include "keysgen.h"
#include <random>
#include <algorithm>

using namespace std;


int main() {

	/*methods to generate keys*/ 
	//keysgenMultiMode kmm = keysgenMultiMode(3, 0.0, 360.0);
	//cout << kmm.numPeaks();
	//kmm.generate("3peaksinput.txt",1048576);


	/*test a cold start GA*/
	GA ga = GA(); //oo la la

	cout << "Inserting from file\t\t\t\t";
	ga.insertFromFile("3peaksinput.txt");
	cout << "Finished Inserting\n";
	cout << "Testing Contains from File\n";
	ga.testLookup("3peaksinput.txt", "3peaksErrors.txt");

	///*This one takes the longest time*/
	//ga.printhead("test.txt");
	ga.info();
	ga.print("3peaksALL.txt");
	//ga.~GA(); not working yet

	return(0);
}

