#include "syntacticAnalyzer.h"

bool syntacticAnalyzer::initialization()
{
    std::string s;
    fin.open("sybmols.txt");
    if(!fin.is_open()) {
        std::cout << "Error with open sybmols.txt\n";
        return false;
    }
    while(!fin.eof()) {
        fin >> s;
        Symbols.push_back(s);
    }
    fin.close();
    sz = 1024;
    lineNumber = 0;
    buf = new char[sz];
    index = 0;
    return true;
}


TokenType getType(char ch)
{
    switch(ch) {
    case'*':
        return Prod;
    case'/':
        return Delen;
    case'+':
        return Plus;
    case'-':
        return Minus;
    case'=':
        return Eq;
    case'(':
        return lS;
    case')':
        return rS;
    case';':
        return zDot;
    case'{':
        return lK;
    case'}':
        return rK;
    case '>':
        return Larger;
    case '<':
        return Less;
    default:
        return Err;
    }
}

TokenType getType(std::string s)
{
    if(s=="==")
        return Ravno;
    else if(s=="&&")
        return And;
    else if(s=="||")
        return Or;
    else
        return Err;
}


std::string getTokenType(const Token& t)
{
    switch(t.tp) {
    case Symbol:
        return "Symbol";
    case String:
        return "String";
    case Int:
        return "int";
    case Float:
        return "float";
    case Err:
        return "Err";
    case Iden:
        return "Iden";
    case Prod:
        return "*";
    case Delen:
        return "/";
    case Eq:
        return "=";
    case rS:
        return ")";
    case lS:
        return "(";
    case Plus:
        return "+";
    case Minus:
        return "-";
    case zDot:
        return ";";
    case lK:
        return "{";
    case rK:
        return "}";
    case Or:
        return "or";
    case And:
        return "and";
    case Ravno:
        return "==";
    case Larger:
        return ">";
    case Less:
        return "<";
    case While:
        return "while";
    case If:
        return "if";
    case Else:
        return "else";
    case Print:
        return "print";
    default:
        return "Unknow";
    }
}


char syntacticAnalyzer::getChar()
{
    ++index;
    return buf[index-1];
}


void syntacticAnalyzer::putback()
{
    --index;
}


bool syntacticAnalyzer::analiz(std::string filename)
{
    results.clear();
    fin.open(filename);
    lineNumber = 0;
    if(!fin.is_open()) {
        std::cout << "Error with open " << filename << std::endl;
        return false;
    }
    parsing();
    fin.close();
    auto vec = getResult();
    for(auto t : vec) {
        if(t.tp==Err||t.tp==Unknown) {
            std::cout << "Error token:" << t.value<<std::endl;
            return false;
        }
    }
    return true;
}


bool syntacticAnalyzer::isSymbol(const char& ch) const
{
    std::string st = "";
    st += ch;
    for(auto s : Symbols) {
        if(s==st)
            return true;
    }
    return false;
}

void syntacticAnalyzer::push(const Token& t)
{
    if(lineNumber-results.size()>=2)
        lineNumber = results.size()+ 1;
    if(results.size()<lineNumber)
        results.push_back(std::vector<Token>());
    results[lineNumber-1].push_back(t);
}


void syntacticAnalyzer::loadLine()
{
    fin.getline(buf,sz);
    index = 0;
    ++lineNumber;
}

void syntacticAnalyzer::parsWord()
{
    std::string s = "";
    while(true) {
        char ch = getChar();
        if(isalpha(ch)||ch=='_')
            s += ch;
        else if(isdigit(ch)&&s!="")
            s += ch;
        else {
            putback();
            if(s=="while")
                push(Token(While));
            else if(s=="if")
                push(Token(If));
            else if(s=="else")
                push(Token(Else));
            else if(s=="print")
                push(Token(Print));
            else
                push(Token(s,Iden));
            return;
        }
    }
}



void syntacticAnalyzer::parsNumber()
{
    std::string s = "";
    bool nd = true;
    while(true) {
        char ch = getChar();
        if(isdigit(ch))
            s += ch;
        else if(ch=='.') {
            if(nd) {
                s += ch;
                nd = false;
            }
            else {
                putback();
                push(Token(s,Float));
                return;
            }
        }
        else {
            putback();
            if(nd)
                push(Token(s,Int));
            else
                push(Token(s,Float));
            return;
        }
    }
}



void syntacticAnalyzer::parsString()
{
    std::string s = "";

        char ch = getChar();
        if(ch=='"')
            s += ch;
        else {
            putback();
            return;
        }
        while(true) {
            ch = getChar();

            if(ch=='"') {
                s += ch;
                push(Token(s,String));
                return;
            }
            else if(ch=='\0') {
                if(fin.eof()) {
                    putback();
                    push(Token(s,Err));
                    return;
                }
                else {
                    loadLine();
                }
            }
            else if(ch=='\n')
                continue;
            else
                s += ch;
        }
}


void syntacticAnalyzer::parsSymbol()
{
    char ch = getChar();
    switch(ch) {
        case'+':case'-':case'*':case'/':case'(':case')':case';':case'{':case'}':case'>':case'<': {
            push(Token(getType(ch)));
            break;
        }
        case'|':case'&': {
            char ch1 = getChar();
            if(ch1==ch) {
                std::string s = "";
                s += ch;
                s += ch1;
                push(Token(getType(s)));
                return;
            }
            else {
                putback();
                push(Token(ch,Err));
                return;
            }
            break;
        }
        case'=': {
            char ch1 = getChar();
            if(ch1==ch) {
                std::string s = "";
                s += ch;
                s += ch1;
                push(Token(Ravno));
                return;
            }
            else {
                putback();
                push(Token(getType(ch)));
                return;
            }
            break;
        }
    }
}


void syntacticAnalyzer::parsing()
{
    loadLine();
    while(true) {
        char ch = getChar();
        if(isalpha(ch)||ch=='_') {
            putback();
            parsWord();
        }
        else if(isdigit(ch)) {
            putback();
            parsNumber();
        }
        else if(ch=='"') {
            putback();
            parsString();
        }
        else if(isSymbol(ch)) {
            putback();
            parsSymbol();
        }
        else if(ch=='\0') {
            if(fin.eof())
                return;
            else {
                loadLine();
            }
        }
        else if(ch==' '||ch=='\t')
            continue;
        else {
            push(Token(ch,Err));
        }
    }
}



void syntacticAnalyzer::showResults() const
{
    for(unsigned int i = 0;i<results.size();++i) {
        std::cout << "Stroke "<< i <<":\n";
        for(auto t : results[i])
            std::cout << "Type: " << getTokenType(t) << "  Value: " << t.value << std::endl;
        std::cout << "------------------------\n";
    }
}

std::vector<Token> syntacticAnalyzer::getResult() const
{
    std::vector<Token> res;
    for(auto vec : results) {
        for(auto t : vec)
            res.push_back(t);
    }
    return res;
}


