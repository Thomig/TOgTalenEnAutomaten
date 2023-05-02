//
// Created by Robbe Branswyk on 03/04/2023.
//

#include "NFA.h"
#include <algorithm>
#include <fstream>
#include <iomanip>

/************
 * Functies *
 ************/

NFA::NFA(const char *jsonFilename) {
    ifstream file(jsonFilename);
    json json;
    file >> json;
    if (json["type"] == "NFA") {
        //cout << "Het is een DFA, alles oké!" << endl;

        for (auto i : json["alphabet"]){
            alphabet.push_back(i);
        }
        for (auto i: json["states"]) {
            //State aanmaken
            State *newState = new State;
            newState->setName(i["name"]);

            //Toevoegen aan States
            states.push_back(newState);

            if (i["starting"]) {
                beginState = newState;
            }
            if (i["accepting"]) {
                endStates.push_back(newState);
            }
        }
        for (auto i : json["transitions"]){
            string from = i["from"];
            for (auto s : states){
                if (s->getName() == from){
                    string to = i["to"];
                    string symbol = i["input"];

                    State* toState = getState(to);
                    addTransition(s, symbol, toState);
                }
            }

        }
    }
    else{
        cerr << "Dit is geen NFA" << endl;
    }
    currentState = {beginState};
}

NFA::~NFA() {
    /**
 Destructor: verwijdert elke State die dynamisch is aangemaakt in de constructor
 */
    for (State* state : states) {
        delete state;
    }
}

void NFA::print() {
    json NFA;
    NFA["type"] = "NFA";

    automateToJson(NFA);
    Automate::print();

    cout << setw(4) << NFA << endl;
}

bool NFA::accepts(const string& input) {
    /**
     Kijken of de gegeven string wordt aanvaard
     */

    //Input kopieren om aan te kunnen passen en de gelezen characters af te kappen
    string inputString = input;

    while (!inputString.empty()){
        string possibleSymbol = "";
        while (!symbolInAlphabet(possibleSymbol)){
            if(inputString.empty()){
                //String bevat symbolen niet in het alfabet waardoor uiteindelijke alle symbolen uit de string zijn toegevoegd en de inputstring leeg is
                cerr << "Input bevat symbolen die niet in het alfabet voorkomen" << endl;
                return false;
            }
            possibleSymbol += inputString[0];
            inputString = inputString.substr(1);
        }

        //Temporary vector aanmaken waar al de mogelijke transities worden aan toegevoegd
        vector<State*> tempCurrStates;
        for (State* state : currentState){
            vector<State*> transitionStates;
            transitionStates = getTransitionState(state, possibleSymbol);
            for(State* i : transitionStates){
                if (find(tempCurrStates.begin(), tempCurrStates.end(), i) == tempCurrStates.end()){
                    tempCurrStates.push_back(i);
                }
            }
        }
        currentState = tempCurrStates;
        tempCurrStates = {};
        possibleSymbol = "";
    }

    //Kijken of de bekomen currenstates een eindstate bevat (dit is een mogelijk positief einde)
    bool result = false;
    for (State* state : currentState){
        if (find(endStates.begin(), endStates.end(), state) != endStates.end()){
            result = true;
        }
    }
    return result;

}

DFA NFA::toDFA() {
    DFA dfa = DFA();
    dfa.setAlphabet(alphabet);

    //Nieuwe beginState maken voor de DFA
    State* beginStateDFA = new State();
    beginStateDFA->setName(makeNewStateName({beginState}));
    dfa.addState(beginStateDFA);
    dfa.setBeginState(beginStateDFA);

    //Kijken of het een eindstate is
    if(containsEndState({beginState})){
        dfa.addEndstate(beginStateDFA);
    }

    vector<vector<State*>> dfaStates = {{beginState}};
    vector<string> transitionsChecked = {};
    vector<string> statesAlreadyMade = {makeNewStateName({beginState})};

    bool done = true;
    while (done){
        done = false;
        for (vector<State*> mainState : dfaStates){
            string stateName = makeNewStateName(mainState);
            if (find(transitionsChecked.begin(), transitionsChecked.end(), stateName) == transitionsChecked.end()){
                transitionsChecked.push_back(stateName);
                //Transities aanmaken
                for(string symbol : alphabet) {
                    vector<State*> allTheTransitions = allTransitions(mainState, symbol);

                    if(find(statesAlreadyMade.begin(), statesAlreadyMade.end(), makeNewStateName(allTheTransitions)) == statesAlreadyMade.end()){
                        done = true; //Er worden nieuwe States aangemaakt dus het eindigt nog niet

                        //Aanmaken van de nieuwe State
                        string allTheTransitionsName = makeNewStateName(allTheTransitions);

                        State* allTheTransitionsState = new State();
                        allTheTransitionsState->setName(allTheTransitionsName);
                        dfa.addState(allTheTransitionsState); //toevoegen aan de states van de dfa

                        //Toevoegen aan de lijst en als transitie voor de mainState
                        dfaStates.push_back(allTheTransitions);
                        statesAlreadyMade.push_back(allTheTransitionsName);
                        for(State* state : dfa.getStates()){
                            if(state->getName() == stateName){
                                dfa.addTransition(state, symbol, allTheTransitionsState);
                            }
                        }

                        //Kijken of het een eindstate is
                        if(containsEndState(allTheTransitions)){
                            dfa.addEndstate(allTheTransitionsState);
                        }
                    } else {
                        //De staat bestaat al, maar moet nog wel toegevoegd worden aan de transities van mainState
                        string allTheTransitionsName = makeNewStateName(allTheTransitions);
                        State* theMainState;
                        for(State* state : dfa.getStates()){
                            //State zoeken
                            if(state->getName() == stateName){
                                theMainState = state;
                            }
                        }
                        for(State* state : dfa.getStates()){
                            //Transitie zoeken
                            if (state->getName() == allTheTransitionsName){
                                dfa.addTransition(theMainState, symbol, state);
                            }
                        }
                    }
                }
            }
        }
    }

    dfa.setCurrentState(dfa.getBeginState());
    return dfa;
}

void NFA::transitionsForTheNewDFAState(State *&newState, vector<State *> transitions) {
    /**
     Bij het omzetten van NFA naar DFA zal deze functie de transities van de nieuwe staat (bestaande uit transitions) toevoegen
     */
     for(string symbol : alphabet){
         for(State* transition : transitions){
             vector<State*> transitionTransitions = getTransitionState(transition, symbol);
             for(State* i : transitionTransitions){
                 if(find(getTransitionState(newState, symbol).begin(), getTransitionState(newState, symbol).end(), i) == getTransitionState(newState, symbol).end()){

                 }
             }
         }
     }

}

State *NFA::toDFAState(const string &stateName) {
    return nullptr;
}
