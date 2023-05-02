//
// Created by ferha on 5/03/2023.
//
#include <iostream>
#include <fstream>
#include <iomanip>
#include "State.h"
#include "json.hpp"
#include <vector>
using namespace std;
using json = nlohmann::json;

#ifndef UNTITLED13_DFA_H
#define UNTITLED13_DFA_H


class DFA {
public:
    DFA();

    DFA(string filename);
    DFA(DFA& A, DFA& B, bool w);

    bool accepts(string N);
    State* findState(string Statename);

    void print();

    string geefNaam(string naam);

    string statesToName(vector<State *> states);

    void setStatesSet(const vector<State *> &statesSet);

    void setAlpha(const vector<string> &alpha);

    void setStart(State *start);

    void mergStates(State* A, bool end);

    string mergName(vector<State *> a);

    bool existState(string Statename);

    State* mergFunc(vector<State*> States, bool A);

    vector<State *> sortState(vector<State *> Alpa);
    State* returnSmal(vector<State *> &vec);


private:
    vector<State*> statesSet;
    vector<string> alpha;
    State* start;





};


#endif //UNTITLED13_DFA_H
