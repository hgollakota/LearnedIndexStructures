#include "GappedArray.h"

GappedArray::GappedArray(LinearModel model, double minKey, int depth)
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
	MAXIND = (int) ARRSIZE - 1;
	left = NULL;
	right = NULL;
	DEPTH = depth;
}

bool GappedArray::insert(double key)
{
	if (!isLeaf) {
		if (lm.y(key) < MEDIND) { return left->insert(key); }
		else { return right->insert(key); }
	}

	int initPos = (int)lm.y(key);

	/*check if in sorted order*/
	int actPos = adjPos(key, initPos);
	if ((*keys)[actPos] == key) return false; // key already in keys
										
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
	return true;
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

	int sampled = 100 / SAMPLE_PERCENTAGE;

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

			if (j % sampled == 1) { //approx SAMPLE_PERCENTAGE% of keys
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

	left = new GappedArray(llm, (*keys)[lmin], DEPTH+1);
	right = new GappedArray(rlm, (*keys)[rmin], DEPTH+1);

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
	keys = NULL;
	free(keysb);
	keysb = NULL;
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
		right->~GappedArray();
		free(left);
		left = NULL;
		free(right);
		right = NULL;
		return;
	}
	free(keys);
	keys = NULL;
	free(keysb);
	keysb = NULL;
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
	int actPos = initPos; //exponential search for key
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
	outfile << "Depth:\t" << DEPTH << "\n";
	
	int min = 0;
	int max = MAXIND;

	while (!(*keysb)[min]) min++;
	while (!(*keysb)[max]) max--;

	
	outfile << "Min Key: " << fixed << (*keys)[min] << "\t\tMax Key: " << fixed << (*keys)[max] << "\n";
	outfile << "Array in order:\t" << testOrder() << "\n";
	outfile << "Number Keys:\t" << (*keysb).count() << "\t\t" << "ARRSIZE:\t" << ARRSIZE << "\n"; 
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
	outfile << "Depth:\t" << DEPTH << "\n";
	int min = 0;
	int max = MAXIND;
	while (!(*keysb)[min]) min++;
	while (!(*keysb)[max]) max--;
	outfile << "Min Key: " << fixed << (*keys)[min] << "\t\tMax Key: " << fixed << (*keys)[max] << "\n";
	outfile << "Array in order:\t" << testOrder() << "\n";
	outfile << "Number Keys:\t" << (*keysb).count() << "\t\t" << "ARRSIZE:\t" << ARRSIZE << "\n";
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

int GappedArray::numLeaves()
{
	if (isLeaf) return 1;
	return left->numLeaves() + right->numLeaves();
}

double GappedArray::avgLeafDepth()
{
	long totDepth = aldh();
	int leaves = numLeaves();

	return (double)totDepth / (double)leaves;
}

long GappedArray::aldh()
{
	if (isLeaf) return DEPTH;
	return left->aldh() + right->aldh();
}

GA::GA()
{
	numKeys = 0;
	root = NULL;
	mmt = NULL;
	lm = NULL;
	initializing = true;
	initSize = 50;
	initkeys = new set<double>;
}

GA::GA(int initialSize)
{
	numKeys = 0;
	root = NULL;
	mmt = NULL;
	lm = NULL;
	initializing = true;
	if (initialSize < 1) initialSize = 50;
	initSize = initialSize;
	initkeys = new set<double>;
}

GA::~GA()
{
	if (!initializing) {
		root->~GappedArray();
		free(root);
		root = NULL;
	}
	free(initkeys);
	initkeys = NULL;
	free(lm);
	lm = NULL;
	free(mmt);
	mmt = NULL;
}

bool GA::insert(double key)
{
	if (!initializing) {
		bool success = root->insert(key);
		if (success) numKeys++;
		return success;
	}
	if (contains(key)) return false;
	initkeys->insert(key);
	numKeys++;
	if (numKeys >= initSize) initialize();
	return true;
}

void GA::insertFromFile(string inputloc)
{
	ifstream fin(inputloc);
	if (!fin.is_open()) {
		cout << "Input file at: " << inputloc << " could not be opened\n";
		return;
	}
	double d = 0.0;
	size_t keys = 0;
	size_t fails = 0;
	bool success = true;
	while (fin >> d) {
		success = insert(d);
		if (!success) fails++;
		keys++;
	}
	if (fails == 0) cout << "Successfully inserted all " << keys << " keys\n";
	else cout << "Failed to insert " << fails << " / " << keys << " keys\n";
}

void GA::initialize()
{
	mmt = new MinMaxTransformer(0, initkeys->size() - 1, 0, ARRSIZE - 1);
	vector<double> initx;
	vector<int> inity;
	double d = 0.0;
	for (auto elem : (*initkeys)) {
		initx.emplace_back(elem);
		inity.emplace_back((int)mmt->y(d));
		d++;
	}
	lm = new LinearModel(initx, inity);
	root = new GappedArray((*lm), initx[0], 0);
	for (size_t i = 1; i < initx.size(); i++) { root->insert(initx[i]); }
	initializing = false;
	free(initkeys);
	initkeys = NULL;
	free(mmt);
	mmt = NULL;
	free(lm);
	lm = NULL;
}

int GA::count()
{
	return (int)numKeys;
}

bool GA::contains(double key)
{
	if (!initializing) return root->lookup(key);
	return (initkeys->count(key) == 1);
}

void GA::testLookup(string inputloc)
{
	ifstream fin(inputloc);
	if (!fin.is_open()) { cout << "Could not open file " << inputloc << "\n"; return; }
	string s = "";
	double d = 0.0;
	size_t keys = 0;
	size_t errors = 0;
	while (fin >> s) {
		d = textToDouble(s);
		if (!contains(d)) {
			cout << "Error finding key:\t" << std::numeric_limits<double>::digits10 + 2 << d << "\n";
			errors++;
		}
		keys++;
	}
	cout << "Errors finding " << errors << " / " << keys << "  = " << ((double) errors / (double) keys)*100 << "% of keys\n";
	fin.close();
}
void GA::testLookup(string inputloc, string outputloc)
{
	ifstream fin(inputloc);
	if (!fin.is_open()) { cout << "Could not open file " << inputloc << "\n"; return; }
	ofstream fo;// declaration of file pointer named outfile
	fo.open(outputloc, ios::out | ofstream::app);
	if (!fo.is_open()) { cout << "Weird error, should have been able to create an output file...\n"; fin.close(); return; }
	string s = "";
	double d = 0.0;
	size_t keys = 0;
	size_t errors = 0;
	while (fin >> s) {
		d = textToDouble(s);
		if (!contains(d)) {
			cout << "Error finding key:\t" << std::numeric_limits<double>::digits10 + 2 << d << "\n";
			fo << std::numeric_limits<double>::digits10 + 2 << d << "\n";
			errors++;
		}
		keys++;
	}
	cout << "Errors finding " << errors << " / " << keys << "  = " << ((double)errors / (double)keys) * 100 << "% of keys\n";
	fin.close();
	fo.close();
}

bool GA::sorted()
{
	if (!initializing) return root->testOrder();
	return true; //a set is always sorted
}

int GA::numLeaves()
{
	if (!initializing) return root->numLeaves();
	return 0;
}

double GA::avgLeafDepth()
{
	if (!initializing) return root->avgLeafDepth();
	return 0.0;
}

double GA::avgKeysPerLeaf()
{
	if (initializing) return 0.0;
	return (double)count() / numLeaves();
}

void GA::print(string outputloc)
{
	ofstream outfile;// declaration of file pointer named outfile
	outfile.open(outputloc, ios::out | ofstream::app); // opens file named "filename" for output
	outfile << "~~Overall GA Output~~\n";
	outfile << "Total Number of Keys:\t" << count() << "\t\t";
	outfile << "Total Number of Leaves:\t" << numLeaves() << "\n";
	outfile << "Average Keys per Leaf:\t" << avgKeysPerLeaf() << "\n";
	outfile << "Array Size: " << ARRSIZE << "\t\tMaximum Density: " << DENSITY << "\n";
	outfile << "Average Leaf Depth:\t" << avgLeafDepth() << "\n";
	outfile << "All GAs Sorted: " << sorted() << "\n\n";
	if (initializing) {	
		for (auto elem : (*initkeys)) { outfile << fixed << elem << "\n"; } 
	}
	outfile.close(); //always close outfile
	if (!initializing) { root->print(outputloc); }
}

void GA::printhead(string outputloc)
{
	ofstream outfile;// declaration of file pointer named outfile
	outfile.open(outputloc, ios::out | ofstream::app); // opens file named "filename" for output
	outfile << "~~Overall GA Output~~\n";
	outfile << "Total Number of Keys:\t" << count() << "\t\t";
	outfile << "Total Number of Leaves:\t" << numLeaves() << "\n";
	outfile << "Average Keys per Leaf:\t" << avgKeysPerLeaf() << "\n";
	outfile << "Array Size: " << ARRSIZE << "\t\tMaximum Density: " << DENSITY << "\n";
	outfile << "Average Leaf Depth:\t" << avgLeafDepth() << "\n";
	outfile << "All GAs Sorted: " << sorted() << "\n\n";
	outfile.close(); //always close outfile
	if (!initializing) { root->printhead(outputloc); }
}

void GA::info()
{
	cout << "\n";
	cout << "~~Overall GA Information~~\n";
	cout << "Total Number of Keys:\t" << count() << "\t\t";
	cout << "Total Number of Leaves:\t" << numLeaves() << "\n";
	cout << "Average Keys per Leaf:\t" << avgKeysPerLeaf() << "\n";
	cout << "Array Size: " << ARRSIZE << "\t\tMaximum Density: " << DENSITY << "\n";
	cout << "Average Leaf Depth:\t" << avgLeafDepth() << "\n";
	cout << "All GAs Sorted: " << sorted() << "\n\n";
}

double GA::textToDouble(const std::string& str)
{
	return strtod(str.c_str(), NULL);
}