#include "LinearModel.h"

LinearModel::LinearModel() : a(0), b(0) {};
LinearModel::LinearModel(double A, double B) : a(A), b(B) {};
LinearModel::LinearModel(vector<double> samplekeys, vector<int> sampleindices) {
	if (samplekeys.size() == 1) { a = 1.0; b = 0.0; return;	}
	double xbar = 0.0;
	double ybar = 0.0;
	for (size_t i = 0; i < samplekeys.size(); i++) {
		xbar += samplekeys[i];
		ybar += sampleindices[i];
	}
	xbar = xbar / (double)samplekeys.size();
	ybar = ybar / (double)samplekeys.size();
	double top = 0.0;
	double bot = 0.0;
	for (size_t i = 0; i < samplekeys.size(); i++) {
		top += (samplekeys[i] - xbar) * (sampleindices[i] - ybar);
		bot += (samplekeys[i] - xbar) * (samplekeys[i] - xbar);
	}
	a = top / bot;
	b = ybar - a * xbar;
	return;
}
double LinearModel::y(double x) { return a * x + b; }
void LinearModel::slope(double A) { a = A; }
double LinearModel::slope() { return a; }
void LinearModel::intercept(double B) { b = B; }
double LinearModel::intercept() { return b; }

MinMaxTransformer::MinMaxTransformer(double min_old, double max_old) {
	oMin = min_old;
	oMax = max_old;
	nMin = 0;
	nMax = 1;
	oDif = max_old - min_old;
	nDif = 1;
}
MinMaxTransformer::MinMaxTransformer(double min_old, double max_old, double min_new, double max_new) {
	oMin = min_old;
	oMax = max_old;
	nMin = min_new;
	nMax = max_new;
	oDif = max_old - min_old;
	nDif = max_new - min_new;
}
double MinMaxTransformer::y(double x) { return ((x - oMin) / oDif) * (nDif)+nMin; }
void MinMaxTransformer::oldMin(double min_old) { oMin = min_old; oDif = oMax - min_old; }
void MinMaxTransformer::oldMax(double max_old) { oMax = max_old; oDif = max_old - oMin; }
void MinMaxTransformer::newMin(double min_new) { nMin = min_new; nDif = nMax - min_new; }
void MinMaxTransformer::newMax(double max_new) { nMax = max_new; nDif = max_new - nMin; }
double MinMaxTransformer::oldMin() { return oMin; }
double MinMaxTransformer::oldMax() { return oMax; }
double MinMaxTransformer::newMin() { return nMin; }
double MinMaxTransformer::newMax() { return nMax; }