//
// Created by Robbe Branswyk on 03/04/2023.
//

#ifndef TOI_AUTOMATE_H
#define TOI_AUTOMATE_H
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
#include "State.h"
#include "json.hpp"
using json = nlohmann::json;

class Automate {
protected:
    //PROPERTIES
    vector<string> alphabet;
    vector<State*> states;
    State* beginState;
    vector<State*> endStates;

    //Hulpfuncties
    State* getState(const string &name);
    vector<State*> getTransitionState(State* beginState, string symbol);
    void addTransition(State* beginState, string symbol, State* endState);
    void automateToJson(json &jsonFile);
    bool symbolInAlphabet(string &symbol);

    //to DFA
    string makeNewStateName(vector<State*> transitions);
    vector<State*> allTransitions(vector<State*> dfaState, string symbol);
    bool containsEndState(vector<State*> states);

public:
    //METHODS
    explicit Automate();
    virtual bool accepts(const string& input);
    virtual void print();

    //getters en setters
    void setAlphabet(const vector<string> &alphabet);
    const vector<string> &getAlphabet() const;

    void setStates(const vector<State *> &states);
    const vector<State *> &getStates() const;

    void setBeginState(State *beginState);
    State *getBeginState() const;

    void setEndStates(const vector<State *> &endStates);
    const vector<State *> &getEndStates() const;

    void addState(State* newState);
    void addEndstate(State* endState);

    void deleteState(State* stateToDel);

};


#endif //TOI_AUTOMATE_H
