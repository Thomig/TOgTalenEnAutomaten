//
// Created by Joren Van der Sande on 19/04/2023.
//

#ifndef PROGRAMMEEROPDRACHT1_TABLE_H
#define PROGRAMMEEROPDRACHT1_TABLE_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>

using namespace std;

class DFA;

class Table {
private:
    vector<vector<string>> table;
    vector<string> tableLabels;

    int getIndex(const string &name);
    string getTableItem(const string& first, const string &second);
    void getEquivalentClass(set<string> &givenSet, int i, int j);

public:
    Table();
    Table(vector<string> labels);

    void resetTable();

    bool isMarked(const string &first, const string &second); // Geeft terug of de tabel gemarkeerd is op de gegeven plek
    void mark(const string &first, const string &second); // Markeer een plek

    vector<set<string>> getEquivalentClasses();

    string tableAsString();
};


#endif //PROGRAMMEEROPDRACHT1_TABLE_H
