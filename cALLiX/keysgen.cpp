#include "keysgen.h"

keysgenUniform::keysgenUniform(double lb, double ub)
{
	lowerBound = lb;
	upperBound = ub;
	outputFile = "output.txt";
	unif = new uniform_real_distribution<double>(lowerBound, upperBound);
}

keysgenUniform::keysgenUniform(string outfile, double lb, double ub)
{
	lowerBound = lb;
	upperBound = ub;
	outputFile = outfile;
	unif = new uniform_real_distribution<double>(lowerBound, upperBound);
}

keysgenUniform::~keysgenUniform()
{
	free(unif);
	unif = NULL;
}

void keysgenUniform::generate(long n)
{
	ofstream fo;// declaration of file pointer named outfile
	fo.open(outputFile, ios::out | ofstream::app);
	if (!fo.is_open()) {
		cout << "Weird error - should be able to create a new output file " << outputFile << "\n";
		return;
	}
	double d = 0.0;
	long i = 0;
	while (i < n) {
		d = (*unif)(re);
		fo << d << "\n";
		i++;
		if (i % 100 == 0) cout << (double)i / n << "% complete\n";
	}
	fo.close();
	cout << "Keys are Generated\n";
}

void keysgenUniform::generate(string outfile, long n)
{
	outputFile = outfile;
	generate(n);
}



keysgenMultiMode::keysgenMultiMode(double m, double s)
{
	np = 1;
	means.emplace_back(m);
	stddevs.emplace_back(s);
	normdist = NULL;
	outputFile = "output.txt";
	approxlb = m - 3 * s;
	approxub = m + 3 * s;
}

keysgenMultiMode::keysgenMultiMode(string outfile, double m, double s)
{
	np = 1;
	means.emplace_back(m);
	stddevs.emplace_back(s);
	normdist = NULL;
	outputFile = outfile;
	approxlb = m - 3 * s;
	approxub = m + 3 * s;
}

keysgenMultiMode::keysgenMultiMode(int numberPeaks, double range)
{
	if (range < 0) range *= -1;
	if (range == 0.0) range = 360.0;
	keysgenMultiMode(numberPeaks, 0.0, range);
}

keysgenMultiMode::keysgenMultiMode(int numberPeaks, double range, string outfile)
{
	outputFile = outfile;
	keysgenMultiMode(numberPeaks, range);
}

keysgenMultiMode::keysgenMultiMode(int numberPeaks, double lb, double ub, string outfile)
{
	outputFile = outfile;
	keysgenMultiMode(numberPeaks, lb, ub);
}

keysgenMultiMode::keysgenMultiMode(int numberPeaks, double lb, double ub)
{
	approxlb = lb;
	approxub = ub;
	np = numberPeaks;
	double distance = -1.0;
	if (ub > lb) distance = ub - lb;
	else if (ub < lb) distance = lb - ub;
	else distance = 360.0;
	double d = distance / (double)np; //distance for each section
	double s = d / 6.0;
	for (int i = 0; i < np; i++) {
		double m = 3 * s + i * d;
		means.emplace_back(m);
		stddevs.emplace_back(s);
	}
	normdist = NULL;
}

keysgenMultiMode::~keysgenMultiMode()
{
	clearPeaks();
	//if (normdist != NULL) {
	//	free(normdist);
	//	normdist = NULL;
	//}
}


int keysgenMultiMode::numPeaks()
{
	return np;
}

void keysgenMultiMode::addPeak(double m, double s)
{
	double templb = m - 3 * s;
	double tempub = m + 3 * s;
	if (templb < approxlb) approxlb = templb;
	if (tempub > approxub) approxub = tempub;
	means.emplace_back(m);
	stddevs.emplace_back(s);
	np++;
}

void keysgenMultiMode::clearPeaks()
{
	np = 0;
	means.clear();
	means.shrink_to_fit();
	stddevs.clear();
	stddevs.shrink_to_fit();
	approxlb = 0.0;
	approxub = 0.0;
}


void keysgenMultiMode::generate(long n)
{
	if (outputFile.empty()) outputFile = "output.txt";
	generate(outputFile, n);
}

void keysgenMultiMode::generate(string outfile, long n)
{
	if (np < 1) {
		cout << "Add Peaks before generating\n";
		return;
	}
	if (n < 1) n = 1024;
	ofstream fo;// declaration of file pointer named outfile
	fo.open(outfile, ios::out | ofstream::app);
	if (!fo.is_open()) {
		cout << "Weird error - should be able to create a new output file " << outputFile << "\n";
		return;
	}
	long kpp = n / np;
	long k = 0;
	for (int i = 0; i < np; i++) {
		normdist = new normal_distribution<double>(means[i], stddevs[i]);
		long j = 0;
		double d = 0.0;
		while (j < kpp) {
			d = (*normdist)(re);
			fo << std::setprecision(std::numeric_limits<double>::digits10 + 2) << d << "\n";
			j++; 
			k++;
			if (j % 100 == 0) cout << ((double)j + (double)i * kpp) / ((double)n)*100 << "% complete\n"; 
		}
		free(normdist);
		normdist = NULL;
	}
	normdist = new normal_distribution<double>(means[0], stddevs[0]);
	double d = 0.0;
	while (k < n) { //add any additional keys remaining to the lowest peak
		d = (*normdist)(re);
		fo << std::setprecision(std::numeric_limits<double>::digits10 + 2) << d << "\n";
		k++;
	}
	fo.close();
	free(normdist);
	normdist = NULL;
	cout << "Keys are Generated in file " << outfile << "\n";
	cout << "Number of keys generated: " << k - 1 << "\n";
	cout << "The details of the peaks are: \n";
	for (int i = 0; i < np; i++) {
		cout << "Mean" << i << " : " << means[i] << "\t\t\t" << "StdDev" << i << " : " << stddevs[i] << "\n";
	}
}
