//
// Created by ferha on 5/03/2023.
//

#include "DFA.h"
#include <algorithm>



bool DFA::accepts(std::string N) {
    State* einde=start;
    for (auto i:N){
        string h;
        h+=i;
        einde=start->GiveNext(h);
    }
    if (einde->isEnd()==true){
        return true;
    }
    else{
        return false;
    }
}

State* DFA::findState(string Namestate){
    for (auto i : statesSet){
        if (i->getName()==Namestate){
            return i;
        }
    }
}


DFA::DFA(string filename) {

    ifstream input(filename);

    json j;
    input >> j;

    auto alp=j["alphabet"];

    for (string Q:alp){
        alpha.push_back(Q);
    }

    auto states=j["states"];

    for (auto i: states){
        State* N= new State();

        if (i["accepting"]==true){
            N->setEndstate(true);
        }
        if (i["starting"]==true){
            N->setStartstate(true);
            start=N;
        }
        N->setName(i["name"]);

        statesSet.push_back(N);
    }

    auto transitions=j["transitions"];
    for (int i=0;i<transitions.size();i++){
        string f=transitions[i]["from"];
        State* F=findState(f);
        string t=transitions[i]["to"];
        State* T=findState(t);
        string n=transitions[i]["input"];
        F->createFunc(n,T);
    }
}

string DFA::geefNaam(string naam){
    string n;
    for (char i:naam){
        if (i=='{'){
            continue;
        }
        else if(i=='}'){
            continue;
        }
        else{
            n+=i;
        }
    }
    return n;
}

string DFA::statesToName(vector<State *> states) {
    if (states.size()==1){
        return states[0]->getName();
    }
    else{
        string naam= "(";
        for (State* i:states){
            naam+= geefNaam(i->getName());
            naam+=",";
        }
        naam.pop_back();
        naam+=")";
        return naam;
    }

}

void DFA::print() {
    json j;
    j["type"] = "DFA";
    j["alphabet"] = alpha;


    for (State* s:statesSet){
        json j1;
        j1["name"]=s->getName();
        j1["starting"]=s->isStart();
        j1["accepting"]=s->isEnd();
        j["states"].push_back(j1);
    }

    for (State* s:statesSet){
        json j1;

        for (auto i:s->functie){
            j1["from"]=s->getName();
            j1["to"]= statesToName(i.second);
            j1["input"]=i.first;
            j["transitions"].push_back(j1);
        }
    }
    cout << setw(4) << j << endl;
}

DFA::DFA() {}

void DFA::setStatesSet(const vector<State *> &statesSet) {
    DFA::statesSet = statesSet;
}

void DFA::setAlpha(const vector<string> &alpha) {
    DFA::alpha = alpha;
}

void DFA::setStart(State *start) {
    DFA::start = start;
}

State* DFA::returnSmal(vector<State *> &vec) {
    State* smal=vec[0];
    int counter=0;
    for (auto i:vec){
        if (smal->getName()>i->getName()){
            smal=i;
            counter+=1;
        }
    }
    vec.erase(vec.begin()+counter);
    return smal;
}

vector<State *> DFA::sortState(vector<State *> Alpa) {
    vector<State*> Sort;
    while (Alpa.size()!=0){
        Sort.push_back(returnSmal(Alpa));
    }
    return Sort;
}

string DFA::mergName(vector<State *> a) {
    string naam="(";
    for (auto i:a){
        naam+= i->getName()+",";
    }
    naam.pop_back();
    naam+=")";
    return naam;
}

State* DFA::mergFunc(vector<State *> States, bool A) {
    State* StateSet=new State();

    int endstates=0;
    string naam= "(";
    for (State* i:States){
        naam+= i->getName();
        naam+=",";
        if (i->isEnd()==true){
            endstates+=1;
        }
    }
    if (endstates==2 and A == true){
        StateSet->setEndstate(true);
    }
    else if (1<=endstates and A==false){
        StateSet->setEndstate(true);
    }

    naam.pop_back();
    naam+=")";
    StateSet->setName(naam);
    for (State* j:States){
        map<string, vector<State*>>::iterator it;

        for (it = j->functie.begin(); it != j->functie.end(); it++)
        {
            for (auto m:it->second){
                vector<State*> newTransition;
                StateSet->createFunc(it->first,m);
            }
        }
    }

    statesSet.push_back(StateSet);
    return StateSet;
}

bool DFA::existState(std::string Statename) {
    for (auto i : statesSet){
        if (i->getName()==Statename){
            return true;
        }
    }
    return false;
}

void DFA::mergStates(State *A, bool end) {
    map<string, vector<State*>>::iterator it;
    for (it = A->functie.begin(); it != A->functie.end(); it++){
        if (it->second.size()!=1){
            if (existState(mergName(it->second))==true){

            }
            else{
                State* newState=mergFunc(it->second, end);
                mergStates(newState, end);

            }
        }
    }
}

DFA::DFA(DFA &A, DFA &B, bool w) {
    vector<State*> nieuweStates;
    nieuweStates.push_back(A.start);
    nieuweStates.push_back(B.start);
    setStart(mergFunc(nieuweStates, w));
    start->setStartstate(true);
    mergStates(start, w);

    alpha=A.alpha;
}
