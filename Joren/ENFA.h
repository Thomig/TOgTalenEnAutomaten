//
// Created by Joren Van der Sande on 29/04/2023.
//

#ifndef PROGRAMMEEROPDRACHT1_ENFA_H
#define PROGRAMMEEROPDRACHT1_ENFA_H

#include "Automaat.h"
#include <algorithm>

class ENFA : public Automaat{
private:
    string eps;
    vector<State*> getAllStatesViaEpsilon(State* state, map<State*, bool> &checkMap);
public:
    ENFA(string symbool, vector<string> alphabet, string eps, string type); // Constructor die twee states aanmaakt met het symbool als transitie

    void giveNames(); // Geeft namen aan elke staat zodat debuggen wat makkelijker is
    void setEps(string setEps);

    void printStats();

    void combineENFA(ENFA* other, State* toOther = nullptr); // Voegt een andere ENFA toe (kopieert alleen de states -> alfabet moet hetzelfde zijn) Laat de pointers ook terug wijzen naar de kopie

    bool accepts(const string& input);
};


#endif //PROGRAMMEEROPDRACHT1_ENFA_H
