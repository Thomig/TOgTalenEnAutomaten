//
// Created by Robbe Branswyk on 03/04/2023.
//

#include "Automate.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
using namespace std;

/*******************************************************
 * Functies die worden overschreven in Derived classes *
 *******************************************************/
Automate::Automate() {}
void Automate::print() {}
bool Automate::accepts(const string& input) {return true;}


/************
 * Functies *
 ************/
void Automate::setAlphabet(const vector<string> &alphabet) {
    Automate::alphabet = alphabet;
}

void Automate::setStates(const vector<State *> &states) {
    Automate::states = states;
}

void Automate::setBeginState(State *beginState) {
    Automate::beginState = beginState;
}

void Automate::setEndStates(const vector<State *> &endStates) {
    Automate::endStates = endStates;
}

void Automate::addState(State *newState) {
    states.push_back(newState);
}

const vector<State *> &Automate::getStates() const {
    return states;
}

/****************
 * Hulpfuncties *
 ****************/
void Automate::addTransition(State *beginState, string symbol, State *endState) {
    /**
     Deze functie voegt een transitie toen aan een Staat
     Deze wordt gebruikt omdat er een onderscheid is bij bv. DFA en NFA, dus wordt overschreven in de klasse DFA

     Wordt gebruikt door: NFA
     */
    beginState->addTransition(symbol, endState);
}

void Automate::automateToJson(json &jsonFile) {
    /**
     hulpfunctie om het algemene deel van een Automaat toe te voegen aan de json File
     Het toevoegen van het alfabet, staten en transities

     Wordt gebruikt in de "print" door: DFA, NFA
     */
    for (string c : alphabet){
        jsonFile["alphabet"].push_back(c);
    }

    for (State* state : states){
        string stateName = state->getName();
        bool starting = false;
        bool accepting = false;

        if (state == beginState){
            starting = true;
        }

        auto it = find(endStates.begin(), endStates.end(), state);
        if (it != endStates.end()){
            accepting = true;
        }

        jsonFile["states"].push_back( {
                                         {"name", stateName},
                                         {"starting", starting},
                                         {"accepting", accepting}});

        for (string c : alphabet){
            auto transitions = state->getTransitionState(c);
            for (auto i : transitions){
                jsonFile["transitions"].push_back({
                                                          {"from", stateName},
                                                          {"to", i->getName()},
                                                          {"input", c}
                                                  });
            }
        }
    }
}

bool Automate::symbolInAlphabet(string &symbol) {
    /**
     Kijkt na of een string in het alfabet zit

     Wordt gebruikt in "accepts" door: DFA, NFA
     */
    return find(alphabet.begin(), alphabet.end(), symbol) != alphabet.end();
}

State *Automate::getState(const string &name) {
    /**
     Geeft pointer naar State terug met de gegeven naam
     */
    for(State* s : states){
        if (s->getName() == name){
            return s;
        }
    }
    return nullptr;
}

vector<State*> Automate::getTransitionState(State *beginState, string symbol) {
    /**
        Functie om pointer naar State terug te geven als een symbool wordt gelezen
        Wordt overschreven in DFA (return: State* ipv vector<State*>)

        Wordt gebruikt door: NFA
     */
    return beginState->getTransitionState(symbol);
}

string Automate::makeNewStateName(vector<State*> transitions) {
    /**
     Creeert een nieuwe string naam adhv vector
     Zet de states om in bv: {p,q} (gescheiden door komma, omgrensd door acolades, alfabetische volgorde)
     */
    //Vector in alfabetische volgorde zetten
    vector<string> alfabetisch;
    for (State* i : transitions){
        alfabetisch.push_back(i->getName());
    }
    sort(alfabetisch.begin(), alfabetisch.end());

    string result = "{";
    for (int i = 0; i < alfabetisch.size(); i++) {
        result += alfabetisch[i];
        if (i != alfabetisch.size() -1){
            result += ",";
        }
    }
    result += "}";

    return result;

}

vector<State *> Automate::allTransitions(vector<State *> dfaState, string symbol) {
    /**
     Geeft al de transities weer van een vector van State met een symbol
        bv: vector {p,q} met symbol a, zal al de states teruggeven met transitie(p,a) en (q,a)
     */
    vector<State*> newDFAState;
    for (State* state : dfaState){
        vector<State*> transitionsToSymbol = getTransitionState(state, symbol);
        for(State* i : transitionsToSymbol){
            if (find(newDFAState.begin(), newDFAState.end(), i) == newDFAState.end()){
                newDFAState.push_back(i);
            }
        }
    }

    return newDFAState;
}

bool Automate::containsEndState(vector<State *> states) {
    bool result = false;
    for(State* state : states){
        if(find(endStates.begin(), endStates.end(), state) != endStates.end()){
            result = true;
        }
    }

    return result;
}

void Automate::addEndstate(State *endState) {
    endStates.push_back(endState);
}

State *Automate::getBeginState() const {
    return beginState;
}

const vector<string> &Automate::getAlphabet() const {
    return alphabet;
}

const vector<State *> &Automate::getEndStates() const {
    return endStates;
}

void Automate::deleteState(State *stateToDel) {
    /**
     verwijdert variabele state uit de automaat EN al de transities naar deze staat
     préconditie: dit mag niet de startStaat zijn!!!
     */
    auto it = find(states.begin(), states.end(), stateToDel);
    states.erase(it);

    auto iteratorEndState = find(endStates.begin(), endStates.end(), stateToDel);
    if (iteratorEndState != endStates.end()){
        endStates.erase(iteratorEndState);
    }

    //Al de transities naar deze staat verwijderen
    for(State* state : getStates()){
        state->deleteTransitionsToState(stateToDel);
    }

    delete stateToDel;
}




