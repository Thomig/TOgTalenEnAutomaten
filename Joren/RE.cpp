//
// Created by Joren Van der Sande on 29/04/2023.
//

#include "RE.h"

void RE::readAlphabet() {
    for (auto c : reString) {
        if (c == '(' or c == ')' or c == '*' or c == '+' or c == eps[0]) continue;
        string temp(1, c);
        auto it = find(alphabet.begin(), alphabet.end(), temp);
        if (it == alphabet.end()) { // Alleen toevoegen als het er nog niet inzit
            alphabet.push_back(temp);
        }
    }
}

RE::RE(const string& setReString, char setEps) {
    RE::reString = setReString;
    string temp(1, setEps);
    RE::eps = temp;
    readAlphabet();
}

const string &RE::getREString() const {
    return reString;
}

void RE::setREString(const string &reStringInput) {
    RE::reString = reStringInput;
}

ENFA* RE::createNewTransition(char symbol) {
    string temp(1, symbol);
    ENFA* newEnfa = new ENFA(temp, alphabet, eps, "enfa");
    return newEnfa;
}

// Zal enfa1 vervangen door (enfa1 + enfa2)
void RE::createPlus(ENFA* enfa1, ENFA* enfa2) {
    // Front state aanmaken
    State* enfa1Start = enfa1->getStartState();
    enfa1Start->starting = false;
    State* enfa2Start = enfa2->getStartState();
    enfa2Start->starting = false;
    map<string, State*> frontStateNext = {{eps + "1", enfa1Start}, {eps + "2", enfa2Start}};
    State* frontState = new State("", frontStateNext, true, false);

    // Back state aanmaken
    State* enfa1End = enfa1->getEndStates()[0]; // Er mag maar één eindstaat zijn
    enfa1End->accepting = false;
    State* enfa2End = enfa2->getEndStates()[0];
    enfa2End->accepting = false;
    State* backState = new State("", {}, false, true);
    // twee enfa's laten wijzen naar de backstate
    enfa1End->nextStateList[eps] = backState;
    enfa2End->nextStateList[eps] = backState;

    // alles toevoegen aan enfa1
    enfa1->combineENFA(enfa2, frontState);
    enfa1->addState(frontState);
    enfa1->addState(backState);

    delete enfa2;
}

// vervangt enfa door (enfa)*
void RE::createStar(ENFA* enfa) {
    State* enfaStart = enfa->getStartState();
    enfaStart->starting = false;
    State* enfaBack = enfa->getEndStates()[0]; // Er mag maar één eindstaat zijn
    enfaBack->accepting = false;

    State* backState = new State("", {}, false, true);
    State* startState = new State("", {{eps + "1", enfaStart}, {eps + "2", backState}}, true, false);
    enfaBack->nextStateList[eps + "1"] = enfaStart;
    enfaBack->nextStateList[eps + "2"] = backState;

    enfa->addState(backState);
    enfa->addState(startState);
}

// Vervangt enfa1 door (enfa1)(enfa2)
void RE::createConcatination(ENFA* enfa1, ENFA* enfa2) {
    // einde van enfa1 zoeken
    State* enfa1End = enfa1->getEndStates()[0]; // Er mag maar één eindstaat zijn
    enfa1End->accepting = false;

    // begin van enfa2 zoeken
    State* enfa2Start = enfa2->getStartState();
    enfa2Start->starting = false;

    enfa1End->nextStateList[eps] = enfa2Start;

    enfa1->combineENFA(enfa2, enfa1End);

    delete enfa2;
}

ENFA RE::toENFA() {
    ENFA* toReturn;
    string reString2 = "(" + reString + ")";
    stack<char> opStack; // houd de operators bij
    stack<ENFA*> stack;
    bool prevWasSymbol = false;
    char prevChar;

    for (auto c : reString2) {
        if (c != '(' and c != ')' and c != '+' and c != '*') {
            // Als het een symbool is
            ENFA* enfa = createNewTransition(c);
            stack.push(enfa);
            if (prevWasSymbol or prevChar == '*' or prevChar == ')') {
                opStack.push('.');
            }
            prevWasSymbol = true;
            continue;
        } else {
            prevWasSymbol = false;
        }

        if (c == '+' and opStack.top() == '.') { // '.' moet voorrang krijgen, dus eerst uitrekenen
            opStack.pop();
            ENFA* enfa2 = stack.top();
            stack.pop();
            ENFA* enfa1 = stack.top();
            stack.pop();
            createConcatination(enfa1, enfa2);
            stack.push(enfa1);
            opStack.push('+');
        } else if (c == '(') {
            if (prevChar == '*' or prevChar == ')') {
                opStack.push('.');
            }
            opStack.push(c);
        } else if (c == '+') {
            opStack.push(c);
        } else if (c == '*') {
            ENFA* enfa1 = stack.top();
            stack.pop();
            createStar(enfa1);
            stack.push(enfa1);
        } else if (c == ')') {
            if (opStack.top() == '(') { // Er is geen operatie om uit te voeren
                opStack.pop();
            } else {
                while (opStack.top() != '(') {
                    if (opStack.top() == '.') {
                        opStack.pop();
                        ENFA* enfa2 = stack.top();
                        stack.pop();
                        ENFA* enfa1 = stack.top();
                        stack.pop();
                        createConcatination(enfa1, enfa2);
                        stack.push(enfa1);
                    } else if (opStack.top() == '+') {
                        opStack.pop();
                        ENFA* enfa2 = stack.top();
                        stack.pop();
                        ENFA* enfa1 = stack.top();
                        stack.pop();
                        createPlus(enfa1, enfa2);
                        stack.push(enfa1);
                    }
                }
                opStack.pop();
            }
        }
        prevChar = c;
    }


    toReturn = stack.top();
    toReturn->setEps(eps);
    toReturn->giveNames();
    stack.pop();
    if (!stack.empty()) { // DEBUG
        cerr << "De stack was op het einde nog niet leeg" << endl;
    }
    if (!opStack.empty()) { // DEBUG
        cerr << "De opStack was op het einde nog niet leeg" << endl;
    }
    return *toReturn;
}