#include <iostream>
#include "DFA.h"
#include "NFA.h"
#include "Regex.h"
#include <fstream>
#include <iomanip>
#include "json.hpp"
#include <string>
using namespace std;

using json = nlohmann::json;

int main() {
/*    NFA nfa("NFA.json");
    //cout << boolalpha << nfa.accepts("") << endl << nfa.accepts("cc") << endl;
    //nfa.print();
    nfa.toDFA().print();*/


    /*NFA nfa("NFAToReTest1.json");
    DFA dfa = nfa.toDFA();
    Regex re = dfa.toRE();
    re.print();*/

    DFA dfa("DFAToReTest5.json");
    dfa.toRE().print();



/*    //Hopelijk antwoord: "((a*+b)*(a*+b)*)+b"
    regEx2.kleeneClosure();
    regEx2.unions(regEx1.getExpression());
    regEx2.kleeneClosure();
    regEx2.powers(2);
    regEx2.unions(regEx1.getExpression());*/

/*    //Hopelijk antwoord: "a+b+c+d+f"
    regEx2.unions("c");
    regEx2.unions("b");
    regEx2.unions("f");
    regEx2.unions("d");*/



    return 0;
}
