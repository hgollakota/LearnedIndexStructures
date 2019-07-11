#include "GappedArray.h"

GappedArray::GappedArray(LinearModel model, double minKey)
{
	lm = model;
	isLeaf = true;
	numKeys = 0;
	density = 0.0;
	medianIndex = keys.size() / 2;
	left = NULL;
	right = NULL;
	keys.fill(minKey);
	keysb.reset();
	keysb[0].flip();
}

void GappedArray::insert(double key)
{
	/*If GappedArray has already been expanded, try inserting into appropriate child instead*/
	if (!isLeaf) {
		if (lm.y(key) < medianIndex) left->insert(key);
		else right->insert(key);
		return;
	}

	int initPos = (int)lm.y(key); //initial predicted index
	/*accounts for if the model predicts higher or lower than the existing minimum/maximum*/
	if (initPos < 0) initPos = 0;
	if (initPos > keys.size() - 1) initPos = keys.size() - 1;
	/*Adjusts the position to appropriate index if necessary*/
	int actPos = adjPos(key, initPos);
	if (keys[actPos] == key) return; //key is already in array

	/*If the position to insert is not a gap, makes a gap*/
	if (keysb[actPos]) makeGap(actPos);

	/*Actually insert key*/
	int i = actPos;
	while (!keysb[i]) {
		keys[i] = key;
		i++;
	}
	keysb[actPos].flip();

	numKeys++;
	density = (double)numKeys / keys.size();
//	if (density >= 0.5) expand();
}


int GappedArray::adjPos(double key, int initPos)
{
	if (keys[initPos] == key) return initPos; //key is already in array
	if (keys[initPos] > key) {
		int b = 2;
		while (initPos - b >= 0 && keys[initPos - b] > key) {b *= 2;}
		if (initPos - b < 0) return binSearch(0, initPos - b / 2, key);
		return binSearch(initPos - b, initPos - b / 2, key);
	}
	if (keys[initPos] < key) {
		int b = 2;
		while (initPos + b < keys.size() && keys[initPos + b] < key) { b *= 2; }
		if (initPos + b > keys.size() - 1) return binSearch(initPos + b / 2, keys.size() - 1, key);
		return binSearch(initPos + b / 2, initPos + b, key);
	}
}

int GappedArray::binSearch(int lb, int ub, double key)
{
	int m = -1;
	while (lb <= ub) {
		m = lb + (ub - lb) / 2;
		if (key < keys[m]) ub = m - 1;
		else if (key > keys[m]) lb = m + 1;
		else return m;
	}
	return lb;
}

void GappedArray::makeGap(int pos)
{
	int lo = pos;
	int hi = pos;

	while (keysb[lo] && keysb[hi]) {
		if (lo > 0) lo--;
		if (hi < keysb.size() - 1) hi++;
	}
	if (!keysb[hi]) {
		for (int i = hi; i > pos; i--) {
			keys[i] = keys[i - 1];
		}
		keysb[hi].flip();
		keysb[pos].flip();
		return;
	}
	if (!keysb[lo]) {
		for (int i = lo; i < pos; i++) {
			keys[i] = keys[i + 1];
		}
		keysb[lo].flip();
		keysb[pos].flip();
		return;
	}
}

void GappedArray::expand()
{

}