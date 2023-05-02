//
// Created by Joren Van der Sande on 05/03/2023.
//

#ifndef PROGRAMMEEROPDRACHT1_DFA_H
#define PROGRAMMEEROPDRACHT1_DFA_H

#include "Table.h"
#include "Automaat.h"

#include <tuple>
#include <vector>
#include <string>
#include <map>

using namespace std;

//class State;

class DFA : public Automaat{
private:
    Table table;

public:
    DFA();
    explicit DFA(string fileName);
    DFA(const DFA& other); // Copy constructor

    bool accepts(string input);

    void fillTable();
    DFA minimize();
    void printTable();

    bool operator==(const DFA& other) const;
};




#endif //PROGRAMMEEROPDRACHT1_DFA_H
