#include "GappedArray.h"

GappedArray::GappedArray(LinearModel model, double minKey)
{
	keys = new array<double, 4096>;
	keys->fill(minKey);
	keysb = new bitset<4096>;
	keysb->reset();
	lm = model;
	isLeaf = true;
	numKeys = 1;
	density = (double)1 / keysb->size();
	medianIndex = keysb->size() / 2;
	left = NULL;
	right = NULL;
}

void GappedArray::insert(double key)
{
	/*If GappedArray has already been expanded, insert into appropriate child instead*/
	if (!isLeaf) {
		if (lm.y(key) < medianIndex) left->insert(key);
		else right->insert(key);
		return;
	}

	int initPos = (int)lm.y(key); //initial predicted index
	/*accounts for if the model predicts higher or lower than the existing minimum/maximum*/
	if (initPos < 0) initPos = 0;
	if (initPos > keysb->size() - 1) initPos = keysb->size() - 1;
	/*Adjusts the position to appropriate index if necessary*/
	int actPos = adjPos(key, initPos);
	if ((*keys)[actPos] == key) return; //key is already in array

	/*If the position to insert is not a gap, makes a gap*/
	if (keysb->test(actPos)) makeGap(actPos);

	/*Actually insert key*/
	int i = actPos;
	while(!keysb->test(i)) {
		(*keys)[i] = key;
		i++;
	}

	keysb->flip(actPos);

	numKeys++;
	density = (double)numKeys / keysb->size();
	if (density >= 0.5) expand();
}

int GappedArray::adjPos(double key, int initPos)
{
	if ((*keys)[initPos] == key) return initPos; //key in array
	/*Exponential search if key is less than key at predicted index location*/
	if ((*keys)[initPos] > key) {
		int b = 2;
		while (initPos - b >= 0 && (*keys)[initPos - b] > key) {b *= 2;}
		if (initPos - b < 0) return binSearch(0, initPos - b / 2, key);
		return binSearch(initPos - b, initPos - b / 2, key);
	}
	/*Exponential search if key is greater than key at predicted index location*/
	if ((*keys)[initPos] < key) {
		int b = 2;
		while (initPos + b < keys->size() && (*keys)[initPos + b] < key) { b *= 2; }
		if (initPos + b > keys->size() - 1) return binSearch(initPos + b / 2, keys->size() - 1, key);
		return binSearch(initPos + b / 2, initPos + b, key);
	}
}

int GappedArray::binSearch(int lb, int ub, double key)
{
	while (lb <= ub) {
		int m = lb + (ub - lb) / 2;
		if (key < (*keys)[m]) ub = m - 1;
		else if (key > (*keys)[m]) lb = m + 1;
		else return m;
	}
	return lb;
}

void GappedArray::makeGap(int pos)
{
	int lo = pos;
	int hi = pos;

	while (keysb->test(lo) && keysb->test(hi)) {
		if (lo > 0) lo--;
		if (hi < keysb->size() - 1) hi++;
	}
	if (!keysb->test(hi)) {
		for (int i = hi; i > pos; i--) {
			(*keys)[i] = (*keys)[i - 1];
		}
		keysb->flip(hi);
		keysb->flip(pos);
		return;
	}
	if (!keysb->test(lo)) {
		for (int i = lo; i < pos; i++) {
			(*keys)[i] = (*keys)[i + 1];
		}
		keysb->flip(lo);
		keysb->flip(pos);
		return;
	}
}


void GappedArray::expand()
{
	vector<double> leftx;
	vector<int> lefty;
	vector<double> rightx;
	vector<int> righty;
	int m = numKeys / 2; //how many unique keys until median key
	int j = 1; //counter for unique keys
	int ilmin = 0; //min element index for left side, initialized to stop warnings might not be necessary
	int ilmax = 0; //max element index for left side
	int irmin = 0; //min element index for right side
	int irmax = 0; //max element index for right side


	/*samples 1/100 key/index pairs as well as locates median index*/
	for (int i = 0; i < keysb->size(); i++) {
		if ((*keysb)[i]) {
			if (j % 100 == 0) { //approx 1% of keys
				if (j < m) {
					leftx.push_back((*keys)[i]);
					lefty.push_back(i);
				}
				else {
					rightx.push_back((*keys)[i]);
					righty.push_back(i);
				}
			}
			if (j == 0) { int ilmin = i; }
			if (j == m - 1) { int ilmax = i; }
			if (j == m) { medianIndex = i; int irmin = i; }
			if (j == numKeys) { int irmax = i; break; }
			j++;
		}
	}
	/*if not present adds each side's min and max key/index pairs to vector*/
	/*!!might not be necessary! I'm not sure
	if not necessary, then might not be neccessary to implement a deque and could use a vector for possible performance gains?*/
	if (lefty[0] != ilmin) { leftx.push_back((*keys)[ilmin]); lefty.push_back(ilmin); }
	if (lefty[lefty.size() - 1] != ilmax) { leftx.push_back((*keys)[ilmax]); lefty.push_back(ilmax); }
	if (righty[0] != irmin) { leftx.push_back((*keys)[irmin]); lefty.push_back(irmin); }
	if (righty[righty.size() - 1] != irmax) { leftx.push_back((*keys)[irmax]); lefty.push_back(irmax); }

	/*creates min-max transformers for y values*/
	MinMaxTransformer leftmmt = MinMaxTransformer(ilmin, ilmax, 0, keysb->size() - 1);
	MinMaxTransformer rightmmt = MinMaxTransformer(irmin, irmax, 0, keysb->size() - 1);

	vector<int> lefty_t;
	vector<int> righty_t;
	/*transforms all indexes into ones that fit the new array*/
	for (int i = 0; i < lefty.size(); i++) { lefty_t[i] = (int)leftmmt.y(lefty[i]); }
	for (int i = 0; i < righty.size(); i++) { righty_t[i] = (int)rightmmt.y(righty[i]); }

	/*Figure out how the pointers etc work to assign "left" and "right" to new GappedArrays with Linear Models constructed from leftx,lefty_t and rightx,righty_t respectively*/
	left = new GappedArray(LinearModel(leftx, lefty_t), (*keys)[ilmin]);
	right = new GappedArray(LinearModel(rightx, righty_t), (*keys)[irmin]);

	/*feed all the relevent keys from each half of gappedarray to respective left or right side gapped array*/

	for (int i = ilmin; i <= ilmax; i++) { 
		if ((*keysb)[i]) { 
			left->insert((*keys)[i]); 
		} 
	}
	for (int i = irmin; i <= irmax; i++) { 
		if ((*keysb)[i]) { 
			right->insert((*keys)[i]); 
		} 
	}

	/*destruct the parent array*/
	free(keys);
	free(keysb);

	/*change isLeaf to false, change density to 0, numKeys to 0*/
	isLeaf = false;
	density = 0.0;
	numKeys = 0;
}

GappedArray::~GappedArray() {
	if (!isLeaf) {
		left->~GappedArray();
		right ->~GappedArray();
		return;
	}
	free(keys);
	free(keysb);
	return;
}

bool GappedArray::lookup(double key)
{
	if (!isLeaf) {
		if (lm.y(key) < medianIndex) return left->lookup(key);
		else return right->lookup(key);
	}
	int initPos = (int)lm.y(key); //initial predicted index
	/*accounts for if the model predicts higher or lower than the existing minimum/maximum*/
	if (initPos < 0) initPos = 0;
	if (initPos > keysb->size() - 1) initPos = keysb->size() - 1;
	int actPos = adjPos(key, initPos);
	if ((*keys)[actPos] == key) return true;
	return false;
}