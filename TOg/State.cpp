//
// Created by Robbe Branswyk on 01/03/2023.
//

#include "State.h"

void State::addTransition(string symbol, State* &transitionState) {
    transitions.push_back({symbol, transitionState});
}

vector<State*> State::getTransitionState(string symbol) {
    vector<State*> transitionStates;
    for (auto transition : transitions){
        if (transition.first == symbol){
            transitionStates.push_back(transition.second);
        }
    }
    return transitionStates;
}

const vector<pair<string, State *>> &State::getTransitions() const {
    return transitions;
}

const string &State::getName() const {
    return name;
}

void State::setName(const string &name) {
    State::name = name;
}

void State::changeTransition(string symbol, State *endState) {
    /**
     In DFA verandert de eindstaat in de Transitie met symbol
     */
    for(auto &transition : transitions){
        if (transition.first == symbol){
            transition.second = endState;
        }
    }
}

void State::changeSymbol(State *endState, const string& symbol) {
    /**
     In DFA verandert het symbool in de transitie naar endState
     */
    for(auto &transition : transitions){
        if (transition.second == endState){
            transition.first = symbol;
        }
    }
}

bool State::hasOnlyTransitionToItself() {
    /**
     True/False ofdat de State enkel transities heeft naar zichzelf
     */
    bool result = true;
    for(pair<string, State*> transition : transitions){
        if(transition.second != this){
            result = false;
        }
    }
    return result;
}

void State::deleteTransitionsToState(State *endState) {
    /**
     Verwijdert al de transities naar één bepaalde eindstaat
     */
    for (int i = 0; i < getTransitions().size(); i++) {
        if(getTransitions()[i].second == endState){
            auto it = find(getTransitions().begin(), getTransitions().end(), getTransitions()[i]);
            transitions.erase(it);
            i -= 1;
        }
    }

}

void State::deleteALLTransitions() {
    /**
     Alle transities worden verwijderd
     */
     transitions = {};
}

pair<bool, string> State::getExpressionToState(State *endState) {
    /**
     Geeft de expressie (in string) terug van de transitie naar eindstate
     Dit is maar 1 string, want moesten er meerdere transities zijn dan zijn die in het begin van toRE samen genomen ("a+b")
     */
     for(auto const &transition : getTransitions()){
         if (transition.second == endState){
             return {true,transition.first};
         }
     }
     return {false, ""};
}



