//
// Created by ferha on 13/04/2023.
//

#ifndef TA_NFA_H
#define TA_NFA_H
#include "DFA.h"
#include <algorithm>

class NFA {
public:
    NFA();

    NFA(string filename);

    bool accepts(string N);
    State* findState(string Statename);
    bool existState(string Statename);

    void print();

    DFA toDFA();

    State* mergeState(vector<State*> States);

    string mergName(vector<State*> a);

    void makeStateSet(State* start, vector<State*> &newset);

    void stateTransition(State* a);

    vector<string> splitState(string);

    State* returnSmal(vector<State*> &vec);
    vector<State*> sortState(vector<State*> Alpa);

    string statesToName(vector<State*> states);

    const vector<State *> &getStatesSet() const;

    const vector<string> &getAlpha() const;

    State *getStart() const;



private:
    vector<State*> statesSet;
    vector<string> alpha;
    State* start;





};


#endif //TA_NFA_H
