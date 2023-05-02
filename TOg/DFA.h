//
// Created by Robbe Branswyk on 01/03/2023.
//

#ifndef TOI_DFA_H
#define TOI_DFA_H
#include "Automate.h"
#include "Regex.h"

class DFA : public Automate {
private:
    //properties
    State* currentState;


public:
    //Constructor en Destructor
    DFA(); //constructor die wordt gebruikt bij de omzetting van NFA naar DFA (geen jsonfile nodig)
    explicit DFA(const char *jsonFilename);
    virtual ~DFA();

    //Methods
    void print();
    bool accepts(const string& input);
    Regex toRE() const;


    //Hulpfuncties (niet private, omdat we ze er buiten moeten kunnen gebruiken)
    State* getTransitionState(State* beginState, string symbol);
    void addTransition(State* beginState, string symbol, State* endState);
    State *getCurrentState() const;
    void setCurrentState(State *currentState);

    //toRE
    DFA copyDFA() const;
    vector<State*> sortStates(vector<State*> toSort);
    vector<State*> getAllStatesWithTransTo(State* endState);
    void transitionsToRE();
    string toReAcceptState(State* acceptState) const;
    void deleteAllStatesForRE(vector<State*> statesToDelete);


};


#endif //TOI_DFA_H
