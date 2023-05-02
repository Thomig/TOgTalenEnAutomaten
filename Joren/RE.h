//
// Created by Joren Van der Sande on 29/04/2023.
//

#ifndef PROGRAMMEEROPDRACHT1_RE_H
#define PROGRAMMEEROPDRACHT1_RE_H

#include <string>
#include <vector>
#include <stack>
#include <algorithm>

#include "ENFA.h"

using namespace std;

class RE {
private:
    string reString;
    string eps;
    vector<string> alphabet;
    void readAlphabet();

    ENFA* createNewTransition(char symbol);
    void createPlus(ENFA* enfa1, ENFA* enfa2);
    void createStar(ENFA* enfa);
    void createConcatination(ENFA* enfa1, ENFA* enfa2);

public:
    RE(const string& setReString, char setEps);

    const string &getREString() const;
    void setREString(const string &reStringInput);

    ENFA toENFA();
};


#endif //PROGRAMMEEROPDRACHT1_RE_H
