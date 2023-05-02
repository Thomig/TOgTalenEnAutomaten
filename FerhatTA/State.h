//
// Created by ferha on 5/03/2023.
//

#include <map>
#include <istream>
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;
#include <iostream>

#ifndef UNTITLED13_STATE_H
#define UNTITLED13_STATE_H


class State {
    string name;
    bool Endstate=false;

    bool Startstate=false;


public:
    map<string,vector<State*>> functie;

    const string &getName() const {
        return name;
    }

    void setName(const string &name) {
        State::name = name;
    }

    void setEndstate(bool endstate) {
        Endstate = endstate;
    }

    void setStartstate(bool startstate) {
        Startstate = startstate;
    }

    State() {};

    void createFunc(string a, State* Q){
        if (functie.find(a) == functie.end()) {
            vector<State*> q;
            q.push_back(Q);
            functie.insert({a, q});
        } else {
            if (std::count(functie[a].begin(), functie[a].end(), Q)) {
                //Do nothing
            }
            else {
                functie[a].push_back(Q);
            }

        }
    };

    bool isStart(){
        return Startstate;
    }

    bool isEnd(){
        return Endstate;
    }

    State* GiveNext(string A){
        if (functie[A].size()==1){
            return functie[A][0];
        }
        else{
            return functie[A][0];
        }
    }

};


#endif //UNTITLED13_STATE_H
