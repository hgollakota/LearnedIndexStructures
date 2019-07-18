#include <string>
#include <random>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

#pragma once
class keysgenUniform
{
	uniform_real_distribution<double>* unif;
public:
	keysgenUniform(double lb, double ub);
	keysgenUniform(string outfile, double lb, double ub);
	~keysgenUniform();
	double lowerBound;
	double upperBound;
	string outputFile;
	default_random_engine re;
	void generate(long n);
	void generate(string outfile, long n);
};

#pragma once
class keysgenMultiMode
{
	vector<double> means;
	vector<double> stddevs;
	int np;
	normal_distribution<double>* normdist;
public:
	keysgenMultiMode(double m, double s);
	keysgenMultiMode(string outfile, double m, double s);
	keysgenMultiMode(int numberPeaks, double range);
	keysgenMultiMode(int numberPeaks, double range, string outfile);
	keysgenMultiMode(int numberPeaks, double lb, double ub);
	keysgenMultiMode(int numberPeaks, double lb, double ub, string outfile);
	~keysgenMultiMode();
	int numPeaks();
	void addPeak(double m, double s);
	void clearPeaks();
	void generate(long n);
	void generate(string outfile, long n);
	default_random_engine re;
	string outputFile;
	double approxlb;
	double approxub;
};