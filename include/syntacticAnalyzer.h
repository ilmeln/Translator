#ifndef  SYNTACTICANALYZER_H
#define  SYNTACTICANALYZER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

enum TokenType {Symbol, Float,Iden, Int, String,Err,Unknown,End,Plus,Minus,Prod,Delen,rS,lS,Eq,zDot,rK,lK,And,Or,Less,Larger,Ravno,While,If,Else,Print};


struct Token {
    std::string value;
    TokenType tp;
    Token()
    {
        value = "";
        tp = Unknown;
    }
    Token(const std::string& val,const TokenType& type)
    {
        value = val;
        tp = type;
    }
    Token(const char& ch,const TokenType& type)
    {
        value = "";
        value += ch;
        tp = type;
    }
    Token(const TokenType& t)
    {
        value = "";
        tp = t;
    }
};


std::string getTokenType(const Token& t);

TokenType getType(char ch);


class syntacticAnalyzer
{
    public:
        syntacticAnalyzer() {}
        bool initialization();
        bool analiz(std::string filename);
        void showResults() const;
        std::vector<Token> getResult() const;
        ~syntacticAnalyzer() { delete[] buf;  }
    private:
        std::vector<std::string> Symbols;
        std::vector<std::vector<Token>> results;
        std::ifstream fin;
        unsigned int sz;
        unsigned int lineNumber;
        char* buf;
        int index;
        void parsing();
        bool isSymbol(const char& ch) const;
        void push(const Token& t);
        void loadLine();
        char getChar();
        void putback();
        void parsWord();
        void parsNumber();
        void parsString();
        void parsSymbol();

};

#endif //  SYNTACTICANALYZER_H
