//
// Created by Robbe Branswyk on 12/04/2023.
//

#include "Regex.h"

/************
 * Functies *
 ************/
void Regex::unions(const string &regex2) {
    /**
     Update de regex naar de unie met regex2
     Voegt een '+' toe en zet ze in alfabetische volgorde
     */
     vector<string> regExpressions = {expression, regex2};
     string result;

    //Nooit haakjes plaatsen bij +, is niet nodig
    //Wel nodig om het te splitsen bij alle +'en zo alfabetisch sorteren
    string copyExpression = getExpression() + "+" + regex2;
    vector<string> splitString = split(copyExpression, '+');
    sort(splitString.begin(), splitString.end());

    for (int i = 0; i < splitString.size(); i++) {
        if(splitString[i] != "") {
            result += splitString[i];
            if (i != splitString.size() - 1) {
                result += "+";
            }
        }
    }

     expression = result;
}

void Regex::concatenate(const string &regex2) {
    /**
     Update de regex naar de concatenatie met regex2
     Concateneert ze en zet ze NIET in alfabetische volgorde
     */

    //Kijken niet na of size == 0 want het is een DFA dus bevat geen lege strings
    //Als size == 1, dan moeten er geen haakjes rond (bv: ab), anders (ab)(b+c)

    if (concatenateBrackets(getExpression())) {
        expression = "(" + getExpression() + ")";
    }
    string result;
    if (concatenateBrackets(regex2)){
        result = "(" + regex2 + ")";
    } else {
        result = regex2;
    }

    expression += result;
}

void Regex::powers(int amount) {
    /**
     Update de regex naar de power of zichzelf
     Power of 0 wordt emptyString; Power of 1 wordt zichzelf;
     */
     if (amount == 0){
         setExpression("");
     } else {
         string toConcatenate = getExpression();
         for (int i = 0; i < amount-1; i++) {
             concatenate(toConcatenate);
         }
     }
}


void Regex::kleeneClosure() {
    /**
     Voegt een '*' toe aan de regex
     */
    string result;
    if(getExpression().size() == 1){
        result = getExpression() + "*";
    } else if(getExpression() != ""){
        result = "(" + getExpression() + ")*";
    }

    setExpression(result);
}


/********************************
 * Constructors/Getters/Setters *
 ********************************/
Regex::Regex(const string &expression) : expression(expression) {}

Regex::Regex() {}

const string &Regex::getExpression() const {
    return expression;
}

void Regex::setExpression(const string &expression) {
    Regex::expression = expression;
}



/****************
 * Hulpfuncties *
 ****************/
bool Regex::containsBrackets(const string &expression) {
    /**
     Overloopt de string en kijkt of er haakjes aanwezig zijn
     Wordt gebruikt in: "unions" (+)
     */
    for(auto c : expression){
        if (c == '(' or c == ')'){
            return true;
        }
    }

    return false;
}

bool Regex::concatenateBrackets(const string &expression) {
    /**
     Check of er bij de concatenatie haakjes rond de string moeten
     Dit moet als er ergens een + in zit dat nog niet ingepakt is (bv: "(a+b)a" geen haakjes nodig, maar "(a+b)+(a+c)" wel)
     */
     //vector aanmaken met al de bewerkingtekens (,),+ in volgorde
     vector<char> bewerkingen;
     for(char c : expression){
         if (c == '+' or c == '(' or c == ')'){
             bewerkingen.push_back(c);
         }
     }

    //getal dat bijhoudt hvl haakjes er zijn in de string,
    // bij pos. int, dan meer '(' dan ')' geweest en moeten we nog sluiten
     int amountOfBrackets = 0;
     for(char c : bewerkingen){
         if(c == '('){
             amountOfBrackets += 1;
             continue;
         } else if (c ==')'){
             amountOfBrackets -= 1;
             continue;
         }

         //bij amountOfBrackets == 0: alle haakjes zijn gesloten dus dit is de bovenste laag: degene te sorteren
         if(c == '+' && amountOfBrackets == 0){
             return true;
         }
     }
    return false;

}

vector<string> Regex::split(const string &expression, const char toSplit) {
    /**
     Split de string op de plaatsen van de aangegeven character en geeft de stukken terug in vector
     Bv: "a+b+c" met split bij '+' => {"a", "b", "c"}
     */
     string copyString = expression;
     vector<string> split;

     string temp = "";
     int amountOfBrackets = 0;  //getal dat bijhoudt hvl haakjes er zijn in de string,
                                // bij pos. int, dan meer '(' dan ')' geweest en moeten we nog sluiten
     for(char c : copyString){
         if(c == '('){
             amountOfBrackets += 1;
             temp += c;
         } else if (c ==')'){
             amountOfBrackets -= 1;
             temp += c;
         } else if (c == toSplit && amountOfBrackets == 0){
             //bij amountOfBrackets == 0: alle haakjes zijn gesloten dus dit is de bovenste laag: degene te sorteren
             split.push_back(temp);
             temp = "";
         } else {
             temp += c;
         }
     }
     split.push_back(temp);
     return split;
}

void Regex::print() {
    cout << getExpression() << endl;
}
