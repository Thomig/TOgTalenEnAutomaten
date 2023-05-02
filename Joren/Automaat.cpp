//
// Created by Joren Van der Sande on 29/04/2023.
//

#include "Automaat.h"

Automaat::Automaat() {}

Automaat::~Automaat() {
    for (State* state : states) {
        delete state;
    }
}

void Automaat::print() const {
    string toPrint;
    // type
    toPrint += "{\n  \"type\": \"" + type + "\",\n";

    // alphabet
    toPrint += "  \"alphabet\": [\n";
    for (int i = 0; i < alphabet.size()-1; ++i) {
        toPrint += "    \"" + alphabet[i] + "\",\n";
    }
    toPrint += "    \"" + alphabet[alphabet.size()-1] + "\"\n";
    toPrint += "  ],\n";

    // states
    toPrint += "  \"states\": [\n";
    for (int i = 0; i < states.size()-1; ++i) {
        toPrint += "    {\n";
        toPrint += R"(      "name": ")" + states[i]->name + "\",\n";
        string startBool = states[i]->starting ? "true" : "false";
        toPrint += "      \"starting\": " + startBool + ",\n";
        string acceptBool = states[i]->accepting ? "true" : "false";
        toPrint += "      \"accepting\": " + acceptBool + "\n";
        toPrint += "    },\n";
    }
    toPrint += "    {\n";
    toPrint += R"(      "name": ")" + states[states.size()-1]->name + "\",\n";
    string startBool = states[states.size()-1]->starting ? "true" : "false";
    toPrint += "      \"starting\": " + startBool + ",\n";
    string acceptBool = states[states.size()-1]->accepting ? "true" : "false";
    toPrint += "      \"accepting\": " + acceptBool + "\n";
    toPrint += "    }\n";
    toPrint += "  ],\n";

    // transitions
    toPrint += "  \"transitions\": [\n";
    bool islast = false;
    int statenr = 0;
    int transnr = 0;
    for (auto currstate : states) {
        for (auto &[key, value] : currstate->nextStateList) {
            if (statenr == states.size()-1 and transnr == currstate->nextStateList.size()-1) {
                islast = true;
            }

            toPrint += "    {\n";
            toPrint += R"(      "from": ")" + currstate->name + "\",\n";
            toPrint += R"(      "to": ")" + value->name + "\",\n";
            toPrint += R"(      "input": ")" + key + "\"\n";

            if (islast) {
                toPrint += "    }\n";
            } else {
                toPrint += "    },\n";
            }
            transnr += 1;
        }
        transnr = 0;
        statenr += 1;
    }
    toPrint += "  ]\n";

    toPrint += "}";
    cout << setw(4) << toPrint << endl;
}

State* Automaat::getState(const string &name) const {
    for (auto state : states) {
        if (state->name == name) {
            return state;
        }
    }
    return nullptr;
}

vector<State*> Automaat::getAllStates() const {
    return states;
}

State* Automaat::getStartState() const {
    for (auto state : states) {
        if (state->starting) {
            return state;
        }
    }
    return nullptr;
}

vector<State*> Automaat::getEndStates() const {
    vector<State*> toReturn;
    for (auto state : states) {
        if (state->accepting) {
            toReturn.push_back(state);
        }
    }
    return toReturn;
}

void Automaat::addState(State* state) {
    // De state moet toegelaten worden door de enfa dus het moet hetzelfde alfabet gebruiken
    Automaat::states.push_back(state);
}

State::State(string name, map<string, State*> nextStateList = {}, bool starting = false, bool accepting = false)
        : name(name), nextStateList(nextStateList), starting(starting), accepting(accepting) {}

