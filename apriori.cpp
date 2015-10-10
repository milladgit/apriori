//============================================================================
// Name        : apriori.cpp
// Author      : Millad
// Version     : 0.1
// Copyright   : 
// Description : Apriori algorithm in C++
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <algorithm>
using namespace std;



vector<set<size_t> > *transactions = NULL;

double getTimeInMicroSec() {
    struct timeval time;
    gettimeofday(&time, NULL);

    return time.tv_sec * 1000000 + time.tv_usec;
}

char *readFile(char *inputFilename) {

	char *data = NULL;
	FILE *f = fopen(inputFilename, "rt");
	if(f == NULL)
		return data;
    fseek(f,0,SEEK_END);
    int size = ftell(f);
    fseek(f,0,SEEK_SET);
	data = (char *) malloc(sizeof(char) * size + 1);
	fread(&data[0], sizeof(char), size, f);
	fclose(f);

	return data;
}

void split(const string& s, const string& delim, vector<string> &result, const bool keep_empty = true) {
    if (delim.empty()) {
        result.push_back(s);
        return ;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
}

void splitIntoVector(const char *s1, char delim, vector<string>* v) {
	split(string(s1), string(1, delim), *v, false);
}


void readTransactions(char *inputFilename) {
    hash<string> hash_str;
	char *data = readFile(inputFilename);
	vector<string> lines;
	splitIntoVector(data, '\n', &lines);
	for(int i=0;i<lines.size();i++) {
		set<size_t> trans;
		vector<string> trans_str;
		splitIntoVector(lines[i].c_str(), ' ', &trans_str);
		for(int j=0;j<trans_str.size();j++) {
			trans.insert(hash_str(trans_str[j]));
		}
		transactions->push_back(trans);
	}
	free(data);
}

void showTransactions() {
	for(int i=0;i<transactions->size();i++) {
		set<size_t> lines = (*transactions)[i];
		set<size_t>::iterator j = lines.begin();
		for(;j != lines.end(); j++)
			cout << *j << "\t";
		cout << endl;
	}
}

void printMapSetSizeT(map<set<size_t>, int> *freqItemSet) {
	map<set<size_t>, int>::iterator iter = freqItemSet->begin();
	for(;iter != freqItemSet->end(); iter++) {
		set<size_t> s = iter->first;
		int c = iter->second;

		set<size_t>::iterator iter2 = s.begin();
		for(;iter2 != s.end();iter2++)
			cout << *iter2 << " ";
		cout << "(" << c << ")" << endl;
	}
}

void printSet(set<size_t> s) {
    hash<string> hash_str;

	set<size_t>::iterator iter = s.begin();
	int size = s.size() - 1;
	cout << "[";
	for(int i=0;i<size;i++) {
		cout << *iter << " , ";
		iter++;
	}
	cout << *iter << "]" << endl;
}


void initPassAndSetFreqItemSet(map<set<size_t>, int> *freqItemSet, int minsup_count) {
	map<set<size_t>, int> tempM;
	map<set<size_t>, int>::iterator iter;
	int size = transactions->size();
	cout << "Number of transactions: " << size << endl;
	for(int i=0;i<size;i++) {
		set<size_t> t = (*transactions)[i];

		set<size_t>::iterator item = t.begin();
		for(;item != t.end(); item++) {
			set<size_t> t_set;
			t_set.insert(*item);
			iter = tempM.find(t_set);
			if(iter == tempM.end())
				tempM.insert(std::pair<set<size_t>, int>(t_set, 1));
			else
				iter->second = iter->second + 1;
		}
	}

	iter = tempM.begin();
	for(;iter != tempM.end(); iter++)
		if(iter->second >= minsup_count)
			freqItemSet->insert(std::pair<set<size_t>, int>(iter->first, iter->second));

}

bool isKStepEqual(set<size_t> f1, set<size_t> f2, set<size_t> &s, map<set<size_t>, int> *inputFreqItemSet) {

	s.clear();
	if(f1.size() != f2.size())
		return false;

	set<size_t>::iterator iter1 = f1.begin();
	set<size_t>::iterator iter2 = f2.begin();
	int size = f1.size();
	for(int i=0;i<size-1;i++) {
		if(*iter1 != *iter2) {
			s.clear();
			return false;
		}
		s.insert(*iter1);

		iter1++;
		iter2++;
	}
	s.insert(*iter1);
	s.insert(*iter2);

	return true;
}

bool canBeAdded(set<size_t> &itemset, map<set<size_t>, int> *inputFreqItemSet) {
	int size = itemset.size();
	for(int i=0;i<size-2;i++) {
		set<size_t>::iterator iter = itemset.begin();
		advance(iter, i);

		size_t item = *iter;
		itemset.erase(iter);

		if(inputFreqItemSet->find(itemset) == inputFreqItemSet->end()) {
			itemset.insert(item);
			return false;
		}

		itemset.insert(item);
	}

	return true;
}

bool findItemSetInTransactionSet(set<size_t> &itemset, set<size_t> &transaction) {
	if(itemset.size() > transaction.size())
		return false;

	set<size_t>::iterator itemset_iter = itemset.begin();
	set<size_t>::iterator trans_iter = transaction.begin();
	int intersect_count = 0;
	while(true) {
		if(itemset_iter == itemset.end())
			break;
		if(trans_iter == transaction.end())
			break;

		if(*itemset_iter == *trans_iter) {
			intersect_count++;
			itemset_iter++;
			trans_iter++;
		} else if(*itemset_iter < *trans_iter) {
			itemset_iter++;
		} else {
			trans_iter++;
		}
	}

	return intersect_count == itemset.size();
}

map<set<size_t>, int> * candidateGeneratorAndCounting(map<set<size_t>, int> *freqItemSet, int minsup_count) {
	map<set<size_t>, int> *Ck = new map<set<size_t>, int>();

	map<set<size_t>, int>::iterator f1 = freqItemSet->begin();
	int N = 1;

	for(;f1 != freqItemSet->end(); f1++) {
		map<set<size_t>, int>::iterator f2 = freqItemSet->begin();
		advance(f2, N);
		N++;

		for(;f2 != freqItemSet->end(); f2++) {
			set<size_t> c;
			if(isKStepEqual(f1->first, f2->first, c, freqItemSet)) {
				if(canBeAdded(c, freqItemSet)) {
					int counter = 0;
					vector<set<size_t> >::iterator tIter = transactions->begin();
					for(;tIter != transactions->end(); tIter++)
						if(findItemSetInTransactionSet(c, *tIter))
							counter++;
					if(counter >= minsup_count)
						Ck->insert(pair<set<size_t>, int>(c, counter));
				}
			}
		}
	}

	return Ck;
}

void printMapWithHashcode(char *intputFilename, map<set<size_t>, int> *freqItemSet, char *outputFilename, char *fileMode) {
	bool show_in_console = false;

	if(freqItemSet->size() == 0) {
		cout << "[NO FREQ_SET]\n";
		return;
	}

	map<size_t, string> *m = new map<size_t, string>();
	char *data = readFile(intputFilename);
    hash<string> hash_str;
	vector<string> lines;
	splitIntoVector(data, '\n', &lines);
	for(int i=0;i<lines.size();i++) {
		vector<string> trans_str;
		splitIntoVector(lines[i].c_str(), ' ', &trans_str);
		for(int j=0;j<trans_str.size();j++) {
			size_t h = hash_str(trans_str[j]);
			if(m->find(h) == m->end())
				m->insert(std::pair<size_t, string>(h, trans_str[j]));
		}
	}
	free(data);

	FILE *f = fopen(outputFilename, fileMode);
	map<set<size_t>, int>::iterator iter = freqItemSet->begin();
	for(;iter != freqItemSet->end(); iter++) {
		string temp;
		set<size_t> s = iter->first;
		int c = iter->second;

		set<size_t>::iterator iter2 = s.begin();
		for(;iter2 != s.end();iter2++) {
			temp.append((*m)[*iter2]);
			temp.append(string(" "));
		}
		char temp2[20];
		sprintf(&temp2[0], "(%d)\n", c);
		temp.append(string(temp2));

		if(show_in_console)
			cout << temp;

		fwrite(temp.c_str(), sizeof(char), temp.size(), f);
	}
	fclose(f);

	delete m;
}

void removeUnusedTransactions(map<set<size_t>, int> *freqItemSet) {
	set<int> indexToRemove;
	for(int i=0;i<transactions->size();i++) {
		map<set<size_t>, int >::iterator freqIter = freqItemSet->begin();
		for(;freqIter != freqItemSet->end();freqIter++) {
			set<size_t> s = freqIter->first;
			if(!findItemSetInTransactionSet(s, (*transactions)[i]))
				indexToRemove.insert(i);
		}
	}
	set<int>::iterator indexIter = indexToRemove.begin();
	for(;indexIter != indexToRemove.end();indexIter++) {
		transactions->erase(transactions->begin() + *indexIter);
	}
}

void loopK(map<set<size_t>, int> *freqItemSet, int minsup_count, int K, char *inputFilename, char *outputFilename) {
	int k;
	for(k=2;k<=K;k++) {
		cout << "\tK level: " << k << endl;
		map<set<size_t>, int> *Ck = candidateGeneratorAndCounting(freqItemSet, minsup_count);
		if(Ck->size() == 0) {
			delete Ck;
			return;
		}

		delete freqItemSet;
		freqItemSet = Ck;

		removeUnusedTransactions(freqItemSet);
	}

	printMapWithHashcode(inputFilename, freqItemSet, outputFilename, "w");

	if(freqItemSet->size() == 0)
		return;

	for(;;k++) {
		cout << "\tK level: " << k << endl;
		map<set<size_t>, int> *Ck = candidateGeneratorAndCounting(freqItemSet, minsup_count);
		if(Ck->size() == 0) {
			delete Ck;
			return;
		}

		delete freqItemSet;
		freqItemSet = Ck;

		removeUnusedTransactions(freqItemSet);

		printMapWithHashcode(inputFilename, freqItemSet, outputFilename, "a");
	}

}

int main(int argc, char *argv[]) {

	transactions = new vector<set<size_t> >();
	double t1;

	int minsup_count = atoi(argv[1]);
	int K = atoi(argv[2]);
	char *inputFilename = argv[3];
	char *outputFilename = argv[4];

	t1 = getTimeInMicroSec();
	readTransactions(inputFilename);
	t1 = getTimeInMicroSec() - t1;
	cout << "\nElapsed time in reading file: " << t1 << "us" << endl;

	map<set<size_t>, int> *freqItemSet = new map<set<size_t>, int>();
	t1 = getTimeInMicroSec();
	initPassAndSetFreqItemSet(freqItemSet, minsup_count);
	t1 = getTimeInMicroSec() - t1;
	cout << "\nElapsed time in init pass and first freq set: " << t1 << "us" << endl;

	t1 = getTimeInMicroSec();
	loopK(freqItemSet, minsup_count, K, inputFilename, outputFilename);
	t1 = getTimeInMicroSec() - t1;
	cout << "\nElapsed time in loopK: " << t1 << "us" << endl;
	cout << "Elapsed time in loopK: " << t1/1000000 << "s" << endl;

	cout << "\nResults are available at: " << outputFilename << endl;

	return 0;
}
