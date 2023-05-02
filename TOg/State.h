//
// Created by Robbe Branswyk on 01/03/2023.
//

#ifndef TOI_STATE_H
#define TOI_STATE_H
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;


class State {
private:
    string name;
    vector<pair<string,State*>> transitions;

public:
    void addTransition(string symbol, State* &transitionState);
    vector<State*> getTransitionState(string symbol);
    const vector<pair<string, State *>> &getTransitions() const;


    //Hulpfunctie voor de DFA (als er al een transitie bestaat met symbool -> overschrijven)
    void changeTransition(string symbol, State* endState);

    //Hulpfunctie voor DFA.toRE()
    void changeSymbol(State* endState, const string& symbol);
    bool hasOnlyTransitionToItself();
    pair<bool, string> getExpressionToState(State* endState);


    void deleteALLTransitions();

    void deleteTransitionsToState(State* endState);

    const string &getName() const;
    void setName(const string &name);
};


#endif //TOI_STATE_H
