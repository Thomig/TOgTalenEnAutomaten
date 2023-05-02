//
// Created by Joren Van der Sande on 29/04/2023.
//

#ifndef PROGRAMMEEROPDRACHT1_AUTOMAAT_H
#define PROGRAMMEEROPDRACHT1_AUTOMAAT_H

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

class State;

class Automaat {
protected:
    string type;
    vector<string> alphabet;
    vector<State*> states; // moet op index gerangschikt zijn, als het er niet is: nullptr -> todo zeker dat dit klopt?
public:
    State* getState(const string &name) const;
    vector<State*> getAllStates() const;
    State* getStartState() const;
    vector<State*> getEndStates() const;

    void addState(State* state);

    Automaat();
    ~Automaat();

    void print() const;

};


class State {
public:
    State(string name, map<string, State*> nextStateList, bool starting, bool accepting);

    string name;
    map<string, State*> nextStateList; // Alle volgende staten waar je naartoe kan gaan vanaf deze staat
    bool starting;
    bool accepting;
};

#endif //PROGRAMMEEROPDRACHT1_AUTOMAAT_H
