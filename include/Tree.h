#ifndef TREE_H
#define TREE_H
#include "syntacticAnalyzer.h"
#include <string>
#include <vector>
#include <iostream>

enum NodeType { Exp, Term,Factor,Def, Fact,Pow};


std::string getStringNodeType(NodeType t);



struct Node {
    std::string val;
    NodeType tp;
    Node* left;
    Node* right;
    Node()
    {
        left = right  = nullptr;
        val = "";
        tp = Def;
    }
    Node(std::string s,NodeType t)
    {
        left = right = nullptr;
        val = s;
        tp = t;
    }
    Node(NodeType t)
    {
        left = right =  nullptr;
        val = "";
        tp = t;
    }
};


class Tree
{
    public:
        Tree()
        {
            index = 0;
            head = new Node;
        }
        void getVector(std::vector<Token> v)
        {
            vec = v;
        }
        void makeTree();
        void showTree();
        void calcTree();
    private:
        double calc(Node* n);
        std::vector<Token> vec;
        std::string show(Node *n);
        unsigned int index;
        Node *head;
        Token getToken()
        {
            if(index==vec.size())
                return Token('e',End);
            ++index;
            return vec[index-1];
        }
        void putback()
        {
            --index;
        }
        Node* factor();
        Node* f();
        Node* term();
        Node* exp();
};

#endif // TREE_H
