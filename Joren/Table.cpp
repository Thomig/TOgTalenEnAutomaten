//
// Created by Joren Van der Sande on 19/04/2023.
//

#include "Table.h"
#include <algorithm>

Table::Table() {}

Table::Table(vector<string> labels) {
    // sorteren
    std::sort(labels.begin(), labels.end());
    tableLabels = labels;
    // de tabel aanmaken
    for (int i = 0; i < labels.size(); ++i) {
        vector<string> row;
        for (int j = 0; j < labels.size(); ++j) {
            if (j < i) {
                row.emplace_back("-");
            } else {
                row.emplace_back("N");
            }

        }
        table.emplace_back(row);
    }

}

void Table::resetTable() {
    vector<vector<string>> newTable;
    for (int i = 0; i < tableLabels.size(); ++i) {
        vector<string> row;
        for (int j = 0; j < tableLabels.size(); ++j) {
            if (j < i) {
                row.emplace_back("-");
            } else {
                row.emplace_back("N");
            }

        }
        newTable.emplace_back(row);
    }

    table = newTable;
}

int Table::getIndex(const string &name) {
    int index;
    auto it = std::find(tableLabels.begin(), tableLabels.end(), name);
    if (it != tableLabels.end()) {
        index = std::distance(tableLabels.begin(), it);
    } else {
        std::cerr << "the given label name is not found in the vector." << std::endl;
    }
    return index;
}

string Table::getTableItem(const string& first, const string &second) {
    int firstIndex = getIndex(first);
    int secondIndex = getIndex(second);

    string element = table[firstIndex][secondIndex];
    if (element == "N") {
        element = table[secondIndex][firstIndex];
    }
    return element;
}

bool Table::isMarked(const string &first, const string &second) {
    if (first == second) {
        return false;
    } else if (getTableItem(first, second) == "X") {
        return true;
    } else if (getTableItem(first, second) == "-") {
        return false;
    } else if (getTableItem(first, second) == "K") {
        return false;
    } else {
        cerr << "onherkenbaar element in table" << endl;
        return false;
    }
}

void Table::mark(const string &first, const string &second) {
    if (isMarked(first, second)) return;

    int firstIndex = getIndex(first);
    int secondIndex = getIndex(second);

    if (table[firstIndex][secondIndex] != "N") {
        table[firstIndex][secondIndex] = "X";
    } else {
        table[secondIndex][firstIndex] = "X";
    }
}


// Voegt alle combinaties toe die leeg zijn in de horizontale of verticale lijn
void Table::getEquivalentClass(set<string> &givenSet, int i, int j) {
    // Gegeven locatie combinatie toevoegen
    givenSet.insert(tableLabels[i]);
    givenSet.insert(tableLabels[j]);
    table[i][j] = "K";

    // Horizontale lijn checken
    for (int k = 0; k < i; ++k) {
        if (table[i][k] == "X" or table[i][k] == "K") continue; // Als het vakje is aangeduid of het zit al in een set
        //else if (isCombinationInSet(tableLabels[i], tableLabels[k], givenSet)) continue; // Als de combinatie al in de set zit
        else {
            getEquivalentClass(givenSet, i, k);
        }
    }

    // Verticale lijn checken
    for (int k = j+1; k < tableLabels.size(); ++k) {
        if (table[k][j] == "X" or table[k][j] == "K") continue; // Als het vakje is aangeduid of het zit al in een set
            //else if (isCombinationInSet(tableLabels[i], tableLabels[k], givenSet)) continue; // Als de combinatie al in de set zit
        else {
            getEquivalentClass(givenSet, k, j);
        }
    }
}

vector<set<string>> Table::getEquivalentClasses() {
    vector<set<string>> classes;

    // Door de tabel loopen, checken op een leeg vakje en dan in de horizontalen en verticale lijn checken op nog andere vakjes
    for (int i = 0; i < table.size(); ++i) {
        for (int j = 0; j < i; ++j) {
            if (table[i][j] == "X" or table[i][j] == "K") {
                continue;
            } else {
                set<string> currset;
                getEquivalentClass(currset, i, j);
                classes.emplace_back(currset);
            }

        }
    }
    return classes;
}

string Table::tableAsString() {
    string toReturn;
    for (int i = 0; i < table.size()-1; ++i) {
        toReturn += tableLabels[i+1] + "   ";
        for (int j = 0; j < table.size()-1; ++j) {
            if (table[i+1][j] == "N") {
                break;
            } else if (table[i+1][j] == "X") {
                toReturn += "X   ";
            } else if (table[i+1][j] == "-" or table[i+1][j] == "K") {
                toReturn += "-   ";
            }
        }
        toReturn += "\n";
    }
    toReturn += "    ";
    for (int i = 0; i < tableLabels.size()-1; ++i) {
        toReturn += tableLabels[i] + "   " ;
    }

    return toReturn;
}

