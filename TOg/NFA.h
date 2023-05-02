//
// Created by Robbe Branswyk on 03/04/2023.
//

#ifndef TOI_NFA_H
#define TOI_NFA_H
#include "Automate.h"
#include "DFA.h"

class NFA: public Automate{
private:
    //properties
    vector<State*> currentState;
    //Hulpfuncties
        //getTransition gebruiken uit superclass Automate
        //addTransition gebruiken uit superclass Automate
    void transitionsForTheNewDFAState(State* &newState, vector<State*> transitions);
    State* toDFAState(const string& stateName);

public:
    //Constructor en Destructor
    NFA(const char *jsonFilename);
    virtual ~NFA();

    //Methods
    void print();
    bool accepts(const string& input);
    DFA toDFA();
};


#endif //TOI_NFA_H
