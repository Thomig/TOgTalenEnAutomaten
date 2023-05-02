//
// Created by Robbe Branswyk on 12/04/2023.
//

#ifndef TOI_REGEX_H
#define TOI_REGEX_H
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;


class Regex {
private:
    string expression;

    //Hulpfuncties
    bool containsBrackets(const string& expression);
    bool concatenateBrackets(const string& expression);
    vector<string> split(const string& expression, const char toSplit);

public:
    explicit Regex(const string &expression);
    Regex();

    //getters en setters
    const string &getExpression() const;
    void setExpression(const string &expression);

    //methods
    void unions(const string &regex2);
    void concatenate(const string &regex2);
    void powers(int amount);
    void kleeneClosure();
    void print();
};


#endif //TOI_REGEX_H
