//
// Created by Joren Van der Sande on 05/03/2023.
//

#include "DFA.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"
#include <utility>

using namespace std;

using json = nlohmann::json;

DFA::DFA() {}

DFA::DFA(string fileName) {

    ifstream input_file(fileName);
    json dfa;
    input_file >> dfa;

    // Type uitlezen
    type = dfa["type"].get<std::string>();

    // Alphabet uitlezen
    for (const string s : dfa["alphabet"]) {
        alphabet.push_back(s);
    }

    // Alle state objecten juist aanmaken
    auto statesElem = dfa["states"];
    for (auto& state : statesElem) {
        string stateName = state["name"];
        bool stateStarting = state["starting"];
        bool stateAccepting = state["accepting"];

        states.push_back(new State(stateName, {}, stateStarting, stateAccepting));
    }

    // De transities aan de state objecten toevoegen
    auto transitionLoop = dfa["transitions"];
    for (auto& transition : transitionLoop) {
        string from = transition["from"];
        string to = transition["to"];
        string input = transition["input"];

        for (auto &i : states) {
            if (i->name == from) {
                for (State* &state : states) {
                    if (state->name == to) {
                        i->nextStateList[input] = state;
                        break;
                    }
                }
            }
        }
    }


    // Table aanmaken
    vector<string> tableLabels;
    for (auto state : states) {
        tableLabels.emplace_back(state->name);
    }
    std::sort(tableLabels.begin(), tableLabels.end());
    table = Table(tableLabels);

}

// Copy constructor
DFA::DFA(const DFA& other) {
    type = other.type;
    alphabet = other.alphabet;

    // Create copies of all the states
    unordered_map<State *, State *> stateMap;
    for (State *state: other.states) {
        State *newState = new State(state->name, state->nextStateList, state->starting, state->accepting);
        states.push_back(newState);
        stateMap[state] = newState;
    }

    // Update the pointers to the copied states
    for (State *newState: states) {
        for (auto &[symbol, nextState]: newState->nextStateList) {
            nextState = stateMap[nextState];
        }
    }
    // Copy the transition table
    table = other.table;
}

bool DFA::accepts(string input) {

    State* currstate;

    for (auto state : states) {
        if (state->starting) {
            currstate = state;
        }
    }

    for (int i = 0; i < input.size(); ++i) {
        for (auto &[key, value] : currstate->nextStateList) {
            if (key == to_string(input[i])) {
                currstate = value;
            }
        }
    }

    if (currstate->accepting)
        return true;
    else return false;
}


void DFA::fillTable() {
    // Alle combinaties van staten met één staat geen eindstaat en de andere wel een eindstaat
    for (int i = 0; i < states.size(); ++i) {
        for (int j = i+1; j < states.size(); ++j) {
            if ((states[i]->accepting and !states[j]->accepting)
                or (!states[i]->accepting and states[j]->accepting)) {
                table.mark(states[i]->name, states[j]->name);
            }
        }
    }

    // Nu itereren over de lege vakken -> kijken of deze combinatie een overgang heeft naar een combinatie dat al gemarkeerd is
    bool marksMade = true;
    while (marksMade) {
        marksMade = false; // als er een markeren wordt gedaan moet dit terug op true worden gezet
        for (int i = 0; i < states.size(); ++i) {
            for (int j = i+1; j < states.size(); ++j) {
                // Continue als gemarkeerd
                if (table.isMarked(states[i]->name, states[j]->name)) continue;
                // Nog niet gemarkeerd -> kijk of er een overgangscombinatie is die naar een gemarkeerde combinatie gaat
                for (const string &input : alphabet) {
                    State* currNS1 = states[i]->nextStateList[input];
                    State* currNS2 = states[j]->nextStateList[input];
                    if (table.isMarked(currNS1->name, currNS2->name)) {
                        table.mark(states[i]->name, states[j]->name);
                        marksMade = true;
                    }
                }
            }
        }
    }
}

// Minimaliseert de DFA
DFA DFA::minimize() {
    table.resetTable();

    fillTable();

    DFA minDFA(*this); // kopie maken van originele dfa

    vector<set<string>> classes = table.getEquivalentClasses();  // Alle equivalentieklassen opvragen

    // Kijken naar één van de equivalentie klassen of de pijlen over gaan naar iets binnen deze klasse of iets er buiten
    for (const auto &set : classes) {
        // Nieuwe state naam aanmaken
        string setName = "{";
        vector<string> currSetvect;
        for (const auto& element : set) {
            currSetvect.emplace_back(element);
        }
        std::sort(currSetvect.begin(), currSetvect.end());
        string aName = currSetvect[0]; // een element in de set, wordt later gebruikt
        for (int i = 0; i < currSetvect.size()-1; ++i) {
            setName += currSetvect[i] + ", ";
        }
        setName += currSetvect[currSetvect.size()-1] + "}";

        State* newState = new State(setName, {}, false, false);
        for (const auto& input : alphabet) {
            // Kijken of voor een willekeurige staat in de transitie functie naar een staat buiten deze set gaat of binnen deze set blijft
            // --> transitie aanmaken
            State* nextState = minDFA.getState(aName)->nextStateList[input];
            if (set.count(nextState->name) > 0) {
                // transitie naar zichzelf
                newState->nextStateList[input] = newState;
            } else {
                // transitie naar een ander
                newState->nextStateList[input] = nextState;
            }
        }

        // Nu alle staten die een transitie functie hebben naar één van de staten in de set naar newState laten wijzen
        for (auto state : minDFA.states) {
            if (set.count(state->name) > 0 ) {
                continue; // alleen de staten bekijken die niet in deze set zitten
            }
            map<string, State*>::iterator it;
            map<string, State*> tempCopy = state->nextStateList;
            for (it = tempCopy.begin(); it != tempCopy.end(); it++) {
                if (set.count(it->second->name) > 0) {
                    // 'state' heeft een transitie naar een state in de current set
                    state->nextStateList[it->first] = newState;
                }
            }
        }

        // kijken of het starting of accepting moet zijn
        if (getState(aName)->accepting) {
            newState->accepting = true;
        }
        // Als één van de staten een starting state was moet heel de nieuwe staat een starting state zijn
        for (auto state : states) {
            if (set.count(state->name) > 0) {
                if (state->starting){
                    newState->starting = true;
                }
            }
        }

        // De oude staten (in de set) die vervangen zijn door één nieuwe staat verwijderen
        for (const auto& item : set) {
            State* currState = minDFA.getState(item);
            auto it = std::find(minDFA.states.begin(), states.end(), currState);
            if (it != minDFA.states.end()) {
                minDFA.states.erase(it);
            }
            delete currState;
        }

        // Vervangen door nieuwe staat
        minDFA.states.emplace_back(newState);

        // {} toevoegen aan alle namen
    }

    for (auto state : minDFA.states) {
        if (state->name[0] != '{' or state->name[state->name.size()-1] != '}') {
            state->name = "{" + state->name + "}";
        }
    }

    return minDFA;
}

void DFA::printTable() {
    cout << table.tableAsString() << endl;
}

bool DFA::operator==(const DFA& other) const {
    // Tabel uitwerken van de twee DFA's samen en kijken of op de startstaat-combinatie een kruisje staat of niet
    // Als er een kruisje staat zijn de DFA's niet equivalent, als er geen kruisje staat zijn ze equivalent

    // Nieuwe DFA aanmaken die een combinatie is van de beide DFA's
    DFA newDFA(*this);

    // States kopieren en toevoegen aan newDFA
    vector<State*> copiedStates;
    for (auto state : other.states) {
        State* copiedState = new State(state->name, state->nextStateList, state->starting, state->accepting);
        copiedStates.push_back(copiedState);
    }
    newDFA.states.insert(newDFA.states.end(), copiedStates.begin(), copiedStates.end());

    // Table aanmaken
    vector<string> labels;
    for (auto state : newDFA.states) {
        labels.emplace_back(state->name);
    }
    newDFA.table = Table(labels);

    // Table filling algoritme toepassen
    newDFA.fillTable();

    // Namen van de startstates zoeken
    string startName1;
    string startName2;
    for (auto state : states) {
        if (state->starting) {
            startName1 = state->name;
        }
    }
    for (auto state : other.states) {
        if (state->starting) {
            startName2 = state->name;
        }
    }

    newDFA.printTable();

    // Kijken of de combinatie van de startstaten gemarkeerd zijn
    if (newDFA.table.isMarked(startName1, startName2)) {
        return false;
    } else {
        return true;
    }
}


