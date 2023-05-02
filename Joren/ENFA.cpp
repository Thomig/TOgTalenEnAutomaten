//
// Created by Joren Van der Sande on 29/04/2023.
//

#include "ENFA.h"

ENFA::ENFA(string symbool, vector<string> alphabet, string eps, string type) {
    ENFA::type = type;
    ENFA::alphabet = alphabet;
    State* end = new State("", {}, false, true);
    State* start = new State("", {{symbool, end}}, true, false);
    addState(end);
    addState(start);
}

void ENFA::giveNames() {
    for (int i = 0; i < states.size(); ++i) {
        states[i]->name = to_string(i);
    }
}

void ENFA::setEps(string setEps) {
    ENFA::eps = setEps;
}

void ENFA::printStats() {
    // no_of_states printen
    cout << "no_of_states=" << states.size() << endl;

    // no_of_transitions voor epsilon printen
    vector<string> epsilons = {eps, eps+"1", eps+"2"};
    int epsCount = 0;
    for (const auto& c : epsilons) {
        for (auto state : states) {
            if (state->nextStateList.count(c) > 0) epsCount++;
        }
    }
    cout << "no_of_transitions[" << eps << "]=" << epsCount << endl;

    // no_of_transitions voor heel het alfabet printenn
    vector<string> sortedAlph = alphabet;
    std::sort(sortedAlph.begin(), sortedAlph.end());
    for (const auto& c : sortedAlph) {
        int count = 0;
        for (auto state : states) {
            if (state->nextStateList.count(c) > 0) count++;
        }
        cout << "no_of_transitions[" << c << "]=" << count << endl;
    }

    // degrees printen
    int total = 0;
    int i = 0;
    while (total < states.size()) {
        int count = 0;
        for (auto state : states) {
            if (state->nextStateList.size() == i) {
                count++;
            }
        }
        cout << "degree[" << to_string(i) << "]=" << to_string(count) << endl;

        total += count;
        i++;
    }
}

void ENFA::combineENFA(ENFA* other, State* toOther) {
    vector<State*> otherStates = other->getAllStates();
    vector<State*> copiedStates;
    for (int i = 0; i < otherStates.size(); ++i) {
        State* copiedState = new State(otherStates[i]->name, otherStates[i]->nextStateList, otherStates[i]->starting, otherStates[i]->accepting);
        copiedStates.push_back(copiedState);
        // Checken of er een staat naar staat van de input other wees (deze staat wordt later verwijderd)
        for (auto thisState : states) {
            for (auto it = thisState->nextStateList.begin(); it != thisState->nextStateList.end(); ++it) {
                if (it->second == otherStates[i]) {
                    thisState->nextStateList[it->first] = copiedState;
                }
            }
        }
    }
    // Nu voor copiedStates alle pointers goed zetten
    for (int i = 0; i < copiedStates.size(); ++i) {
        for (auto it = copiedStates[i]->nextStateList.begin(); it != copiedStates[i]->nextStateList.end(); ++it) {
            for (int j = 0; j < otherStates.size(); ++j) {
                if (it->second == otherStates[j]) {
                    copiedStates[i]->nextStateList[it->first] = copiedStates[j];
                }
            }
        }
    }

    if (toOther != nullptr) {
        // Nu voor een mogelijke staat die naar other wees de pointer(s) goed zetten
        for (auto it = toOther->nextStateList.begin(); it != toOther->nextStateList.end(); ++it) {
            for (int j = 0; j < otherStates.size(); ++j) {
                if (it->second == otherStates[j]) {
                    toOther->nextStateList[it->first] = copiedStates[j];
                }
            }
        }
    }


    states.insert(states.end(), copiedStates.begin(), copiedStates.end());
}

vector<State*> ENFA::getAllStatesViaEpsilon(State* state, map<State*, bool> &checkMap) {
    vector<State*> toReturn;
    map<string, State*> currNext = state->nextStateList;
    map<string, State*>::iterator it;
    for (it = currNext.begin(); it != currNext.end(); ++it) {
        if (it->first == eps or it->first == eps+"1" or it->first == eps+"2") {
            if (!checkMap[it->second]) {
                checkMap[it->second] = true;
                toReturn.push_back(it->second);
                vector<State*> temp = getAllStatesViaEpsilon(it->second, checkMap);
                toReturn.insert(toReturn.end(), temp.begin(), temp.end());
            }
        }
    }
    return toReturn;
}

bool ENFA::accepts(const string& input) {
    vector<State*> currStates = {getStartState()};

    // Over alle char's van de input lopen en kijken naar welke staten je telkens kan gaan
    for (int i = 0; i < input.size(); ++i) {
        // Dingen die ik nodig heb aanmaken
        vector<State*> allPossibleNext = {};
        string inputCharacter(1, input[i]);

        // over alle currStates gaan en kijken naar hun volgende
        for (auto currState : currStates) {
            map<State*, bool> checkMap; // Als true is de state al geweest (voor epsilon transities)
            for (auto state : states) checkMap[state] = false; // check map initialiseren
            map<State*, bool> checkMap2; // Als true is de state al geweest (voor epsilon transities)
            for (auto state : states) checkMap2[state] = false; // check map initialiseren

            vector<State*> allPossibleNextCurr = {};
            map<string, State*> nextStateList = currState->nextStateList;

            // In de current state kijken of er een overgang is met het inputCharacter
            map<string, State*>::iterator it1;
            for (it1 = nextStateList.begin(); it1 != nextStateList.end(); ++it1) {
                if (it1->first == inputCharacter) {
                    allPossibleNextCurr.push_back(it1->second);
                }
            }

            // epsilon overgangen checken
            vector<State*> eclose1;
            map<string, State*>::iterator it2;
            for (it2 = nextStateList.begin(); it2 != nextStateList.end(); ++it2) {
                if (it2->first == eps or it2->first == eps+"1" or it2->first == eps+"2") {
                    eclose1.push_back(it2->second);
                    checkMap[it2->second] = true;
                    vector<State*> temp = getAllStatesViaEpsilon(it2->second, checkMap);
                    eclose1.insert(eclose1.end(), temp.begin(), temp.end());
                }
            }

            // Kijken naar alle eclose-states, of er een overgang is met inputCharacter
            for (auto ecloseState : eclose1) {
                map<string, State*>::iterator it3;
                for (it3 = ecloseState->nextStateList.begin(); it3 != ecloseState->nextStateList.end(); ++it3) {
                    if (it3->first == inputCharacter) {
                        allPossibleNextCurr.push_back(it3->second);
                    }
                }
            }

            // epsilon overgangen checken nog is checken vanaf allPossibleNextCurr
            vector<State*> eclose2;
            map<string, State*>::iterator it4;
            for (auto possNext : allPossibleNextCurr) {
                for (it4 = possNext->nextStateList.begin(); it4 != possNext->nextStateList.end(); ++it4) {
                    if (it4->first == eps or it4->first == eps+"1" or it4->first == eps+"2") {
                        eclose2.push_back(it4->second);
                        checkMap2[it4->second] = true;
                        vector<State*> temp = getAllStatesViaEpsilon(it4->second, checkMap2);
                        eclose2.insert(eclose2.end(), temp.begin(), temp.end());
                    }
                }
            }

            allPossibleNextCurr.insert(allPossibleNextCurr.end(), eclose2.begin(), eclose2.end());
            allPossibleNext.insert(allPossibleNext.end(), allPossibleNextCurr.begin(), allPossibleNextCurr.end());
        }
        currStates = allPossibleNext;
    }

    // Checken of je in een eindstaat terecht bent gekomen
    for (auto state : currStates) {
        if (state->accepting) return true;
    }
    return false;
}
