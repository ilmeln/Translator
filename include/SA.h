#ifndef SA_H
#define SA_H
#include "syntacticAnalyzer.h"
#include <vector>
#include <string>
#include <iostream>

enum NodeType { Factor,Term,Exp, Stmt,StmtList,logExp,addExp,Empty,WHILE,StmtExp,IF,IFELSE,ELSE,SKOBE,PRINT,Statements};



struct Node {
    NodeType Type;
    Token tok;
    Node* nodes;
    Node()
    {
        nodes = nullptr;
    }
    Node(int n)
    {
        nodes = new Node[n];
    }
    Node(NodeType tp,Token t)
    {
        Type = tp;
        tok = t;
        nodes = nullptr;
    }
    Node(NodeType tp,Token t,int n)
    {
        Type = tp;
        tok = t;
        nodes = new Node[n];
    }
    void push(int i,Node* n)
    {
        *(nodes+i) = *n;
    }
    Node(NodeType tp,int n)
    {
        Type = tp;
        nodes = new Node[n];
    }
    ~Node()
    {
        delete[] nodes;
    }
};


std::string NodeTypeToString(NodeType t);


struct Var {
    TokenType type;
    std::string name;
    Var(TokenType t,std::string s)
    {
        type = t;
        name = s;
    }
};


class SA
{
    public:
        SA()
        {
            index = tab = 0;
            Error =  false;

            head = new Node;
        }
        void getTokenVector(std::vector<Token> v)
        {
            tokens = v;
            tokens.push_back(Token(End));
        }
        void analiz()
        {
            head = stmtList();
            if(Error)
                std::cout << "Error:" <<errorMessage;
        }
        void showTree()
        {
            if(Error)
                return;
            std::cout << show(head,0) <<std::endl;
        }
        void inPython();
    private:
        std::vector<Token> tokens;
        unsigned int index;
        std::vector<Var> vars;
        Node *head;
        bool Error;
        std::string errorMessage;
        int tab;
        void error_f(std::string s);
        Token getToken()
        {
            ++index;
            return tokens[index-1];
        }
        void putBack()
        {
            --index;
        }
        Node* factor();
        Node* term();
        Node* additive_exp();
        Node* log_exp();
        Node* exp();
        Node* stmt();
        Node* stmtList();
        Node* stmtExp();
        TokenType check(Node* n);
        std::string toPython(Node* n);
        std::string show(Node *n,int t);
};

#endif // SA_H
