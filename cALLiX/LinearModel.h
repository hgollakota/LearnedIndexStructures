#include <vector>
#include <array>
#include <list>
#include <numeric>

using namespace std;
#pragma once
class LinearModel //y = ax + b
{   
	double a; //slope
	double b; //intercept
public:
	LinearModel(); //construct linear model a=0 b=0
	LinearModel(double A, double B); //construct linear model
	LinearModel(vector<double> samplekeys, vector<int> sampleindices); //least squares regression line
	double y(double x); //return output for input x
	double intercept(); //return intercept
	void intercept(double B); //set intercept
	double slope(); //return slope
	void slope(double A); //set slope
};

#pragma once
class MinMaxTransformer
{
	double oMin, oMax;
	double nMin, nMax;
	double oDif, nDif;
public:
	MinMaxTransformer(double min_old, double max_old);
	MinMaxTransformer(double min_old, double max_old, double min_new, double max_new);
	double y(double x);
	void oldMin(double min_old);
	void oldMax(double max_old);
	void newMin(double min_new);
	void newMax(double max_new);
	double oldMin();
	double oldMax();
	double newMin();
	double newMax();
};


