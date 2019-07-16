#include "GappedArray.h"

GappedArray::GappedArray(LinearModel model, double minKey)
{
	keys = new array<double, 4096>;
	keys->fill(minKey);
	keysb = new bitset<4096>;
	keysb->reset();
	keysb->flip(0);
	lm = model;
	isLeaf = true;
	numKeys = 1;
	density = (double)1 / keysb->size();
	MEDIND = (int) keysb->size() / 2;
	MAXIND = (int) keysb->size() - 1;
	left = NULL;
	right = NULL;
}

void GappedArray::insert(double key)
{
	if (!isLeaf) {
		if (lm.y(key) < MEDIND) { left->insert(key); }
		else { right->insert(key); }
		return;
	}

	int initPos = (int)lm.y(key);

	/*check if in sorted order*/
	int actPos = adjPos(key, initPos);
	if ((*keys)[actPos] == key) return; // key already in keys
										
    /*can safely assume actPos is either the key immediately larger than key, the max position, the minimum position
	or a gap in sorted order*/
		
	/*this part is a MESS but it seems to work*/
	if (!(*keysb)[actPos]) {
		(*keys)[actPos] = key;
		(*keysb)[actPos].flip();
	}
	else if (actPos == MAXIND) {
		if (key < (*keys)[actPos]) { actPos--; }
		int i = actPos;
		while ((*keysb)[i]) {
			i--;
		}
		(*keysb)[i].flip();
		while (i < actPos) {
			(*keys)[i] = (*keys)[i + 1];
			i++;
		}
		(*keys)[actPos] = key;
	}
	else if (actPos == 0) {
		if ((*keys)[actPos] < key) {
			actPos++;
		}
		int i = actPos;
		while ((*keysb)[i]) {
			i++;
		}
		(*keysb)[i].flip();
		while (i > actPos) {
			(*keys)[i] = (*keys)[i - 1];
			i--;
		}
		(*keys)[actPos] = key;
	}
	else {
		int lo = actPos;
		int hi = actPos;
		while ((*keysb)[lo] && (*keysb)[hi]) {
			if (lo > 0) lo--;
			if (hi < MAXIND) hi++;
		}
		if (!(*keysb)[hi]) {
			if(key > (*keys)[actPos]) actPos++;
			if (hi == actPos) {
				(*keys)[actPos] = key;
				(*keysb)[actPos].flip();
			}
			else {
				(*keysb)[hi].flip();
				while (hi > actPos) {
					(*keys)[hi] = (*keys)[hi - 1];
					hi--;
				}
				(*keys)[actPos] = key;
			}
		}
		else if (!(*keysb)[lo]) {
			actPos--;
			if (lo == actPos) {
				(*keys)[actPos] = key;
				(*keysb)[actPos].flip();
			}
			else {
				(*keysb)[lo].flip();
				while (lo < actPos) {
					(*keys)[lo] = (*keys)[lo + 1];
					lo++;
				}
				(*keys)[actPos] = key;
			}
		}
	}


	/*makes sure key is repeated until next key*/
	int j = actPos;
	while (j+1 <= MAXIND && (*keys)[j + 1] < (*keys)[j]) {
		(*keys)[j + 1] = (*keys)[j];
		j++;
	}

	numKeys = (*keysb).count();
	density = (double)(*keysb).count() / (*keysb).size();
	if (density > 0.5) expand();
}





int GappedArray::adjPos(double key, int initPos)
{
	/*edgecases*/
	if (initPos < 0) initPos = 0;
	if (initPos > MAXIND) initPos = MAXIND;
	if ((*keys)[initPos] == key) return initPos; //key already in keys
	if ((*keys)[0] >= key) return 0; //key smaller than minimum key

	int actPos = initPos;

	if (key > (*keys)[actPos]) {
		int b = 1;
		while (actPos + b <= MAXIND && key > (*keys)[actPos + b]) { b *= 2; }
		if (actPos + b > MAXIND) { actPos = binSearch(actPos + b / 2, MAXIND, key); }
		else actPos = binSearch(actPos + b / 2, actPos + b, key);
	}
	else if (key < (*keys)[actPos]) {
		int b = 1;
		while (actPos - b >= 0 && key < (*keys)[actPos - b]) { b *= 2; }
		if (actPos - b < 0) { actPos = binSearch(0, actPos - b / 2, key); }
		else { actPos = binSearch(actPos - b, actPos - b / 2, key); }
	}
	
	
	if ((*keys)[actPos] == key) return actPos; //key was found by binary search
	
	/*get as close to initPos as possible while maintaining sorted order*/
	if (initPos < actPos) {
		int j = actPos;
		while(j > initPos) {
			j--;
			if (j == 0 || (*keysb)[j]) break;
		}
		return j + 1;
	}
	return actPos;
}



int GappedArray::binSearch(int lb, int ub, double key)
{
	while (lb <= ub) {
		if ((*keys)[lb] == (*keys)[ub]) break;
		int m = lb + (ub - lb) / 2;
		if (key < (*keys)[m]) ub = m - 1;
		else if (key > (*keys)[m]) lb = m + 1;
		else return m;
	}
	return lb;
}

//void GappedArray::expand()
//{
//	vector<double> leftx;
//	vector<int> lefty;
//	vector<double> rightx;
//	vector<int> righty;
//	int m = numKeys / 2; //how many unique keys until median key
//	int j = 1; //counter for unique keys
//	int ilmin = 0; //min element index for left side, initialized to stop warnings might not be necessary
//	int ilmax = 0; //max element index for left side
//	int irmin = 0; //min element index for right side
//	int irmax = 0; //max element index for right side
//
//
//	/*samples 1/100 key/index pairs as well as locates median index*/
//	for (int i = 0; i < keysb->size(); i++) {
//		if ((*keysb)[i]) {
//			if (j % 100 == 0) { //approx 1% of keys
//				if (j < m) {
//					leftx.push_back((*keys)[i]);
//					lefty.push_back(i);
//				}
//				else {
//					rightx.push_back((*keys)[i]);
//					righty.push_back(i);
//				}
//			}
//			if (j == 0) { int ilmin = i; }
//			if (j == m - 1) { int ilmax = i; }
//			if (j == m) { medianIndex = i; int irmin = i; }
//			if (j == numKeys) { int irmax = i; break; }
//			j++;
//		}
//	}
//	/*if not present adds each side's min and max key/index pairs to vector*/
//	/*!!might not be necessary! I'm not sure
//	if not necessary, then might not be neccessary to implement a deque and could use a vector for possible performance gains?*/
//	if (lefty[0] != ilmin) { leftx.push_back((*keys)[ilmin]); lefty.push_back(ilmin); }
//	if (lefty[lefty.size() - 1] != ilmax) { leftx.push_back((*keys)[ilmax]); lefty.push_back(ilmax); }
//	if (righty[0] != irmin) { leftx.push_back((*keys)[irmin]); lefty.push_back(irmin); }
//	if (righty[righty.size() - 1] != irmax) { leftx.push_back((*keys)[irmax]); lefty.push_back(irmax); }
//
//	/*creates min-max transformers for y values*/
//	MinMaxTransformer leftmmt = MinMaxTransformer(ilmin, ilmax, 0, keysb->size() - 1);
//	MinMaxTransformer rightmmt = MinMaxTransformer(irmin, irmax, 0, keysb->size() - 1);
//
//	vector<int> lefty_t;
//	vector<int> righty_t;
//	/*transforms all indexes into ones that fit the new array*/
//	for (int i = 0; i < lefty.size(); i++) { lefty_t[i] = (int)leftmmt.y(lefty[i]); }
//	for (int i = 0; i < righty.size(); i++) { righty_t[i] = (int)rightmmt.y(righty[i]); }
//
//	/*Figure out how the pointers etc work to assign "left" and "right" to new GappedArrays with Linear Models constructed from leftx,lefty_t and rightx,righty_t respectively*/
//	left = new GappedArray(LinearModel(leftx, lefty_t), (*keys)[ilmin]);
//	right = new GappedArray(LinearModel(rightx, righty_t), (*keys)[irmin]);
//
//	/*feed all the relevent keys from each half of gappedarray to respective left or right side gapped array*/
//
//	for (int i = ilmin; i <= ilmax; i++) { 
//		if ((*keysb)[i]) { 
//			left->insert((*keys)[i]); 
//		} 
//	}
//	for (int i = irmin; i <= irmax; i++) { 
//		if ((*keysb)[i]) { 
//			right->insert((*keys)[i]); 
//		} 
//	}
//
//	/*destruct the parent array*/
//	free(keys);
//	free(keysb);
//
//	/*change isLeaf to false, change density to 0, numKeys to 0*/
//	isLeaf = false;
//	density = 0.0;
//	numKeys = 0;
//}

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
		if (lm.y(key) < MEDIND) return left->lookup(key);
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

void GappedArray::print(string outputloc)
{
	if (!isLeaf) {
		left->print(outputloc);
		right->print(outputloc);
		return;
	}
	ofstream outfile;// declaration of file pointer named outfile
	outfile.open(outputloc, ios::out); // opens file named "filename" for output

	
	outfile << "Gapped Array Output\n";
	outfile << "Linear Model\ty=" << lm.slope() << "x + " << lm.intercept() << "\n";
	
	int min = 0;
	int max = MAXIND;

	while (!(*keysb)[min]) min++;
	while (!(*keysb)[max]) max--;

	
	outfile << "Min Key:\t" << fixed << (*keys)[min] << "\tMax Key:\t" << fixed << (*keys)[max] << "\n";
	outfile << "Array in order:\t" << testOrder() << "\n";

	outfile << "Number Keys:\t" << (*keysb).count() << "\n"; 


	outfile << "Bitset:\t\t" << keysb->to_string() << "\n";
	for (int i = 0; i < keys->size(); i++) {
		outfile << "Position " << i << "\t";
		outfile << fixed << (*keys)[i] << "\t";
		outfile << "BitPos=" << (*keysb)[i] << "\t";
		if ((i > 0) && ((*keys)[i] < (*keys)[i - 1])) {
			outfile << "Out of order" << "\t";
		}
		outfile << "\n";
	}

	outfile.close();// closes file; always do this when you are done using the file
}

bool GappedArray::testOrder()
{
	for (int i = 0; i < MAXIND; i++) {
		if ((*keys)[i] > (*keys)[i + 1]) return false;
	}
	return true;
}