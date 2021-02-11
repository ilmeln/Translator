#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H
#include "syntacticAnalyzer.h"
#include <vector>
#include <string>

enum NodeType { Factor,Term,Exp, Stmt,StmtList,Cicle};


struct Node {
    NodeType tp;
    Token tok;
    Node *left,*right;
    Node()
    {
        left = right = nullptr;
    }
    Node(NodeType type,Token t)
    {
        tp = type;
        tok = t;
        left = right = nullptr;
    }
    Node(NodeType t)
    {
        tp = t;
        left = right = nullptr;
    }
};


class semanticAnalyzer
{
    public:
        semanticAnalyzer()
        {
            index = 0;
            head = new Node;
        }
        void getTokenVector(std::vector<Token> v)
        {
            tokens = v;
            tokens.push_back(Token(End));
        }
        void makeTree()
        {
            index = 0;
            head = stmt();
        }
        void showTree();
        void calcTree()
        {
            std::cout << "\nCalc:" << calc(head) <<std::endl;
        }
    private:
        std::vector<Token> tokens;
        unsigned int index;
        std::vector<std::string> Vars;
        Node* head;
        Token getToken()
        {
            ++index;
            //std::cout << "Token: " << getTokenType(tokens[index-1]) <<std::endl;
            return tokens[index-1];
        }
        void putback()
        {
            --index;
        }
        Node* factor();
        Node* term();
        Node* exp();
        Node* stmt();
        std::string show(Node* n);
        double calc(Node *n);
};

#endif // SEMANTICANALYZER_H
