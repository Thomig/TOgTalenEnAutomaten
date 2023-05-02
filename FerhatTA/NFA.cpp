//
// Created by ferha on 13/04/2023.
//

#include "NFA.h"

bool NFA::accepts(std::string N) {
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

State* NFA::findState(string Namestate){
    for (auto i : statesSet){
        if (i->getName()==Namestate){
            return i;
        }
    }
}

bool NFA::existState(std::string Statename) {
    for (auto i : statesSet){
        if (i->getName()==Statename){
            return true;
        }
    }
    return false;
}



string returnName(string naam){
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

vector<string> NFA::splitState(string naam) {
    vector<string> n;
    for (char i:naam){
        if (i=='{'){
            continue;
        }
        else if(i=='}'){
            continue;
        }
        else if(i==','){
            continue;
        }
        else{
            string q="";
            q+=i;
            n.push_back(q);
        }
    }
    return n;
}

string NFA::mergName(vector<State *> a) {
    string naam="{";
    for (auto i:a){
        naam+= returnName(i->getName())+",";
    }
    naam.pop_back();
    naam+="}";
    return naam;
}

void NFA::stateTransition(State *a) {
    map<string, vector<State*>>::iterator it;
    for (it = a->functie.begin(); it != a->functie.end(); it++){
        it->second= sortState(it->second);
        if (it->second.size()!=1){
            if (existState(mergName(it->second))==true){

            }
            else{
                State* newState=mergeState(it->second);
                stateTransition(newState);

            }
        }
    }
}


State* NFA::returnSmal(vector<State *> &vec) {
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

vector<State *> NFA::sortState(vector<State *> Alpa) {
    vector<State*> Sort;
    while (Alpa.size()!=0){
        Sort.push_back(returnSmal(Alpa));
    }
    return Sort;
}

State* NFA::mergeState(vector<State*> States){
    State* StateSet=new State();
    vector<State*> Sort= sortState(States);

    string naam= "{";
    for (State* i:Sort){
        naam+= returnName(i->getName());
        naam+=",";
        if (i->isEnd()==true){
            StateSet->setEndstate(true);
        }
    }
    naam.pop_back();
    naam+="}";
    StateSet->setName(naam);
    for (State* j:Sort){
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




NFA::NFA(string filename) {

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
        string Nm=i["name"];
        N->setName(Nm);
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


void NFA::print() {
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
            j1["to"]= mergName(i.second);
            j1["input"]=i.first;
            j["transitions"].push_back(j1);
        }
    }
    cout << setw(4) << j << endl;
}

string NFA::statesToName(vector<State *> states) {
    if (states.size()==0){
        return states[0]->getName();
    }
    else{
        string naam= "{";
        for (State* i:states){
            naam+= returnName(i->getName());
            naam+=",";
        }
        naam.pop_back();
        naam+="}";
        return naam;
    }

}

void NFA::makeStateSet(State *start,vector<State*> &setState) {
    map<string, vector<State *>>::iterator it;
    for (it = start->functie.begin(); it != start->functie.end(); it++) {
        if (count(setState.begin(), setState.end(), findState(statesToName(it->second)))) {
            // found
        }
        else {
            setState.push_back(findState(statesToName(it->second)));
            makeStateSet(findState(statesToName(it->second)), setState);
        }
    }
}

DFA NFA::toDFA() {
    for (auto i: statesSet){
        i->setName("{"+i->getName()+"}");
    }
    for (auto j : statesSet){
        stateTransition(j);
    }
    vector<State*> set;
    set.push_back(start);
    makeStateSet(start, set);
    DFA newDFA;
    newDFA.setAlpha(getAlpha());
    newDFA.setStart(getStart());
    newDFA.setStatesSet(set);
    return newDFA;
}

const vector<State *> &NFA::getStatesSet() const {
    return statesSet;
}

const vector<string> &NFA::getAlpha() const {
    return alpha;
}

State *NFA::getStart() const {
    return start;
}

