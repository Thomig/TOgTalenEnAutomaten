//
// Created by Robbe Branswyk on 01/03/2023.
//

#include "DFA.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;


/************
 * Functies *
 ************/
DFA::DFA() {}

DFA::DFA(const char *jsonFilename) : Automate() {
    /**
     Constructor: leest de jsonfile uit en voegt de informatie toe
     */
    ifstream file(jsonFilename);
    json json;
    file >> json;
    if (json["type"] == "DFA") {
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
        cerr << "Dit is geen DFA" << endl;
    }
    currentState = beginState;
}

DFA::~DFA() {
    /**
     Destructor: verwijdert elke State die dynamisch is aangemaakt in de constructor
     */
    for (State* state : states) {
        delete state;
    }
}

void DFA::print() {
    /**
        Functie om de DFA uit te printen in de console als json file
     */
    json DFA;
    DFA["type"] = "DFA";

    //Het algemene stuk dat voor elke automaat hetzelfde is
    automateToJson(DFA);

    cout << setw(4) << DFA << endl;
}

bool DFA::accepts(const string& input) {
    /**
        Functie om te kijken of een string zou worden aanvaard
     */

    //input kopieren om aan te passen en de gelezen characters af te kappen
    string inputString = input;

    while (!inputString.empty()){
        string possibleSymbol = "";
        while (!symbolInAlphabet(possibleSymbol)){
            possibleSymbol += inputString[0];
            inputString = inputString.substr(1);
        }
        currentState = getTransitionState(currentState, possibleSymbol);

        possibleSymbol = "";
    }

    //return of de currentState een eindstaat is ja of neen
    bool result = find(endStates.begin(), endStates.end(), currentState) != endStates.end();
    return result;

}

Regex DFA::toRE() const{
    /**
     Een DFA wordt omgezet naar Regex met State Elimination technique
     */
    Regex result;

    //Maak copy van DFA waarin al de aanpassingen (zoals verwijderen) worden gemaakt
    DFA newDFA = copyDFA();

    //Zet al de transitieSymbolen om naar Regex
    //Als vanuit 1 staat meerdere transities (met versch symbolen) bestaan naar dezelfde staat dan Regex: "a+b"
    newDFA.transitionsToRE();


    //Alfabetisch de NIET-eindstaten verwijderen (buiten beginStaat)
    //"verwijderen" := de transities van begin- en eindstaat aanpassen
    vector<State*> nonAcceptStates; //Alle staten die niet beginStaat of AcceptState zijn

    //Zoeken naar al de niet AcceptStates (of Beginstate)
    for(State* state : newDFA.getStates()){
        if (state == newDFA.getBeginState()){
            continue;
        }
        if(find(newDFA.getEndStates().begin(), newDFA.getEndStates().end(), state) != newDFA.getEndStates().end()){
            continue;
        }

        nonAcceptStates.push_back(state);
    }

    nonAcceptStates = newDFA.sortStates(nonAcceptStates); //alfabetisch sorteren
    newDFA.deleteAllStatesForRE(nonAcceptStates);


    //Hier verder werken met de eindstaten
    vector<State*> acceptStates = newDFA.getEndStates();     //Alfabetisch sorteren
    sort(acceptStates.begin(), acceptStates.end());

    for(State* acceptState : newDFA.getEndStates()){
        string expression = newDFA.toReAcceptState(acceptState);
        result.unions(expression);
    }

    return result;
}

/****************
 * Hulpfuncties *
 ****************/

State *DFA::getTransitionState(State *beginState, string symbol) {
    /**
        Geeft de transitie terug als een symbool wordt uitgelezen
        De vector wordt nu 1 staat, want bij DFA kan er maar 1 transitie per symbool per staat zijn
     */
    if (beginState->getTransitionState(symbol).empty()){
        return nullptr;
    }
    return beginState->getTransitionState(symbol)[0];
}

void DFA::addTransition(State *beginState, string symbol, State *endState) {
    /**
        Voegt een transitie toe aan een staat
        Omdat het een DFA is zal de eindstaat van de transitie worden overschreven indien de transitie al bestaat
     */
    if(getTransitionState(beginState, symbol) == nullptr){
        beginState->addTransition(symbol, endState);
    } else {
        beginState->changeTransition(symbol, endState);
    }
}

State *DFA::getCurrentState() const {
    return currentState;
}

void DFA::setCurrentState(State *currentState) {
    DFA::currentState = currentState;
}

DFA DFA::copyDFA() const{
    /**
     Maakt een copy van de DFA aan
     Wordt gebruikt in: toRE
     */
     DFA copyDFA = DFA();
     copyDFA.setAlphabet(getAlphabet());

     //vector met al de transities in string vorm, daarna omzetten met ptrs naar nieuwe States
     vector<vector<string>> transitionsCopyDFA;

     for(State* state : getStates()){
         State* copyState = new State();
         copyState->setName(state->getName());
         copyDFA.addState(copyState);

         for(pair<string, State*> transition : state->getTransitions()){
             vector<string> newTransition = {state->getName(), transition.first, transition.second->getName()};
             transitionsCopyDFA.push_back(newTransition);
         }

         if (find(getEndStates().begin(), getEndStates().end(), state) != getEndStates().end()){
             copyDFA.addEndstate(copyState);
         }
         if (state == getCurrentState()){
             copyDFA.setCurrentState(copyState);
             copyDFA.setBeginState(copyState);
         }
     }

     for(vector<string> transition : transitionsCopyDFA){
         State* beginState;
         string symbol = transition[1];
         State* endState;

         for(State* state : copyDFA.getStates()){
            if(state->getName() == transition[0]){
                beginState = state;
            }
            if(state->getName() == transition[2]){
                endState = state;
            }
         }

         beginState->addTransition(symbol, endState);
     }

    return copyDFA;
}

vector<State*> DFA::sortStates(vector<State *> toSort) {
    /**
     Sorteert een vector van States alfabetisch volgens de naam van de State
     */
    vector<string> sortedNames;
    for(State* state : toSort){
        sortedNames.push_back(state->getName());
    }

    sort(sortedNames.begin(), sortedNames.end());

    //De gesorteerde state vector
    vector<State*> sortedStates;
    for(string name : sortedNames){
        for(State* state : toSort){
            if(state->getName() == name){
                sortedStates.push_back(state);
            }
        }
    }

    return sortedStates;
}

vector<State*> DFA::getAllStatesWithTransTo(State *endState) {
    /**
     Geeft vector van States terug met al de States die een transitie hebben naar de variabele endState
     Wordt gebruikt in: toRE
     */
    vector<State*> states;
    for(State* state : getStates()){
        if(state == endState){
            continue;
        }
        bool hasTransition = false;
        vector<State*> transitionsTo;
        for(auto transition : state->getTransitions()){
             transitionsTo.push_back(transition.second);
        }

        if(find(transitionsTo.begin(), transitionsTo.end(), endState) != transitionsTo.end()){
            hasTransition = true;
        }

        if(hasTransition){
            states.push_back(state);
        }
    }

    return states;
}

void DFA::transitionsToRE() {
    /**
    Zet al de transitieSymbolen om naar Regex
    Als vanuit 1 staat meerdere transities (met versch symbolen) bestaan naar dezelfde staat dan Regex: "a+b"
    */

    for(State* state : getStates()){
        vector<pair<Regex, State*>> regexTransitions; //Al de transities van state met Regex

        for(auto transition : state->getTransitions()){
            bool newEndstateInTransitions = true;
            for(pair<Regex, State*> &transitionRE : regexTransitions){
                //Als de eindstaat al te vinden is in de vector, dan moet er een unie van genomen worden: "a+b"
                if(transitionRE.second == transition.second){
                    newEndstateInTransitions = false;
                    transitionRE.first.unions(transition.first);
                }
            }
            //Anders, dan moet het paar toegevoegd worden aan vector
            if(newEndstateInTransitions){
                regexTransitions.push_back({Regex(transition.first), transition.second});
            }
        }

        //Eerst al de transities verwijderen, dan nieuwe toevoegen volgens de vector regexTransitions
        //Dit is om duplicates toe te voegen (addTransition() zorgt voor geen duplicates)
        state->deleteALLTransitions();
        for(pair<Regex, State*> &transitionRE : regexTransitions){
            state->addTransition(transitionRE.first.getExpression(),transitionRE.second);
        }
    }
}

string DFA::toReAcceptState(State *acceptState) const {
    /**
     Maakt de regex voor de gevraagde eindStaat
     Verwijdert al de andere eindstaten (behalve beginStaat) en vormt zo de R,S,U en T
     Als het de beginStaat zelf is: alle staten verwijderen en R* zoeken
     */
    Regex result;

    //Copy maken van de DFA om staten te kunnen verwijderen zonder oorspronkelijke te beschadigen
    DFA newDFA = copyDFA();

    //Omdat er nu een nieuwe DFA is, moeten we de juiste ptr zoeken van de gevraagde acceptState
    State* acceptStateRE;
    for(State* endState : newDFA.getEndStates()){
        if(endState->getName() == acceptState->getName()){
            acceptStateRE = endState;
        }
    }

    //Nu vector opstellen met al de staten dat we gaan verwijderen (niet acceptStateRE en beginState)
    vector<State*> statesToDel;
    for(State* endState : newDFA.getEndStates()){
        if(endState != acceptStateRE){
            statesToDel.push_back(endState);
        }
    }

    //Nu de states uit statesToDel verwijderen en de transities aanpassen
    newDFA.deleteAllStatesForRE(statesToDel);

    //R,S,U en T ophalen OF R* als het beginStaat is
    if(acceptStateRE == newDFA.getBeginState()){
        Regex R;
        //De transitie naar zichzelf
        string toItselfRE = acceptStateRE->getExpressionToState(acceptStateRE).second;

        R.setExpression(toItselfRE);
        //Kleene Closure nemen om het sterretje toe te voegen
        R.kleeneClosure();
        result.setExpression(R.getExpression());
    } else {
        Regex R; // Regex van BeginStaat naar BeginStaat
        Regex S; // Regex van BeginStaat naar AcceptStaat
        Regex U; // Regex van AcceptStaat naar AcceptStaat
        Regex T; // Regex van AcceptStaat naar BeginStaat

        string reR = newDFA.getBeginState()->getExpressionToState(newDFA.getBeginState()).second;
        R.setExpression(reR);

        string reS = newDFA.getBeginState()->getExpressionToState(acceptStateRE).second;
        S.setExpression(reS);

        string reU = acceptStateRE->getExpressionToState(acceptStateRE).second;
        U.setExpression(reU);

        string reT = acceptStateRE->getExpressionToState(newDFA.getBeginState()).second;
        T.setExpression(reT);

        //Result van de vorm (R+SU*T)*SU*
        //Als T == "", dan is er geen mogelijkheid om terug te keren naar beginstaat dus valt SU*T weg
        result.setExpression(R.getExpression());       //R
        U.kleeneClosure();                             //U*
        S.concatenate(U.getExpression());       //SU*

        if (T.getExpression() != ""){
            S.concatenate(T.getExpression());       //SU*T
            result.unions(S.getExpression());       //R+SU*T
        }

        result.kleeneClosure();                        //(R+SU*T)*      OF R*
        result.concatenate(reS);                //(R+SU*T)*S     OF R*S
        result.concatenate(U.getExpression());  //(R+SU*T)*SU*   OF R*SU*
    }

    return result.getExpression();
}

void DFA::deleteAllStatesForRE(vector<State *> statesToDelete) {
    /**
     Overloopt al de staten uit de vector en verbindt de staten die er naar wijzen met de staten waaruit ze wijzen
     Zo om de RegEx van de automaat te vormen
     */

    /*while(statesToDelete.size() != 0){
        State* stateToDel = statesToDelete[0];
        //STD = StateToDelete
        vector<State*> statesToSTD = getAllStatesWithTransTo(stateToDel); //Alle states met transitie naar staat, zonder de staat zelf

        //Als stateToDel enkel states naar zichzelf heeft (= deathstate) kunnen we hem zo verwijderen + transities naar hem
        if(stateToDel->hasOnlyTransitionToItself()){
            auto it = find(statesToDelete.begin(), statesToDelete.end(), stateToDel);
            statesToDelete.erase(it);

            deleteState(stateToDel);
            continue;
        }

        //Het is geen deathstate
        for(State* state : statesToSTD){
            Regex expressionToSTD = Regex(state->getExpressionToState(stateToDel).second);

            //Kijken of er een transitie is naar zichzelf, die moeten worden geconcateneerd met een kleene Closure
            //bv: a wordt a* en dan concateneren
            Regex expressionToItself;
            if(stateToDel->getExpressionToState(stateToDel).first){
                expressionToItself = Regex(stateToDel->getExpressionToState(stateToDel).second);
                expressionToItself.kleeneClosure();
                expressionToSTD.concatenate(expressionToItself.getExpression());
            }

            //De andere transities
            for(pair<string, State*> transition : stateToDel->getTransitions()){
                //De transitie naar zichzelf is al afgerond hier boven
                if(transition.second == stateToDel){
                    continue;
                }

                Regex expression = Regex(transition.first);

                //Copy maken van expressionToSTD
                Regex copyExpressionToSTD = Regex(expressionToSTD.getExpression());
                copyExpressionToSTD.concatenate(expression.getExpression());

                //Nieuwe transitie toevoegen aan State dat de verbinding is met State en de transitieState vanuit NAS
                state->addTransition(copyExpressionToSTD.getExpression(), transition.second);

            }

            //De transities naar de NAS verwijderen (want ze zijn vervangen)
            state->deleteTransitionsToState(stateToDel);
        }

        //Al de nieuwe verbindingen zijn gelegd tussen de staten verbonden met NAS en de transitieStaten van NAS
        //Nu NAS verwijderen uit de DFA

        auto it = find(statesToDelete.begin(), statesToDelete.end(), stateToDel);
        statesToDelete.erase(it);

        deleteState(stateToDel);

        //Nog eens al de transities omzetten naar Regex
        //Als vanuit 1 staat meerdere transities (met versch symbolen) bestaan naar dezelfde staat dan Regex: "a+b"
        transitionsToRE();
    }*/

    while(statesToDelete.size() != 0){
        State* stateToDel = statesToDelete[0];
        //STD = StateToDelete
        vector<State*> statesToSTD = getAllStatesWithTransTo(stateToDel); //Alle states met transitie naar staat, zonder de staat zelf

        //Als stateToDel enkel states naar zichzelf heeft (= deathstate) kunnen we hem zo verwijderen + transities naar hem
        if(stateToDel->hasOnlyTransitionToItself()){
            auto it = find(statesToDelete.begin(), statesToDelete.end(), stateToDel);
            statesToDelete.erase(it);

            deleteState(stateToDel);
            continue;
        }

        //Het is geen deathstate
        for(State* state : statesToSTD){
            Regex expressionToSTD = Regex(state->getExpressionToState(stateToDel).second);

            //Kijken of er een transitie is naar zichzelf, die moeten worden geconcateneerd met een kleene Closure
            //bv: a wordt a* en dan concateneren
            Regex expressionToItself;
            if(stateToDel->getExpressionToState(stateToDel).first){
                expressionToItself = Regex(stateToDel->getExpressionToState(stateToDel).second);
                expressionToItself.kleeneClosure();
                expressionToSTD.concatenate(expressionToItself.getExpression());
            }

            //De andere transities
            for(pair<string, State*> transition : stateToDel->getTransitions()){
                //De transitie naar zichzelf is al afgerond hier boven
                if(transition.second == stateToDel){
                    continue;
                }

                Regex expression = Regex(transition.first);

                //Copy maken van expressionToSTD
                Regex copyExpressionToSTD = Regex(expressionToSTD.getExpression());
                copyExpressionToSTD.concatenate(expression.getExpression());

                //Nieuwe transitie toevoegen aan State dat de verbinding is met State en de transitieState vanuit NAS
                state->addTransition(copyExpressionToSTD.getExpression(), transition.second);

            }

            //De transities naar de NAS verwijderen (want ze zijn vervangen)
            state->deleteTransitionsToState(stateToDel);
        }

        //Al de nieuwe verbindingen zijn gelegd tussen de staten verbonden met NAS en de transitieStaten van NAS
        //Nu NAS verwijderen uit de DFA

        auto it = find(statesToDelete.begin(), statesToDelete.end(), stateToDel);
        statesToDelete.erase(it);

        deleteState(stateToDel);

        //Nog eens al de transities omzetten naar Regex
        //Als vanuit 1 staat meerdere transities (met versch symbolen) bestaan naar dezelfde staat dan Regex: "a+b"
        transitionsToRE();
    }
}





