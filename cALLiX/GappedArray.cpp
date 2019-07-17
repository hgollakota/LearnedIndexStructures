#include "GappedArray.h"

GappedArray::GappedArray(LinearModel model, double minKey)
{
	keys = new array<double, ARRSIZE>;
	keys->fill(minKey);
	keysb = new bitset<ARRSIZE>;
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
	/*either inserts key or makes gap then inserts key*/
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
	if (density > DENSITY) expand();
}

void GappedArray::expand()
{
	vector<double> leftx;
	vector<int> lefty;
	vector<double> rightx;
	vector<int> righty;

	int lmin;
	int lmax;
	int rmin;
	int rmax;

	int m = (*keysb).count() / 2; //number
	int i = 0;
	int j = 0;
	while (i < (*keysb).size()) {
		if ((*keysb)[i]) {
			j++;
			if (j == 1) { lmin = i; }
			else if (j + 1 == m) { lmax = i; }
			else if (j == m) { rmin = i; MEDIND = i; }
			else if (j == (*keysb).count()) { rmax = i; }

			if (j % 20 == 1) { //approx 5% of keys
				if (j < m) {
					leftx.push_back((*keys)[i]);
					lefty.push_back(i);
				}
				else {
					rightx.push_back((*keys)[i]);
					righty.push_back(i);
				}
			}
		}
		i++;
	}

	MinMaxTransformer lmmt = MinMaxTransformer(lmin, lmax, 0, MAXIND);
	MinMaxTransformer rmmt = MinMaxTransformer(rmin, rmax, 0, MAXIND);

	vector<int> lefty_t;
	vector<int> righty_t;

	for (int k = 0; k < lefty.size(); k++) { lefty_t.push_back((int)lmmt.y(lefty[k])); }
	for (int k = 0; k < righty.size(); k++) { righty_t.push_back((int)rmmt.y(righty[k])); }

	LinearModel llm = LinearModel(leftx, lefty_t);
	LinearModel rlm = LinearModel(rightx, righty_t);

	left = new GappedArray(llm, (*keys)[lmin]);
	right = new GappedArray(rlm, (*keys)[rmin]);

	for (int k = 0; k < (*keys).size(); k++) {
		if ((*keysb)[k]) {
			if (lm.y((*keys)[k]) < MEDIND) {
				left->insert((*keys)[k]);
			}
			else {
				right->insert((*keys)[k]);
			}
		}
	}

	free(keys);
	free(keysb);
	density = 0.0;
	isLeaf = false;
	return;
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
		if ((int)lm.y(key) < MEDIND) return left->lookup(key);
		else return right->lookup(key);
		
	}
	int initPos = lm.y(key);
	if (initPos < 0) initPos = 0;
	if (initPos > MAXIND) initPos = MAXIND;
	if ((*keys)[initPos] == key) return true;
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
	outfile.open(outputloc, ios::out | ofstream::app); // opens file named "filename" for output

	
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
	return;
}

void GappedArray::printhead(string outputloc)
{
	if (!isLeaf) {
		left->printhead(outputloc);
		right->printhead(outputloc);
		return;
	}
	ofstream outfile;// declaration of file pointer named outfile
	outfile.open(outputloc, ios::out | ofstream::app); // opens file named "filename" for output
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

	outfile.close();
	return;

}

bool GappedArray::testOrder()
{
	if (!isLeaf) {
		return left->testOrder() && right->testOrder();
	}
	for (int i = 0; i < MAXIND; i++) {
		if ((*keys)[i] > (*keys)[i + 1]) return false;
	}
	return true;
}