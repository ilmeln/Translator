#include <iostream>
#include "syntacticAnalyzer.h"
#include "SA.h"
using namespace std;

int main() //lalallalalalalalalaal
{
    syntacticAnalyzer sa;
    SA s;
    if(!sa.initialization())
        return 1;
    if(sa.analiz("data.txt")) {
        s.getTokenVector(sa.getResult());
        s.analiz();
        s.showTree();
        s.inPython();
        return 0;
    }
    return 2;
}
